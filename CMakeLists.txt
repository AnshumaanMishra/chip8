cmake_minimum_required(VERSION 3.20)
project(chip8 VERSION 0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-fsanitize=address,undefined)
  add_link_options(-fsanitize=address,undefined)
endif()

option(CHIP8_HEADLESS "Build without SDL (for testing)" OFF)

if(NOT CHIP8_HEADLESS)
  find_package(SDL3 REQUIRED)
endif()

add_subdirectory(src)
add_subdirectory(tests)
