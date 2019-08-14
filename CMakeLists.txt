cmake_minimum_required (VERSION 3.14)
set(CMAKE_CXX_STANDARD 20)

project ("big_numbers" VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_VERSION 3.14)
set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES main.cpp)

add_executable (big_numbers "main.cpp")