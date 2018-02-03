cmake_minimum_required(VERSION 3.7)

add_definitions("-O3")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++17")

# u8g2 library
file(GLOB U8G2_CSOURCES "u8g2/csrc/*.c")
add_library(u8g2 ${U8G2_CSOURCES})
target_include_directories(u8g2 PRIVATE "u8g2/csrc")

# WiringPi library
file(GLOB WIRINGPI_SOURCES "wiringpi/wiringPi/*.c")
add_library(wiringpi ${WIRINGPI_SOURCES})
target_include_directories(wiringpi PRIVATE "wiringpi/wiringPi")

# Main binary
file(GLOB_RECURSE SOURCES "src/*.cpp")
#set(SOURCES ${SOURCES} "../ui/pxplus-vga8-32.c")
add_executable(playground ${SOURCES})
target_include_directories(playground PRIVATE "u8g2/csrc" "wiringpi/wiringPi")
target_link_libraries(playground u8g2 wiringpi pthread)
