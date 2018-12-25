#ifndef TOUCHSLIDER_H
#define TOUCHSLIDER_H

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <functional>
#include <libsoc-cpp/gpio.h>
#include <libsoc-cpp/i2c.h>

class TouchSlider
{
public:
    TouchSlider();
    ~TouchSlider();
    void onRightFlick(std::function<void()> callback);
    void onLeftFlick(std::function<void()> callback);
    void onTap(std::function<void()> callback);

private:
    libsoc::I2c i2c;
    libsoc::Gpio ready;
    std::shared_ptr<spdlog::logger> log;

    std::function<void()> rightFlickCallback;
    std::function<void()> leftFlickCallback;
    std::function<void()> tapCallback;

    void lowPowerMode();
    static int eventTriggered(void* self);

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
        ForceCommunication(libsoc::Gpio& interrupt)
            : interrupt(interrupt)
        {
            interrupt.setDirection(libsoc::Direction::OUTPUT);
            interrupt.write(libsoc::Level::LOW);
        }
        ~ForceCommunication()
        {
            interrupt.setDirection(libsoc::Direction::INPUT);
        }
        libsoc::Gpio& interrupt;
    };
};

#endif