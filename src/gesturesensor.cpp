#include <gesturesensor.h>
#include <iostream>
#include <chrono>
#include <thread>

// Pins
#define I2C_BUS 0
#define DEVICE_ADDR 0x39
#define INT 33

// Registers
#define EN 0x80
#define ATIME 0x81
#define PERS 0x8C
#define PPULSE 0x8E
#define CTRL1 0x8F
#define AICLEAR 0xE7
#define GCONF1 0xA2
#define GCONF2 0xA3
#define GCONF3 0xAA
#define GCONF4 0xAB
#define GPULSE 0xA6
#define GFLVL 0xAE
#define GSTATUS 0xAF
#define GFIFO_U 0xFC
#define GFIFO_D 0xFD
#define GFIFO_L 0xFE
#define GFIFO_R 0xFF
#define GPENTH 0xA0
#define GEXTH 0xA1

GestureSensor::GestureSensor()
    : i2c(I2C_BUS), interrupt(INT)
{
    // Setup Logger
    log = spdlog::stdout_color_mt("Gestures");
    log->set_pattern("[%T %ems] %^[%n] %v%$");

    log->info("Configuring I2C with address 0x{0:x}", DEVICE_ADDR);
    i2c.frequency(mraa::I2cMode::I2C_STD);
    if (i2c.address(DEVICE_ADDR) != mraa::Result::SUCCESS)
    {
        log->error("Device address invalid");
    }

    interrupt.dir(mraa::DIR_IN);
    interrupt.isr(mraa::Edge::EDGE_FALLING, intCallback, this);

    log->info("APDS-9960 setting up registers");
    setupADC();
    
    // Clear interrupts
    i2c.writeByte(AICLEAR);
    setupGestureRegisters();
    powerMode(PowerMode::ON);
    log->info("APDS-9960 powered on");
}

GestureSensor::~GestureSensor()
{
    log->info("Sleeping APDS-9960");
    powerMode(PowerMode::OFF);
}

void GestureSensor::powerMode(PowerMode mode)
{
    // Set GEN, PEN and PON to 1
    i2c.writeReg(EN, 0b01000100 + static_cast<uint8_t>(mode));
}

void GestureSensor::enableGestures()
{
    // Clears gesture FIFO, enables interrupt
    i2c.writeReg(GCONF4, 0b00000'1'1'0);
    i2c.writeByte(AICLEAR);
}

void GestureSensor::setupADC()
{
//    float time = 256 - (10 / 2.78);
    i2c.writeReg(ATIME, 219);

    i2c.writeReg(CTRL1, 0b0000'10'01);
}

void GestureSensor::setupGestureRegisters()
{
    // Set FIFO Threshold to 4 datasets
    i2c.writeReg(GCONF1, 0b01'0000'10);
    
    // Sets gain to 4, LED drive strength to 100mA, wait time to 0ms
    i2c.writeReg(GCONF2, 0b0'10'00'000);

    // Sets pulse length to 16us, with 10 pulses
    i2c.writeReg(GPULSE, 0b10'001001);

    // Sets pulse length to 16us, with 10 pulses
    i2c.writeReg(PPULSE, 0b10'000101);

    // Enables all gesture dimensions
    i2c.writeReg(GCONF3, 0x00);

    // Set promixity threshold to 50
    i2c.writeReg(GPENTH, 20);
    // i2c.writeReg(GEXTH, 30);

    // Clears gesture FIFO, enables interrupt
    i2c.writeReg(GCONF4, 0b00000'1'1'0);

    i2c.writeReg(PERS, 0b0001'0001);

    // enableGestures();
}

// When proximity sensor detects hand over sensor, interrupt is
// triggered, and gesture reading should begin
void GestureSensor::intCallback(void* data)
{
    GestureSensor* gc = reinterpret_cast<GestureSensor*>(data);
    // std::cout << "Interrupt\n";
    gc->readGestureData();
    gc->enableGestures();
}

void GestureSensor::readGestureData()
{
    uint8_t dataCounts = i2c.readReg(GFLVL);
    std::array<uint8_t, 128> fifoBuffer{};

    i2c.readBytesReg(GFIFO_U, fifoBuffer.data(), dataCounts);
    parseFifoData(fifoBuffer, dataCounts);
}

void GestureSensor::sleep(int milliseconds)
{
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void GestureSensor::parseFifoData(const std::array<uint8_t, 128>& data, uint8_t dataCount)
{
    int upDownDiff = data[0] - data[1];
    int leftRightDiff = data[2] - data[3];
    const int threshold = 13;
    
    if (upDownDiff < -threshold)
    {
        if (detects.down > 0) {
            gestureBuffer.add(Direction::UP);
            // swipeManager->moveToUp();
            log->info("UP gesture detected");
            detects.reset();
            sleep(200);
        }
        else detects.up++;
    }
    else if (upDownDiff > threshold)
    {
        if (detects.up > 0) {
            gestureBuffer.add(Direction::DOWN);
            // swipeManager->moveToDown();
            log->info("DOWN gesture detected");
            detects.reset();
            sleep(200);
        }
        else detects.down++;
    }
    if (leftRightDiff < -threshold)
    {
        if (detects.right > 0) {
            gestureBuffer.add(Direction::LEFT);
            // swipeManager->moveToLeft();
            log->info("LEFT gesture detected");
            detects.reset();
            sleep(200);
        }
        else detects.left++;
    }
    else if (leftRightDiff > threshold)
    {
        if (detects.left > 0) {
            gestureBuffer.add(Direction::RIGHT);
            // swipeManager->moveToRight();
            log->info("RIGHT gesture detected");
            detects.reset();
            sleep(200);
        }
        else detects.right++;
    }
}
