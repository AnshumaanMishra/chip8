find_package(GTest REQUIRED)

add_executable(chip8_tests
  test_cpu.cpp
  test_memory.cpp
)

target_link_libraries(chip8_tests PRIVATE chip8_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(chip8_tests)
