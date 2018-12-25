#ifndef FINGERPRINTSENSOR_H
#define FINGERPRINTSENSOR_H

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <libsoc-cpp/spi.h>
#include <libsoc-cpp/gpio.h>

class FingerprintSensor
{
public:
    FingerprintSensor();

private:
    libsoc::Spi spi;
    libsoc::Gpio reset;
    libsoc::Gpio interrupt;
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
    static int intCallback(void* instance);
};

#endif