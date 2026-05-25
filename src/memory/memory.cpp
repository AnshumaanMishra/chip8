#include "memory.h"

#include "common/constants.h"

void Memory::load_font() {
  std::copy(FONT.begin(), FONT.end(), memory.begin() + FONT_START);
}

void Memory::load(std::span<const uint8_t> data) {
  if (ROM_START + data.size() > MEMORY_SIZE) {
    throw std::runtime_error("Invalid Load: Memory Limit Exceeded");
  }
  std::copy(data.begin(), data.end(), memory.begin() + ROM_START);
}

uint8_t Memory::read(uint16_t address) {
  assert(address < MEMORY_SIZE and "Invalid Write: Memory Address out of Bounds");
  if (address >= MEMORY_SIZE) {
    return 0;
  }

  return memory[address];
}

void Memory::write(uint16_t address, uint8_t value) {
  assert(address < MEMORY_SIZE and "Invalid Write: Memory Address out of Bounds");
  if (address >= MEMORY_SIZE) {
    return;
  }

#ifndef NDEBUG
  if (address < ROM_START) {
    std::cerr << "Warning: Write below " << ROM_START << "! " << std::endl;
  }
#endif

  memory[address] = value;
}
