#ifndef FINGERPRINTSENSOR_H
#define FINGERPRINTSENSOR_H

#include <mraa.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class FingerprintSensor
{
public:
    FingerprintSensor();

private:
    mraa::Spi spi;
    mraa::Gpio reset;
    mraa::Gpio interrupt;
    std::shared_ptr<spdlog::logger> log;
    enum class State
    {
        WAITING_FOR_FINGER,
        WAITING_FOR_IMAGE_DATA
    } state;

    void setupSPI();
    void setupGPIO();
    void checkHW();
    void setupRegisters();
    static void intCallback(void* instance);
};

#endif