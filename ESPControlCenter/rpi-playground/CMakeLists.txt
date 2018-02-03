cmake_minimum_required(VERSION 3.7)

file(GLOB U8G2_CSOURCES "u8g2/csrc/*.c")
add_library(u8g2 ${U8G2_CSOURCES})
target_include_directories(u8g2 PRIVATE "u8g2/csrc")

file(GLOB WIRINGPI_SOURCES "wiringpi/wiringPi/*.c")
add_library(wiringpi ${WIRINGPI_SOURCES})
target_include_directories(wiringpi PRIVATE "wiringpi/wiringPi")

file(GLOB SOURCES *.cpp)
add_executable(u8g2test ${SOURCES})
target_include_directories(u8g2test PRIVATE "u8g2/csrc" "wiringpi/wiringPi")
target_link_libraries(u8g2test u8g2 wiringpi pthread)

add_definitions("-O3")