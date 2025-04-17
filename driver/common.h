#ifndef DRIVER_COMMON_H
#define DRIVER_COMMON_H

#include "pico/types.h"

#define PIN_SPI_SCLK 10
#define PIN_SPI_MOSI 11
#define PIN_SPI_MISO 12

#define SPI_SPEED_LCD 60 * 1000 * 1000 // 60 MHz
#define SPI_SPEED_TOUCH 25 * 100 * 1000 // 2,5 MHz

void common_driver_init_spi();
void common_driver_spi_send(const uint cs_num_gpio, const uint8_t byte);
void common_driver_send16(const uint cs_num_gpio, const uint16_t hword);

#endif // DRIVER_COMMON_H
