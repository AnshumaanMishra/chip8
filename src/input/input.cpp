#include "input.h"

void Keypad::press_key(uint8_t key) {
  if (key <= Chip8::NUM_KEYS) {
    this->keys[key] = true;
  }
}

void Keypad::release_key(uint8_t key) {
  if (key <= Chip8::NUM_KEYS) {
    this->keys[key] = false;
  }
}

bool Keypad::is_key_pressed(uint8_t key) const {
  if (key <= Chip8::NUM_KEYS) {
    return this->keys[key];
  }
  return false;
}
