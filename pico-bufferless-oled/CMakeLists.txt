cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(pico-sdk/pico_sdk_init.cmake)

project(pico_bufferless_oled)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(pico_bufferless_oled
    main.c
    ssd1306.c
    widget.c
    widget_font.c
    widget_text.c
)

target_link_libraries(pico_bufferless_oled PRIVATE
    pico_stdlib
    hardware_i2c

    hardware_adc
    hardware_base
    hardware_claim
    hardware_clocks
    hardware_divider
    hardware_dma
    hardware_flash
    hardware_gpio
    hardware_interp
    hardware_irq
    hardware_pio
    hardware_pll
    hardware_pwm
    hardware_resets
    hardware_rtc
    hardware_spi
)

# target_compile_options(pico_bufferless_oled PRIVATE -std=c11)
target_compile_options(pico_bufferless_oled PRIVATE -Wall -Wextra -Wno-type-limits)
# target_compile_options(pico_bufferless_oled PRIVATE -fdiagnostics-show-option)

pico_enable_stdio_usb(pico_bufferless_oled 1)

pico_add_extra_outputs(pico_bufferless_oled)