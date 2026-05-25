add_library(chip8_lib STATIC
  chip8/chip8.cpp
  cpu/cpu.cpp
  memory/memory.cpp
  display/display.cpp
  input/input.cpp
  audio/audio.cpp
)

target_include_directories(chip8_lib PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(chip8_lib PUBLIC SDL3::SDL3)

add_executable(chip8 main.cpp)
target_link_libraries(chip8 PRIVATE chip8_lib)
