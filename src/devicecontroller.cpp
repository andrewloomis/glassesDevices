#include <devicecontroller.h>

DeviceController::DeviceController()
{
    // Setup Logger
    log = spdlog::stdout_color_mt("Devices");
    spdlog::set_pattern("[%T %ems] %^[%n] %v%$");
}