#pragma once
#include "../common/constants.h"
#include "../common/shared_libs.h"

// The main Emulator Class
class CPU {
 private:
  // To store the current opcode, we need a data type that allows 2 bytes
  uint16_t opcode{0};

  // Allocating the general purpose registers
  std::array<uint8_t, NUM_REGISTERS> registers{0};
  // The 16th Register is the VF register that contains the carry flag
  // If a pixel is turned off as a part of drawing, that register is turned off
  // Used for Collision Detection

  // Index Register
  uint16_t index_register{0};
  // Program Counter
  uint16_t program_counter{0};

  // Timer Registers
  uint8_t delay_timer{0};
  uint8_t sound_timer{0};
  // Whenever sound_timer reaches 0, system buzzes

  // Stack
  // CHIP-8 has 16 levels of stack and a stack pointer
  std::array<uint16_t, STACK_SIZE> stack{0};
  uint16_t stack_pointer{0};

 public:
  CPU() = default;
};
