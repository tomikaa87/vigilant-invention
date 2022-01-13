cmake_minimum_required(VERSION 3.20)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(pico-sdk/pico_sdk_init.cmake)

project(pico-oled-widgets)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(pico-oled-widgets
    main.cpp

    Painter.cpp
    Widget.cpp
)

target_link_libraries(pico-oled-widgets
    PRIVATE
        pico_stdlib
        hardware_i2c
        hardware_uart
)

set_target_properties(pico-oled-widgets
    PROPERTIES
        C_STANDARD 17
        CXX_STANDARD 20
)

target_compile_options(pico-oled-widgets
    PRIVATE
        -O3
        -Wall
        $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
)

target_compile_definitions(pico-oled-widgets
    PRIVATE
        CONFIG_DISPLAY_CONTROLLER_SH1106
)

# target_compile_options(pico-oled-widgets PRIVATE -Wall -Wextra -Wno-type-limits)
# target_compile_options(pico-oled-widgets PRIVATE -fdiagnostics-show-option)

pico_enable_stdio_usb(pico-oled-widgets 1)

pico_add_extra_outputs(pico-oled-widgets)