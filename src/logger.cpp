#include <logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

Logger::Logger()
{
    logger = spdlog::stdout_color_mt("devices");
    spdlog::set_pattern("[%T-%ems] %v");
}