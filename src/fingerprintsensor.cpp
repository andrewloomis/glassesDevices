#include <fingerprintsensor.h>

#define SPI_BUS 0

FingerprintSensor::FingerprintSensor()
    : spi(0)
{
    // Setup Logger
    log = spdlog::stdout_color_mt("Fingerprint");

    spi.mode(mraa::Spi_Mode::SPI_MODE0);

    // 12 Mbit/s max supported for sensor
    const int freq = 5'000'000;
    spi.frequency(freq); //5MHz
    log->info("Configuring SPI at {}MHz", freq / 1'000'000);
    spi.bitPerWord(8);
}