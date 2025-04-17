#include "../common.h"
#include "driver_touch.h"

#include "hardware/gpio.h"

/**
 * Initialize CS pin
 */
void touch_driver_init_cs()
{
    gpio_init(PIN_TC_SPI_CS);
    gpio_set_function(PIN_TC_SPI_CS, GPIO_FUNC_SIO);
    gpio_set_dir(PIN_TC_SPI_CS, true);
    gpio_put(PIN_TC_SPI_CS, true);
}

/**
 * Initialize IRQ pin
 */
void touch_driver_init_irq()
{
    gpio_init(PIN_TC_IRQ);
    gpio_set_function(PIN_TC_IRQ, GPIO_FUNC_SIO);
    gpio_set_dir(PIN_TC_IRQ, false);
}

/**
 * Initialize touch driver
 */
void touch_driver_init()
{
    touch_driver_init_cs();
    touch_driver_init_irq();
    common_driver_init_spi();
}

/**
 * Returns, wether the screen is currently touched
 */
bool touch_driver_is_touched()
{
    return !gpio_get(PIN_TC_IRQ);
}
