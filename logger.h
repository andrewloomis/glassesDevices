#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>
#include <memory>

class Logger
{
public:
    Logger();
    void info();
    std::shared_ptr<spdlog::logger> getLogger() { return logger; }

private:
    std::shared_ptr<spdlog::logger> logger;
};

#endif