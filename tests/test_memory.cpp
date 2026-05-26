#include <gtest/gtest.h>

#include <span>
#include <vector>

#include "../src/common/constants.h"
#include "../src/memory/memory.h"

class MemoryTest : public ::testing::Test {
 protected:
  Memory memory;
};

TEST_F(MemoryTest, BoundarySafety) {
  // Assert safe fallback (returning 0) for out-of-bounds reads
  EXPECT_EQ(memory.read(0xFFFF), 0);

  // Assert out-of-bounds write does not crash or corrupt state
  memory.write(0xFFFF, 0xAA);
  EXPECT_EQ(memory.read(0xFFFF), 0);
}

TEST_F(MemoryTest, ReadWriteConsistency) {
  memory.write(0x500, 0x42);
  EXPECT_EQ(memory.read(0x500), 0x42);
}

TEST_F(MemoryTest, FontLoading) {
  memory.load_font();
  // Check if the first byte of the standard font loaded to the correct offset
  EXPECT_EQ(memory.read(Chip8::FONT_START), Chip8::FONT[0]);
}

TEST_F(MemoryTest, RomLoading) {
  std::vector<uint8_t> dummy_rom = {0x00, 0xE0, 0xA2, 0x2A};
  memory.load(std::span<const uint8_t>(dummy_rom));

  EXPECT_EQ(memory.read(Chip8::ROM_START), 0x00);
  EXPECT_EQ(memory.read(Chip8::ROM_START + 1), 0xE0);
  EXPECT_EQ(memory.read(Chip8::ROM_START + 2), 0xA2);
  EXPECT_EQ(memory.read(Chip8::ROM_START + 3), 0x2A);
}
