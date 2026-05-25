#pragma once
#include "../common/constants.h"
#include "../common/shared_libs.h"

class Memory {
 private:
  // To enable fixed width data types
  // For Memory Copy

  // To store the current opcode, we need a data type that allows 2 bytes
  uint16_t opcode;

  // Allocation of the 4096 bytes of RAM
  // unsigned 8 bit integer array
  std::array<uint8_t, MEMORY_SIZE> memory;
  // The system Memory Map is as follows:
  // - `0x000` -> `0x1FF` (0 to 511) : CHIP-8 Interpreter
  // - `0x050` -> `0x0A0` (75 to 150) : 4 × 5 pixel font set
  // - `0x200` -> `0xFFF` (512 to 4095) : Program ROM and work RAM

  // Allocating the general purpose registers
  std::array<uint8_t, NUM_REGISTERS> registers;
  // The 16th Register is the VF register that contains the carry flag
  // If a pixel is turned off as a part of drawing, that register is turned off
  // Used for Collision Detection

  // Index Register
  uint16_t index_register;
  // Program Counter
  uint16_t program_counter;

  // Display Grid
  // 64 by 32 pixels
  // Each block holds the pixel state 0 or 1
  std::array<uint16_t, DISPLAY_HEIGHT * DISPLAY_WIDTH> display_grid;

  // Timer Registers
  uint8_t delay_timer;
  uint8_t sound_timer;
  // Whenever sound_timer reaches 0, system buzzes

  // Stack
  // CHIP-8 has 16 levels of stack and a stack pointer
  std::array<uint16_t, STACK_SIZE> stack;
  uint16_t sp;

 public:
  Memory();
  int8_t load_font();
};
