#include <touchslider.h>
#include <iostream>

#define I2C_BUS 0
#define DEVICE_ADDR 0x44
#define READY_PIN 21

// Registers
#define SYSFLAGS0 0x01
#define PROXSETTTINGS 0x09

TouchSlider::TouchSlider()
    : i2c(I2C_BUS), ready(READY_PIN)
{
    // Setup Logger
    log = spdlog::stdout_color_mt("TouchSlider");
    log->set_pattern("[%T-%ems] [%n] %v");

    log->info("Configuring I2C with address {0:h}", DEVICE_ADDR);
    i2c.frequency(mraa::I2cMode::I2C_STD);
    if (i2c.address(DEVICE_ADDR) != mraa::Result::SUCCESS)
    {
        log->error("Device address invalid");
    }
    ready.dir(mraa::DIR_IN);
    ready.isr(mraa::Edge::EDGE_FALLING, eventTriggered, this);

    // Set projection mode to "self"
    i2c.writeReg(SYSFLAGS0, 0x00);

    // ProxSettings
    // > WDT Disable
    // > Event Mode
    // > 3CH Slider
    const uint8_t proxSettings[4] = {0x00, 0xDC, 0x00, 0x00};
    i2c.writeByte(PROXSETTTINGS);
    i2c.write(proxSettings, sizeof(proxSettings));
}

TouchSlider::~TouchSlider()
{
    lowPowerMode();
}

void TouchSlider::lowPowerMode()
{
    ForceCommunication fc(ready);
    i2c.writeReg(SYSFLAGS0, 0x01);
}

void TouchSlider::eventTriggered(void* data)
{
    TouchSlider* self = reinterpret_cast<TouchSlider*>(data);
    ForceCommunication fc(self->ready);

    uint8_t flags[2];
    self->i2c.readBytesReg(SYSFLAGS0, flags, 2);

    auto event = static_cast<Events>(flags[1]);
    switch (event)
    {
    case Events::FlickRight:
        break;
    case Events::FlickLeft:
        break;
    default:
        break;
    }
}