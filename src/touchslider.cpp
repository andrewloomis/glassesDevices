#include <touchslider.h>
#include <iostream>

#define I2C_BUS 0
#define DEVICE_ADDR 0x44
#define READY_PIN 21

// Registers
#define SYSFLAGS0 0x01
#define PROXSETTTINGS 0x09

TouchSlider::TouchSlider()
    : i2c(I2C_BUS, DEVICE_ADDR), ready(READY_PIN)
{
    // Setup Logger
    log = spdlog::stdout_color_mt("TouchSlider");

    log->info("Configuring I2C with address {0:x}", DEVICE_ADDR);
    if (!i2c.isOpen())
    {
        log->error("Device address invalid");
    }
    using namespace libsoc;
    ready.setDirection(Direction::INPUT);
    ready.setInterrupt(Edge::FALLING, eventTriggered, this);

    // Set projection mode to "self"
    i2c.writeRegByte(SYSFLAGS0, 0x00);

    // ProxSettings
    // > WDT Disable
    // > Event Mode
    // > 3CH Slider
    uint8_t proxSettings[4] = {0x00, 0xDC, 0x00, 0x00};
    i2c.writeByte(PROXSETTTINGS);
    i2c.write(proxSettings, sizeof(proxSettings));
}

TouchSlider::~TouchSlider()
{
    lowPowerMode();
}

void TouchSlider::lowPowerMode()
{
    log->info("IQS263 going to low power");
    ForceCommunication fc(ready);
    i2c.writeRegByte(SYSFLAGS0, 0x01);
}

int TouchSlider::eventTriggered(void* instance)
{
    TouchSlider* self = reinterpret_cast<TouchSlider*>(instance);
    ForceCommunication fc(self->ready);

    uint8_t flags[2];
    self->i2c.readRegBytes(SYSFLAGS0, flags, 2);

    auto event = static_cast<Events>(flags[1]);
    switch (event)
    {
    case Events::FlickRight:
        if (self->rightFlickCallback)
        {
            self->rightFlickCallback();
        }
        else self->log->error("Right flick callback not set!");
        break;
    case Events::FlickLeft:
        if (self->leftFlickCallback)
        {
            self->leftFlickCallback();
        }
        else self->log->error("Left flick callback not set!");
        break;
    case Events::Tap:
        if (self->tapCallback)
        {
            self->tapCallback();
        }
        else self->log->error("Tap callback not set!");
        break;
    default:
        self->log->warn("Event not recognized");
    }
}

void TouchSlider::onRightFlick(std::function<void()> callback)
{
    rightFlickCallback = callback;
}

void TouchSlider::onLeftFlick(std::function<void()> callback)
{
    leftFlickCallback = callback;
}

void TouchSlider::onTap(std::function<void()> callback)
{
    tapCallback = callback;
}