#ifndef RGBLED_H
#define RGBLED_H

#include <inttypes.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <libsoc-cpp/i2c.h>

class RGBLed
{
public:
    RGBLed();
    void setColor(int red, int green, int blue);

private:
    libsoc::I2c i2c;
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
