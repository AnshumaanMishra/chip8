#pragma once
#include "../common/constants.h"
#include "../common/shared_libs.h"

class Keypad {
 private:
  std::array<bool, Chip8::NUM_KEYS> keys{false};

 public:
  Keypad() = default;

  void press_key(uint8_t key);
  void release_key(uint8_t key);
  bool is_key_pressed(uint8_t key) const;
};
