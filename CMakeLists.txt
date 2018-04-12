cmake_minimum_required(VERSION 3.5.1)
project(ZSO1718_1)

include_directories(${ZSO1718_1_SOURCE_DIR}/src)
include_directories(${ZSO1718_1_SOURCE_DIR}/include)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
add_executable(emu src/main.c src/display.c src/debug.c include/alienos.h include/debug.h include/display.h)
target_link_libraries(emu ${CURSES_LIBRARIES})