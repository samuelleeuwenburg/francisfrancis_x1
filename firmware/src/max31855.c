#include "hardware/spi.h"
#include "max31855.h"

#define SPI_SCK_PIN 18
#define SPI_RX_PIN 16
#define SPI_CSN_PIN 17
#define CLOCK_SPEED 500000

max31855 max31855_new() {
    max31855 m = {
        .temperature = 0.0,
        .reference_temperature = 0.0,
        .errors = 0
    };

    return m;
}

void max31855_init(max31855 *m) {
    spi_init(spi0, CLOCK_SPEED);
    gpio_set_function(SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_SCK_PIN, GPIO_FUNC_SPI);

    gpio_init(SPI_CSN_PIN);
    gpio_set_dir(SPI_CSN_PIN, GPIO_OUT);

    // Pull the chip select high
    gpio_put(SPI_CSN_PIN, 1);
}


static float parse_thermocouple_temperature(const uint8_t buffer[4]) {
    // Add the sign
    int16_t thermocouple_data = (buffer[0] & 0x80) << 8;
    // Drop 2 LSB (reserved and fault bit) treat the rest as the LSB data 6 bits
    thermocouple_data += (buffer[1] >> 2);
    // Drop the sign, move the rest up
    thermocouple_data += ((buffer[0] & 0x7f) << 6);
    // Multiply times the celcius step of the datasheet
    return thermocouple_data * 0.25;
}

static float parse_reference_temperature(const uint8_t buffer[4]) {
    // Add the sign
    int16_t reference_data = (buffer[2] & 0x80) << 8;
    // Drop 4 LSB
    reference_data += buffer[3] >> 4;
    // Drop the sign, move the rest up
    reference_data += (buffer[2] & 0x7f) << 4;
    // Multiply times the celcius step of the datasheet
    return reference_data * 0.0625;
}

static uint8_t parse_errors(const uint8_t buffer[4]) {
    uint8_t errors = 0;
    // Take only 3 LSB
    errors += buffer[3] & 0x7;
    // And add the fault bit
    errors += (buffer[1] & 0x1) << 3;
    return errors;
}

void max31855_read(max31855 *m) {
    uint8_t buffer[4];

    // Set the chip select low
    gpio_put(SPI_CSN_PIN, 0);

    spi_read_blocking(spi0, 0, buffer, sizeof(buffer));

    m->temperature = parse_thermocouple_temperature(buffer);
    m->reference_temperature = parse_reference_temperature(buffer);
    m->errors = parse_errors(buffer);

    // Set the chip select high
    gpio_put(SPI_CSN_PIN, 1);
}
