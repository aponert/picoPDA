#ifndef DRIVER_TOUCH_H
#define DRIVER_TOUCH_H

#include "pico/types.h"

#define PIN_TC_SPI_CS 16
#define PIN_TC_IRQ 17

void touch_driver_init();
bool touch_driver_is_touched();

#endif // DRIVER_TOUCH_H
