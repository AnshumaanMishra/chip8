#pragma once
#include "../common/constants.h"
#include "../common/shared_libs.h"

class Memory {
 private:
  // Allocation of the 4096 bytes of RAM
  // unsigned 8 bit integer array
  std::array<uint8_t, Chip8::MEMORY_SIZE> memory{0};
  // The system Memory Map is as follows:
  // - `0x000` -> `0x1FF` (0 to 511) : CHIP-8 Interpreter
  // - `0x050` -> `0x0A0` (75 to 150) : 4 × 5 pixel font set
  // - `0x200` -> `0xFFF` (512 to 4095) : Program ROM and work RAM

 public:
  Memory() = default;

  void load_font();
  void load(std::span<const uint8_t> data);
  uint8_t read(uint16_t address);
  void write(uint16_t address, uint8_t value);
};
