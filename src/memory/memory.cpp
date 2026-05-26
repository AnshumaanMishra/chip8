#include "memory.h"

#include "common/constants.h"

void Memory::load_font() {
  std::copy(Chip8::FONT.begin(), Chip8::FONT.end(), memory.begin() + Chip8::FONT_START);
}

void Memory::load(std::span<const uint8_t> data) {
  if (Chip8::ROM_START + data.size() > Chip8::MEMORY_SIZE) {
    throw std::runtime_error("Invalid Load: Memory Limit Exceeded");
  }
  std::copy(data.begin(), data.end(), memory.begin() + Chip8::ROM_START);
}

uint8_t Memory::read(uint16_t address) {
  assert(address < Chip8::MEMORY_SIZE and "Invalid Write: Memory Address out of Bounds");
  if (address >= Chip8::MEMORY_SIZE) {
    return 0;
  }

  return memory[address];
}

void Memory::write(uint16_t address, uint8_t value) {
  assert(address < Chip8::MEMORY_SIZE and "Invalid Write: Memory Address out of Bounds");
  if (address >= Chip8::MEMORY_SIZE) {
    return;
  }

#ifndef NDEBUG
  if (address < Chip8::ROM_START) {
    std::cerr << "Warning: Write below " << Chip8::ROM_START << "! " << std::endl;
  }
#endif

  memory[address] = value;
}
