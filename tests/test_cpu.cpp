#include <gtest/gtest.h>

#include <memory>

#include "../src/audio/audio.h"
#include "../src/common/constants.h"
#include "../src/cpu/cpu.h"
#include "../src/display/display.h"
#include "../src/input/input.h"
#include "../src/memory/memory.h"

class CPUTest : public ::testing::Test {
 protected:
  Memory memory;
  // Headless injections for CI/CD safety (Bypasses SDL_Init & RAII Crashes)
  Display display{true};
  Keypad keypad;  // No internal SDL bindings, default initialization is safe
  Audio audio{true};

  std::unique_ptr<CPU> cpu;

  void SetUp() override {
    // Wire up dependencies exactly as Emulator orchestrator does
    cpu = std::make_unique<CPU>(memory, display, keypad, audio);
  }
};

TEST_F(CPUTest, BootState) {
  EXPECT_NE(cpu, nullptr);
  EXPECT_EQ(cpu->get_pc(), Chip8::ROM_START);
}

TEST_F(CPUTest, Instruction_1NNN_FlowGoto) {
  // Write 1ABC (Jump to 0xABC)
  memory.write(Chip8::ROM_START, 0x1A);
  memory.write(Chip8::ROM_START + 1, 0xBC);

  cpu->cycle();

  EXPECT_EQ(cpu->get_pc(), 0xABC);
}

TEST_F(CPUTest, Instruction_6XNN_SetRegister) {
  // Write 6342 (Set V3 = 0x42)
  memory.write(Chip8::ROM_START, 0x63);
  memory.write(Chip8::ROM_START + 1, 0x42);

  cpu->cycle();

  EXPECT_EQ(cpu->get_register(3), 0x42);
  EXPECT_EQ(cpu->get_pc(), Chip8::ROM_START + 2);
}

TEST_F(CPUTest, Instruction_7XNN_AddRegister) {
  cpu->set_register(2, 0x10);

  // Write 7205 (Add 0x05 to V2)
  memory.write(Chip8::ROM_START, 0x72);
  memory.write(Chip8::ROM_START + 1, 0x05);

  cpu->cycle();

  EXPECT_EQ(cpu->get_register(2), 0x15);
  EXPECT_EQ(cpu->get_pc(), Chip8::ROM_START + 2);
}

TEST_F(CPUTest, StackPushPopState) {
  cpu->stack_push(0x0FFF);
  cpu->stack_push(0x0AAB);

  EXPECT_EQ(cpu->stack_pop(), 0x0AAB);
  EXPECT_EQ(cpu->stack_pop(), 0x0FFF);
}
