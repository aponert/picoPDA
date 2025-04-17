#ifndef DRIVER_LCD_H
#define DRIVER_LCD_H

#include "pico/types.h"

#define PIN_LCD_DC 8
#define PIN_LCD_BL 13
#define PIN_LCD_RST 15

#define PIN_LCD_SPI_CS 9

#define LCD_CMD_NOP 0x00
#define LCD_CMD_SWRESET 0x01
#define LCD_CMD_SLPIN 0x10
#define LCD_CMD_SLPOUT 0x11
#define LCD_CMD_NORON 0x13
#define LCD_CMD_RDDCOLMOD 0x0c
#define LCD_CMD_INVOFF 0x20
#define LCD_CMD_INVON 0x21
#define LCD_CMD_ALLPOFF 0x22
#define LCD_CMD_ALLPON 0x23
#define LCD_CMD_DISOFF 0x28
#define LCD_CMD_DISON 0x29
#define LCD_CMD_CASET 0x2a
#define LCD_CMD_PASET 0x2b
#define LCD_CMD_RAMWR 0x2c
#define LCD_CMD_RAMWRC 0x3c
#define LCD_CMD_MADCTL 0x36
#define LCD_CMD_COLMOD 0x3a

void lcd_driver_init();
void lcd_set_backlight(const uint8_t level);
void lcd_reset();
void lcd_set_allpixel(const bool on);
void lcd_set_invert(const bool on);
void lcd_draw_start(const uint16_t x1, const uint16_t x2, const uint16_t y1, const uint16_t y2);
void lcd_draw_pixel(const uint16_t px);
void lcd_draw_buffer(const uint16_t* buffer, const size_t len);

#endif // DRIVER_LCD_H
