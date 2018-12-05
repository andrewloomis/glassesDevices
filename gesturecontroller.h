#ifndef GESTURECONTROLLER_H
#define GESTURECONTROLLER_H

#include <usertypes.h>
#include <mraa.hpp>
#include <array>
#include <vector>
#include <memory>

class GestureController
{
public:
    GestureController();
    ~GestureController();
    Gesture getLatestGesture() const { return gestureBuffer.getLatest(); }

private:
    mraa::I2c i2c;
    mraa::Gpio interrupt;
    GestureBuffer gestureBuffer;
    Direction lastDetect = Direction::NONE;

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
    static void intCallback(void*);
    void readGestureData();
    void parseFifoData(const std::array<uint8_t, 128>& data, uint8_t dataCount);
    void powerMode(PowerMode mode);

    void setupGestureRegisters();
    void setupADC();
    void sleep(int ms);
};

#endif
