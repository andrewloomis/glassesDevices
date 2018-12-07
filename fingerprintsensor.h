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
    std::shared_ptr<spdlog::logger> log;
};

#endif