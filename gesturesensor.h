#ifndef GESTURESENSOR_H
#define GESTURESENSOR_H

#include <usertypes.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <array>
#include <vector>
#include <memory>
#include <libsoc-cpp/i2c.h>
#include <libsoc-cpp/gpio.h>

// APDS-9960 Sensor
class GestureSensor
{
public:
    GestureSensor();
    ~GestureSensor();
    Gesture::Gesture getLatestGesture() const { return gestureBuffer.getLatest(); }

private:
    libsoc::I2c i2c;
    libsoc::Gpio interrupt;
    Gesture::Buffer gestureBuffer;
    Gesture::Direction lastDetect = Gesture::Direction::NONE;
    std::shared_ptr<spdlog::logger> log;

    struct PreviousDetects
    {
        int up = 0;
        int down = 0;
        int right = 0;
        int left = 0;
        void reset()
        {
            up = 0;
            down = 0;
            right = 0;
            left = 0;
        }
    } detects;

    enum class PowerMode
    {
        OFF = 0,
        ON = 1
    };

    void enableGestures();
    static int intCallback(void*);
    void readGestureData();
    void parseFifoData(const std::array<uint8_t, 128>& data, uint8_t dataCount);
    void powerMode(PowerMode mode);

    void setupGestureRegisters();
    void setupADC();
    void sleep(int ms);
};

#endif
