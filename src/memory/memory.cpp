#include "memory.h"

int8_t Memory::load_font() {
  for (size_t i = FONT_START; i <= FONT_END; i++) {
    memory[i] = FONT[i - FONT_START];
  }
  return 0;
}
