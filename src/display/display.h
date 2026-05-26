#pragma once
#include <SDL3/SDL.h>

#include "../common/constants.h"
#include "../common/shared_libs.h"
#include "SDL3/SDL_render.h"

struct SDL_WindowDeleter {
  void operator()(SDL_Window* w) const {
    if (w != nullptr)
      SDL_DestroyWindow(w);
  }
};

struct SDL_RendererDeleter {
  void operator()(SDL_Renderer* w) const {
    if (w != nullptr)
      SDL_DestroyRenderer(w);
  }
};

struct SDL_TextureDeleter {
  void operator()(SDL_Texture* w) const {
    if (w != nullptr)
      SDL_DestroyTexture(w);
  }
};

// Smart Pointer Aliases
using SmartWindow = std::unique_ptr<SDL_Window, SDL_WindowDeleter>;
using SmartRenderer = std::unique_ptr<SDL_Renderer, SDL_RendererDeleter>;
using SmartTexture = std::unique_ptr<SDL_Texture, SDL_TextureDeleter>;

class Display {
 private:
  // Display Grid
  // 64 by 32 pixels
  // Each block holds the pixel state 0 or 1
  std::array<uint16_t, Chip8::DISPLAY_WIDTH * Chip8::DISPLAY_HEIGHT> framebuffer_array{0};

  // Draw flag
  bool draw_flag{false};

  // Headless flag
  bool headless{false};
  // SDL Pointers
  SmartWindow window{nullptr};
  SmartRenderer renderer{nullptr};
  SmartTexture texture{nullptr};

 public:
  explicit Display(bool is_headless = false);

  void clear();
  bool draw_sprite(uint8_t x, uint8_t y, uint8_t sprite_byte);
  void render();
  bool is_draw_flag_set() const;
  bool requires_render() const {
    return this->draw_flag;
  }
};
