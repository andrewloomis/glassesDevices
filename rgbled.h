#ifndef RGBLED_H
#define RGBLED_H

#include <mraa.hpp>
#include <inttypes.h>

class RGBLed
{
public:
    RGBLed();
    void setColor(int red, int green, int blue);

private:
    mraa::I2c i2c;
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
