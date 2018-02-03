cmake_minimum_required(VERSION 3.7)

file(GLOB U8G2_CSOURCES "u8g2/csrc/*.c")
add_library(u8g2 ${U8G2_CSOURCES})
target_include_directories(u8g2 PRIVATE "u8g2/csrc")

file(GLOB WIRINGPI_SOURCES "wiringpi/wiringPi/*.c")
add_library(wiringpi ${WIRINGPI_SOURCES})
target_include_directories(wiringpi PRIVATE "wiringpi/wiringPi")

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(playground ${SOURCES})
target_include_directories(playground PRIVATE "u8g2/csrc" "wiringpi/wiringPi")
target_link_libraries(playground u8g2 wiringpi pthread)

add_definitions("-O3")