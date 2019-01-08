#include <backlight.h>

Backlight::Backlight(float intensity)
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
        setBrightness(intensity);
    }
}

void Backlight::setBrightness(int intensity)
{
    switch (intensity)
    {
    case 0:
        backlight << "0";
        break;
    case 10:
        backlight << "1";
        break;
    case 20:
        backlight << "2";
        break;
    case 30:
        backlight << "3";
        break;
    case 40:
        backlight << "4";
        break;
    case 50:
        backlight << "5";
        break;
    case 60:
        backlight << "6";
        break;
    case 70:
        backlight << "7";
        break;
    case 80:
        backlight << "8";
        break;
    case 90:
        backlight << "9";
        break;
    case 100:
        backlight << "10";
        break;
    default:
        backlight << "0";
    }
    lastIntensity = intensity;
}

void Backlight::on()
{
    if (lastIntensity == 0) lastIntensity = 100;
    log->info("Turning on");
    setBrightness(lastIntensity);
}

void Backlight::off()
{
    log->info("Turning off");
    setBrightness(0);
}

int Backlight::increaseBrightness()
{
    int newIntensity = lastIntensity + 10;
    setBrightness(newIntensity);
    return newIntensity;
}

int Backlight::decreaseBrightness()
{
    int newIntensity = lastIntensity - 10;
    setBrightness(newIntensity);
    return newIntensity;
}

Backlight::~Backlight()
{
    off();
}