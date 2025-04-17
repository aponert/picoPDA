cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_library(driver_prt35 STATIC
        common.c
        lcd/driver_lcd.c
        touch/driver_touch.c)
target_include_directories(driver_prt35 INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        lcd/
        touch/
)

target_link_libraries(driver_prt35
        pico_time
        pico_stdlib
        hardware_pwm
        hardware_gpio
        hardware_spi)
