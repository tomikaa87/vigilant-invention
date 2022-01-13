cmake_minimum_required(VERSION 3.20)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(pico-sdk/pico_sdk_init.cmake)

project(pico-snowfall)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(pico-snowfall
    main.cpp
    Driver_SH1106.cpp
    Driver_SSD1306.cpp
)

target_link_libraries(pico-snowfall
    PRIVATE
        pico_stdlib
        hardware_i2c
)

set_target_properties(pico-snowfall
    PROPERTIES
        C_STANDARD 17
        CXX_STANDARD 17
)

target_compile_options(pico-snowfall
    PRIVATE
        -O3
        -Wall
        $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
)

# target_compile_options(pico-snowfall PRIVATE -Wall -Wextra -Wno-type-limits)
# target_compile_options(pico-snowfall PRIVATE -fdiagnostics-show-option)

pico_enable_stdio_usb(pico-snowfall 1)

pico_add_extra_outputs(pico-snowfall)