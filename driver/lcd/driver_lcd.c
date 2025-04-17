#include "../common.h"
#include "driver_lcd.h"

#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/spi.h"
#include "pico/time.h"

/**
 * Helper function - returning the pwm level for the brightness value (0-10)
 */
uint16_t pwm_level_for_brightness(int x)
{
    if (x > 10) x = 10;
    return (uint16_t)((x / 10.0) * 65535);
}

/**
 * Initialize Backlight Pin with PWM
 */
void lcd_driver_init_backlight_pwm()
{
    gpio_init(PIN_LCD_BL);
    gpio_set_function(PIN_LCD_BL, GPIO_FUNC_PWM);
    
    uint slice = pwm_gpio_to_slice_num(PIN_LCD_BL);
    uint chan = pwm_gpio_to_channel(PIN_LCD_BL);
    pwm_set_wrap(slice, 65535);
    pwm_set_clkdiv(slice, 1.9073);
    pwm_set_chan_level(slice, chan, pwm_level_for_brightness(10));
    pwm_set_enabled(slice, true);
}

/**
 * Initialize LCD hardware reset pin
 */
void lcd_driver_init_reset()
{
    gpio_init(PIN_LCD_RST);
    gpio_set_function(PIN_LCD_RST, GPIO_FUNC_SIO);
    gpio_set_dir(PIN_LCD_RST, GPIO_OUT);
    gpio_put(PIN_LCD_RST, true);
    sleep_ms(150);
}

/**
 * Init LCD D/C pin
 */
void lcd_driver_init_dc()
{
    gpio_init(PIN_LCD_DC);
    gpio_set_function(PIN_LCD_DC, GPIO_FUNC_SIO);
    gpio_set_dir(PIN_LCD_DC, GPIO_OUT);
    gpio_put(PIN_LCD_DC, false);
}

/**
 * Init LCD CS pin
 */
void lcd_driver_init_cs()
{
    gpio_init(PIN_LCD_SPI_CS);
    gpio_set_function(PIN_LCD_SPI_CS, GPIO_FUNC_SIO);
    gpio_set_dir(PIN_LCD_SPI_CS, GPIO_OUT);
    gpio_put(PIN_LCD_SPI_CS, 1);
}

/**
 * Send an 8-bit command to LCD via SPI
 */
void lcd_driver_cmd(const uint8_t cmd)
{
    gpio_put(PIN_LCD_DC, false);
    common_driver_spi_send(PIN_LCD_SPI_CS, cmd);
}

/**
 * Send an 8-bit data to LCD via SPI
 */
void lcd_driver_data(const uint8_t data)
{
    gpio_put(PIN_LCD_DC, true);
    common_driver_spi_send(PIN_LCD_SPI_CS, data);
}

/**
 * Send an 16-bit pixel data to LCD via SPI
 */
void lcd_driver_pixel(const uint16_t data)
{
    gpio_put(PIN_LCD_DC, true);
    common_driver_send16(PIN_LCD_SPI_CS, data);
}

/**
 * Initialize LCD driver
 */
void lcd_driver_init()
{
    // initialize pins and communication
    lcd_driver_init_reset();
    lcd_driver_init_dc();
    lcd_driver_init_cs();
    lcd_driver_init_backlight_pwm();
    common_driver_init_spi();

    // initialize display
    lcd_driver_cmd(LCD_CMD_SLPOUT);
    sleep_ms(150);
    lcd_driver_cmd(LCD_CMD_NORON);
    lcd_driver_cmd(LCD_CMD_DISON);
    lcd_driver_cmd(LCD_CMD_INVON);

    lcd_driver_cmd(LCD_CMD_COLMOD);
    lcd_driver_data(0x55);

    lcd_driver_cmd(LCD_CMD_MADCTL);
    lcd_driver_data(0x48);
}

/**
 * Set backlight brightness. Values 0-10
 */
void lcd_set_backlight(const uint8_t level)
{
    pwm_set_gpio_level(PIN_LCD_BL, pwm_level_for_brightness(level));
}

/**
 * Hard reset via hardware pin
 */
void lcd_reset()
{
    gpio_put(PIN_LCD_RST, false);
    sleep_us(25);
    gpio_put(PIN_LCD_RST, true);
    sleep_ms(150);
}

/**
 * Turn all pixels on or off
 */
void lcd_set_allpixel(const bool on)
{
    if (on) {
        lcd_driver_cmd(LCD_CMD_ALLPON);
    } else {
        lcd_driver_cmd(LCD_CMD_ALLPOFF);
    }
}

/**
 * Enable/Disable inversion
 */
void lcd_set_invert(const bool on)
{
    if (on) {
        lcd_driver_cmd(LCD_CMD_INVON);
    } else {
        lcd_driver_cmd(LCD_CMD_INVOFF);
    }
}

/**
 * Set horizontal bounds
 */
void lcd_set_column_addr(const uint16_t start, const uint16_t end)
{
    uint8_t start_high = (uint8_t)(start >> 8);
    uint8_t start_low = (uint8_t)(start & 0xff);
    uint8_t end_high = (uint8_t)(end >> 8);
    uint8_t end_low = (uint8_t)(end & 0xff);
    lcd_driver_cmd(LCD_CMD_CASET);
    lcd_driver_data(start_high);
    lcd_driver_data(start_low);
    lcd_driver_data(end_high);
    lcd_driver_data(end_low);
}

/**
 * Set vertical bounds
 */
void lcd_set_page_addr(const uint16_t start, const uint16_t end)
{
    uint8_t start_high = (uint8_t)(start >> 8);
    uint8_t start_low = (uint8_t)(start & 0xff);
    uint8_t end_high = (uint8_t)(end >> 8);
    uint8_t end_low = (uint8_t)(end & 0xff);
    lcd_driver_cmd(LCD_CMD_PASET);
    lcd_driver_data(start_high);
    lcd_driver_data(start_low);
    lcd_driver_data(end_high);
    lcd_driver_data(end_low);
}

/**
 * Set bounds and prepare to send pixel data
 */
void lcd_draw_start(const uint16_t x1, const uint16_t x2, const uint16_t y1, const uint16_t y2)
{
    lcd_set_column_addr(x1, x2);
    lcd_set_page_addr(y1, y2);
    lcd_driver_cmd(LCD_CMD_RAMWR);
}

/**
 * Send pixel data
 */
void lcd_draw_pixel(const uint16_t px)
{
    lcd_driver_pixel(px);
}

/**
 * Draw a whole buffer without toggling pins too often
 */
void lcd_draw_buffer(const uint16_t* buffer, const size_t len)
{
    uint8_t buf8[len * 2];

    for (size_t i = 0; i < len; i++) {
        buf8[i * 2] = (uint8_t)(buffer[i] >> 8);
        buf8[i * 2 + 1] = (uint8_t)(buffer[i] & 0xff);
    }

    gpio_put(PIN_LCD_DC, true);
    gpio_put(PIN_LCD_SPI_CS, false);
    spi_write_blocking(spi1, buf8, len * 2);
    gpio_put(PIN_LCD_SPI_CS, true);
}
