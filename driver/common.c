#include "common.h"
#include "lcd/driver_lcd.h"
#include "touch/driver_touch.h"

#include "hardware/gpio.h"
#include "hardware/spi.h"

bool spi_initialized = false;

/**
 * Initialize SPI for LCD and Touch
 */
void common_driver_init_spi()
{
    if (spi_initialized) return;
    gpio_init(PIN_SPI_SCLK);
    gpio_init(PIN_SPI_MOSI);
    gpio_init(PIN_SPI_MISO);
    gpio_set_function(PIN_SPI_SCLK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SPI_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SPI_MISO, GPIO_FUNC_SPI);

    spi_init(spi1, SPI_SPEED_LCD);
    spi_set_format(spi1, 8, 0, 0, SPI_MSB_FIRST);
    spi_initialized = true;
}

/**
 * Toggle SPI speed depending on selected device
 */
void common_driver_spi_switch_speed(const uint cs_num_gpio)
{
    switch (cs_num_gpio)
    {
    case PIN_TC_SPI_CS:
        spi_set_baudrate(spi1, SPI_SPEED_TOUCH);
        break;
    case PIN_LCD_SPI_CS:
    default:
        spi_set_baudrate(spi1, SPI_SPEED_LCD);
        break;
    }
}

/**
 * Send a byte via SPI
 */
void common_driver_spi_send(const uint cs_num_gpio, const uint8_t byte)
{
    common_driver_spi_switch_speed(cs_num_gpio);
    gpio_put(cs_num_gpio, false);
    spi_write_blocking(spi1, &byte, 1);
    gpio_put(cs_num_gpio, true);
}

/**
 * Send 2 bytes (halfword) via SPI
 */
void common_driver_send16(const uint cs_num_gpio, const uint16_t hword)
{
    uint8_t high = (uint8_t)(hword >> 8);
    uint8_t low = (uint8_t)(hword & 0xff);
    uint8_t buf8[2] = {high, low};
    common_driver_spi_switch_speed(cs_num_gpio);
    gpio_put(cs_num_gpio, false);
    spi_write_blocking(spi1, buf8, 2);
    gpio_put(cs_num_gpio, true);
}