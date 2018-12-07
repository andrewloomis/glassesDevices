#ifndef TOUCHSLIDER_H
#define TOUCHSLIDER_H

#include <mraa.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class TouchSlider
{
public:
    TouchSlider();
    ~TouchSlider();

private:
    mraa::I2c i2c;
    mraa::Gpio ready;
    std::shared_ptr<spdlog::logger> log;

    void lowPowerMode();
    static void eventTriggered(void* self);

    enum class Events
    {
        Prox = 1 << 0,
        Touch = 1 << 1,
        Slide = 1 << 2,
        ATI = 1 << 3,
        Movement = 1 << 4,
        Tap = 1 << 5,
        FlickLeft = 1 << 6,
        FlickRight = 1 << 7
    };

    class ForceCommunication
    {
    public:
        ForceCommunication(mraa::Gpio interrupt)
            : interrupt(interrupt)
        {
            interrupt.dir(mraa::DIR_OUT_LOW);
        }
        ~ForceCommunication()
        {
            interrupt.dir(mraa::DIR_IN);
        }
        mraa::Gpio& interrupt;
    };
};

#endif