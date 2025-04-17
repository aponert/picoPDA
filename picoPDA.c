#include <stdio.h>
#include "pico/stdlib.h"

#include "driver_lcd.h"
#include "driver_touch.h"

#include "lvgl.h"

#include "lvgl/examples/lv_examples.h"

#define FB_PARTIAL_BYTES 32768

lv_obj_t *label_touch;

uint8_t fb_partial1[FB_PARTIAL_BYTES];
uint8_t fb_partial2[FB_PARTIAL_BYTES];

uint32_t get_current_ms(void)
{
    return (uint32_t)(get_absolute_time()/1000);
}

void lvgl_flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
{
    uint16_t * buf16 = (uint16_t *)px_map;
    lcd_draw_start((uint16_t)(area->x1), (uint16_t)(area->x2), (uint16_t)(area->y1), (uint16_t)(area->y2));
    int32_t pcount = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1);
    lcd_draw_buffer(buf16, pcount);

    lv_display_flush_ready(display);
}

void lvgl_init()
{
    lv_init();
    lv_tick_set_cb(get_current_ms);
    lv_display_t * display = lv_display_create(320, 480);
    lv_display_set_buffers(display, fb_partial1, fb_partial2, FB_PARTIAL_BYTES, LV_DISP_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display, lvgl_flush_cb);
}

static void anim_x_cb(void * var, int32_t v)
{
    lv_obj_set_x(var, v);
}

static void anim_size_cb(void * var, int32_t v)
{
    lv_obj_set_size(var, v, v);
}

void lvgl_draw_ui()
{
    lv_obj_t * win = lv_win_create(lv_screen_active());
    lv_obj_t * btn;

    lv_win_add_title(win, " Touch Driver Test");

    btn = lv_win_add_button(win, LV_SYMBOL_CLOSE, 60);

    lv_obj_t * cont = lv_win_get_content(win);  /*Content can be added here*/
    label_touch = lv_label_create(cont);

    lv_label_set_text(label_touch, "Touched? No");
}

int main()
{
    stdio_init_all();
    touch_driver_init();
    lcd_driver_init();
    lvgl_init();
    lvgl_draw_ui();
    
    while (true) {
        lv_timer_periodic_handler();
        if (touch_driver_is_touched()) {
            lv_label_set_text(label_touch, "Touched? Yes");
        } else {
            lv_label_set_text(label_touch, "Touched? No");
        }
    }
}
