#pragma once
#include <SDL3/SDL.h>

#include <memory>

#include "../common/constants.h"
#include "../common/shared_libs.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"

// Smart Pointer Aliases
using SmartWindow = std::unique_ptr<SDL_Window, void (*)(SDL_Window*)>;
using SmartRenderer = std::unique_ptr<SDL_Renderer, void (*)(SDL_Renderer*)>;
using SmartTexture = std::unique_ptr<SDL_Texture, void (*)(SDL_Texture*)>;

class Display {
 private:
  // Display Grid
  // 64 by 32 pixels
  // Each block holds the pixel state 0 or 1
  std::array<uint16_t, DISPLAY_WIDTH * DISPLAY_HEIGHT> framebuffer_array{0};

  // Draw flag
  bool draw_flag{false};

  // SDL Pointers
  SmartWindow window{nullptr, nullptr};
  SmartRenderer renderer{nullptr, nullptr};
  SmartTexture texture{nullptr, nullptr};

 public:
  Display();

  void clear();
  bool draw_sprite(uint8_t x, uint8_t y, uint8_t sprite_byte);
  void render();
  bool is_draw_flag_set() const;
};
