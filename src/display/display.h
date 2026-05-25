#pragma once
#include "../common/constants.h"
#include "../common/shared_libs.h"

class Display {
 private:
  // Display Grid
  // 64 by 32 pixels
  // Each block holds the pixel state 0 or 1
  std::array<uint16_t, DISPLAY_HEIGHT * DISPLAY_WIDTH> display_grid{0};

 public:
  Display() = default;
};
