#ifndef BACKLIGHT_H
#define BACKLIGHT_H

#include <fstream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class Backlight
{
public:
    Backlight(float intensity);
    ~Backlight();
    void on();
    void off();
    int increaseBrightness();
    int decreaseBrightness();

    /// @param intensity percent (0-100) in increments of 10
    void setBrightness(int intensity);

private:
    std::shared_ptr<spdlog::logger> log;
    std::ofstream backlight;
    float lastIntensity = 0;
};

#endif