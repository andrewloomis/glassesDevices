#ifndef BACKLIGHT_H
#define BACKLIGHT_H

#include <fstream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class Backlight
{
public:
    Backlight(bool startOn);
    ~Backlight();
    void on();
    void off();

private:
    std::shared_ptr<spdlog::logger> log;
    std::ofstream backlight;
};

#endif