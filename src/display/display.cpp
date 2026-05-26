#include "display.h"

#include <cstdint>

#include "SDL3/SDL_render.h"

Display::Display(bool is_headless) : headless(is_headless) {
  if (!is_headless) {
    SDL_Window* raw_window = nullptr;
    SDL_Renderer* raw_renderer = nullptr;

    if (!SDL_CreateWindowAndRenderer("CHIP-8 Emulator", Chip8::DISPLAY_WIDTH * Chip8::DISPLAY_SCALE,
                                     Chip8::DISPLAY_HEIGHT * Chip8::DISPLAY_SCALE, 0, &raw_window,
                                     &raw_renderer)) {
      throw std::runtime_error("Error: Could not create window and renderer");
    }

    this->window = SmartWindow{raw_window};
    this->renderer = SmartRenderer{raw_renderer};

    SDL_Texture* raw_texture =
        SDL_CreateTexture(this->renderer.get(), SDL_PIXELFORMAT_RGBA8888,
                          SDL_TEXTUREACCESS_STREAMING, Chip8::DISPLAY_WIDTH, Chip8::DISPLAY_HEIGHT);
    SDL_SetTextureScaleMode(raw_texture, SDL_SCALEMODE_NEAREST);
    this->texture = SmartTexture{raw_texture};
  }
}

void Display::clear() {
  std::fill(this->framebuffer_array.begin(), this->framebuffer_array.end(), 0);
  this->draw_flag = true;
}

bool Display::draw_sprite(uint8_t x, uint8_t y, uint8_t sprite_byte) {
  bool collision = false;

  uint8_t start_x = x % Chip8::DISPLAY_WIDTH;
  uint8_t start_y = y % Chip8::DISPLAY_HEIGHT;

  for (uint8_t bit_index = 0; bit_index < 8; bit_index++) {
    uint8_t current_x = start_x + bit_index;

#ifndef LEGACY
    // SUPER-CHIP (Modern): Clip if it hits the right edge while drawing
    if (current_x >= Chip8::DISPLAY_WIDTH) {
      break;
    }
    uint8_t x_warp = current_x;
#else
    // CHIP-8 (Legacy): Wrap around to the left edge while drawing
    uint8_t x_warp = current_x % Chip8::DISPLAY_WIDTH;
#endif

    size_t index = (start_y * Chip8::DISPLAY_WIDTH) + x_warp;
    uint8_t current_bit = (sprite_byte >> (7 - bit_index)) & 1;

    if (current_bit == 1) {
      if (this->framebuffer_array[index] == 1) {
        collision = true;
      }
      this->framebuffer_array[index] ^= 1;
    }
  }

  this->draw_flag = true;
  return collision;
}
void Display::render() {
  if (!headless) {
    std::array<uint32_t, Chip8::DISPLAY_WIDTH * Chip8::DISPLAY_HEIGHT> pixel_buffer;
    for (size_t i = 0; i < Chip8::DISPLAY_WIDTH * Chip8::DISPLAY_HEIGHT; i++) {
      pixel_buffer[i] = (this->framebuffer_array[i] == 1) ? 0xFFFFFFFF : 0x000000FF;
    }

    int pitch = Chip8::DISPLAY_WIDTH * sizeof(uint32_t);
    SDL_UpdateTexture(this->texture.get(), nullptr, pixel_buffer.data(), pitch);
    SDL_RenderClear(this->renderer.get());
    SDL_RenderTexture(this->renderer.get(), this->texture.get(), nullptr, nullptr);
    SDL_RenderPresent(this->renderer.get());
  }
  this->draw_flag = false;
}

bool Display::is_draw_flag_set() const {
  return draw_flag;
}
