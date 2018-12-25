#include <rgbled.h>
#include <iostream>

#define I2C_BUS 0
#define DEVICE_ADDR 0x62

// Registers
#define PSC0 0x01
#define PWM0 0x02
#define PSC1 0x03
#define PWM1 0x04
#define LS0 0x05

RGBLed::RGBLed()
    : i2c(I2C_BUS, DEVICE_ADDR)
{
    // Setup Logger
    log = spdlog::stdout_color_mt("LED");

    log->info("Configuring I2C with address 0x{0:x}", DEVICE_ADDR);

    // 100kHz
    if (!i2c.isOpen())
    {
        log->error("Device address invalid");
    }
}

void RGBLed::setColor(int red, int green, int blue)
{
    // Only 2 colors can have variable intensity
    setIntensity(LED::Red, red);
    setIntensity(LED::Green, green);
    setIntensity(LED::Blue, blue);
}

void RGBLed::setIntensity(LED led, int intensity)
{
    uint8_t option = 0;
    if (intensity <= 0)
    {
        option = 0b01;
    }
    else if (intensity > 254)
    {
        option = 0b00;
    }
    else
    {
        option = 0b10;
        setPWMDutyCycle(static_cast<float>(intensity)/255);
    }
    i2c.writeRegByte(LS0, option << static_cast<uint8_t>(led));
}

void RGBLed::setPWMFreq(float freq)
{
    // The period of BLINK0 = (PSC0 + 1) / 44
    uint8_t prescaler = static_cast<uint8_t>(44*(1/freq) - 1);
    i2c.writeRegByte(PSC0, prescaler);
}

// @param dutyCycle 0.0 to 1.0
void RGBLed::setPWMDutyCycle(float dutyCycle)
{
    // The duty cycle of BLINK0 = (256 − PWM0) / 256
    uint8_t pwm = static_cast<uint8_t>(256 - 256*(dutyCycle));
    i2c.writeRegByte(PWM0, pwm);
}
