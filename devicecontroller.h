#ifndef DEVICECONTROLLER_H
#define DEVICECONTROLLER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class DeviceController
{
public:
    DeviceController();

private:
    std::shared_ptr<spdlog::logger> log;
};

#endif