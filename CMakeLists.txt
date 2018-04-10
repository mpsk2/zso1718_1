cmake_minimum_required(VERSION 3.10)
project(emu C)

set(CMAKE_C_STANDARD 11)

add_executable(emu main.c alienos.h)