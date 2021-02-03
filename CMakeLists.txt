cmake_minimum_required(VERSION 3.15)
project(C-Json C)

set(CMAKE_C_STANDARD 99)

add_executable(C-Json c_json.c c_json.h main.c)