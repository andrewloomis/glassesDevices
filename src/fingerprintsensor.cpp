#include <fingerprintsensor.h>

#define SPI_BUS 0
#define RESET_GPIO 22
#define INT_GPIO 23

// Commands
#define WAIT_FOR_FINGER 0x24
// #define FINGER_PRESENT 0x20
#define READ_INT_CLEAR 0x1C
#define READ_IMAGE 0xC4
#define CAPTURE_IMAGE 0xC0

// Registers
#define FPC_ERROR 0x38
#define FNGR_PRESENT_STATUS 0xD4
    #define FNGR_PRESENT 0x0FFF
#define HW_ID 0xFC

// Errors
#define FIFO_UNDERFLOW 1

// Interrupts
#define CMD_DONE 1<<7
#define NEW_DATA 1<<5
#define INT_ERROR 1<<2
#define FINGER_DOWN 1

FingerprintSensor::FingerprintSensor()
    : spi(SPI_BUS), reset(RESET_GPIO), interrupt(INT_GPIO)
{
    // Setup Logger
    log = spdlog::stdout_color_mt("Fingerprint");

    setupSPI();
    setupGPIO();
    checkHW();
    setupRegisters();
}

void FingerprintSensor::setupSPI()
{
    spi.mode(mraa::Spi_Mode::SPI_MODE0);

    // 12 Mbit/s max supported for sensor
    const int freq = 5'000'000;
    spi.frequency(freq); //5MHz
    log->info("Configuring SPI at {}MHz", freq / 1'000'000);
    spi.bitPerWord(8);
}

void FingerprintSensor::setupGPIO()
{
    interrupt.dir(mraa::DIR_IN);
    interrupt.isr(mraa::Edge::EDGE_RISING, intCallback, this);

    reset.dir(mraa::DIR_OUT_LOW);
}

void FingerprintSensor::checkHW()
{
    uint8_t cmd = READ_INT_CLEAR;
    uint8_t hw;
    spi.transfer(&cmd, &hw, 1);
    if ((hw >> 4) & 0x0FFF == 0x20 && (hw & 0x000F) == 0xA)
    {
        log->info("Hardware authenticated");
    }
    else
    {
        log->error("Incorrect fingerprint sensor");
    }
}

void FingerprintSensor::setupRegisters()
{
    // oscTrim is default 16kHz

    spi.writeByte(WAIT_FOR_FINGER);
    state = State::WAITING_FOR_FINGER;
}

void FingerprintSensor::intCallback(void* instance)
{
    auto self = reinterpret_cast<FingerprintSensor*>(instance);

    uint8_t cmd[2] = {READ_INT_CLEAR, 0x00};
    uint8_t rx[2];
    self->spi.transfer(cmd, rx, 2);
    uint8_t interrupt = rx[1];

    if(interrupt & INT_ERROR)
    {
        self->log->error("Interrupt error");
        uint8_t reg = FPC_ERROR;
        uint8_t error;
        self->spi.transfer(&reg, &error, 1);
        if (error & FIFO_UNDERFLOW)
        {
            self->log->error("-> Fifo underflow. Attempted to read data when image FIFO was empty.");
        }
        else
        {
            self->log->error("-> Unknown error type");
        }
    }
    switch(self->state)
    {
    case State::WAITING_FOR_FINGER:
        if (interrupt & FINGER_DOWN)
        {
            uint8_t cmd[3] = {FNGR_PRESENT_STATUS, 0, 0};
            uint8_t status[3];
            self->spi.transfer(cmd, status, 3);
            if ((status[1] << 8 | status[2]) & FNGR_PRESENT)
            {
                self->log->info("Finger on sensor, capturing image");
                self->spi.writeByte(CAPTURE_IMAGE);
                self->state = State::WAITING_FOR_IMAGE_DATA;
            }
        }
        break;
    case State::WAITING_FOR_IMAGE_DATA:
        if (interrupt & NEW_DATA)
        {
            constexpr unsigned int size = 192*192 + 2;
            std::array<uint8_t, size> cmd {};
            // std::fill(cmd.begin(), cmd.end(), 0);
            cmd[0] = READ_IMAGE;
            
            std::array<uint8_t, size> rx;
            self->log->info("Reading image of {} bytes", size);
            self->spi.transfer(cmd.data(), rx.data(), size);
            self->state = State::WAITING_FOR_FINGER;
        }
        break;
    default:
        self->log->error("Unknown state");
    }
}