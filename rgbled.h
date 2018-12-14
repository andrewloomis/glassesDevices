#ifndef RGBLED_H
#define RGBLED_H

#include <mraa.hpp>
#include <inttypes.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class RGBLed
{
public:
    RGBLed();
    void setColor(int red, int green, int blue);

private:
    mraa::I2c i2c;
    std::shared_ptr<spdlog::logger> log;
    int variableColorCount = 0;

    enum class LED
    {
        Red = 0,
        Green,
        Blue
    };

    void setPWMFreq(float freq);
    void setPWMDutyCycle(float dutyCycle);
    void setIntensity(LED led, int intensity);
};

#endif // RGBLED_H
