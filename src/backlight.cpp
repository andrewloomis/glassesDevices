#include <backlight.h>

Backlight::Backlight(bool startOn)
    : backlight("/sys/class/backlight/backlight/brightness")
{
    // Setup Logger
    log = spdlog::stdout_color_mt("Backlight");

    if (!backlight.is_open())
    {
        log->error("Failed to open backlight sysfs interface!");
    }
    else
    {
        if (startOn) on();
        else off();
    }
}

void Backlight::on()
{
    log->info("Turning on");
    backlight << "1";
}

void Backlight::off()
{
    log->info("Turning off");
    backlight << "0";
}

Backlight::~Backlight()
{
    off();
}