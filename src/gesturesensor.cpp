#include <gesturesensor.h>
#include <libsoc-cpp/types.h>

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
    : i2c(I2C_BUS, DEVICE_ADDR), interrupt(INT)
{
    // Setup Logger
    log = spdlog::stdout_color_mt("Gestures");

    log->info("Configuring I2C with address 0x{0:x}", DEVICE_ADDR);
    if (!i2c.isOpen())
    {
        log->error("Device address invalid");
    }

    using namespace libsoc;
    interrupt.setDirection(Direction::INPUT);
    interrupt.setInterrupt(Edge::FALLING, intCallback, this);

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
    i2c.writeRegByte(EN, 0b01000100 + static_cast<uint8_t>(mode));
}

void GestureSensor::enableGestures()
{
    // Clears gesture FIFO, enables interrupt
    i2c.writeRegByte(GCONF4, 0b00000'1'1'0);
    i2c.writeByte(AICLEAR);
}

void GestureSensor::setupADC()
{
//    float time = 256 - (10 / 2.78);
    i2c.writeRegByte(ATIME, 219);

    i2c.writeRegByte(CTRL1, 0b0000'10'01);
}

void GestureSensor::setupGestureRegisters()
{
    // Set FIFO Threshold to 4 datasets
    i2c.writeRegByte(GCONF1, 0b01'0000'10);
    
    // Sets gain to 4, LED drive strength to 100mA, wait time to 0ms
    i2c.writeRegByte(GCONF2, 0b0'10'00'000);

    // Sets pulse length to 16us, with 10 pulses
    i2c.writeRegByte(GPULSE, 0b10'001001);

    // Sets pulse length to 16us, with 10 pulses
    i2c.writeRegByte(PPULSE, 0b10'000101);

    // Enables all gesture dimensions
    i2c.writeRegByte(GCONF3, 0x00);

    // Set promixity threshold to 50
    i2c.writeRegByte(GPENTH, 20);
    // i2c.writeRegByte(GEXTH, 30);

    // Clears gesture FIFO, enables interrupt
    i2c.writeRegByte(GCONF4, 0b00000'1'1'0);

    i2c.writeRegByte(PERS, 0b0001'0001);

    // enableGestures();
}

// When proximity sensor detects hand over sensor, interrupt is
// triggered, and gesture reading should begin
int GestureSensor::intCallback(void* data)
{
    GestureSensor* gc = reinterpret_cast<GestureSensor*>(data);
    gc->readGestureData();
    gc->enableGestures();
}

void GestureSensor::readGestureData()
{
    uint8_t dataCounts = i2c.readRegByte(GFLVL);
    std::array<uint8_t, 128> fifoBuffer{};

    i2c.readRegBytes(GFIFO_U, fifoBuffer.data(), dataCounts);
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
            gestureBuffer.add(Gesture::Direction::UP);
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
            gestureBuffer.add(Gesture::Direction::DOWN);
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
            gestureBuffer.add(Gesture::Direction::LEFT);
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
            gestureBuffer.add(Gesture::Direction::RIGHT);
            // swipeManager->moveToRight();
            log->info("RIGHT gesture detected");
            detects.reset();
            sleep(200);
        }
        else detects.right++;
    }
}
