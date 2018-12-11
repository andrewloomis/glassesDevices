#include <devicecontroller.h>
#include <mraa/common.hpp>

DeviceController::DeviceController()
{
    // Setup Logger
    log = spdlog::stdout_color_mt("Devices");
    spdlog::set_pattern("[%T %ems] %^[%n] %v%$");

    // Configure MRAA library
    mraa::Result res = mraa::init();
    if (res != mraa::Result::SUCCESS)
    {
        log->error("MRAA init failed");
        exit(1);
    }
    else
    {
        log->info("Using MRAA {} on {}", mraa::getVersion(), 
            mraa::getPlatformName());
    }
}