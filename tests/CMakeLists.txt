find_package(GTest REQUIRED)
find_package(SDL3 REQUIRED)

set(CORE_SOURCES
    ../src/audio/audio.cpp
    ../src/cpu/cpu.cpp
    ../src/display/display.cpp
    ../src/input/input.cpp
    ../src/memory/memory.cpp
    ../src/chip8/emulator.cpp
)

add_executable(chip8_tests
    test_memory.cpp
    test_cpu.cpp
    ${CORE_SOURCES}
)

target_link_libraries(chip8_tests PRIVATE GTest::gtest_main SDL3::SDL3)

if (CHIP8_HEADLESS)
  target_compile_definitions(chip8_tests PRIVATE CHIP8_HEADLESS)
endif()

include(GoogleTest)
gtest_discover_tests(chip8_tests)
