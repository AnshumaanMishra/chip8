#include "display.h"

#include <cstdint>

#include "SDL3/SDL_render.h"
#include "common/constants.h"

Display::Display() {
  SDL_Window* raw_window = nullptr;
  SDL_Renderer* raw_renderer = nullptr;

  if (!SDL_CreateWindowAndRenderer("CHIP-8 Emulator", DISPLAY_WIDTH * 10, DISPLAY_HEIGHT * 10, 0,
                                   &raw_window, &raw_renderer)) {
    throw std::runtime_error("Error: Could not create window and renderer");
  }

  this->window = SmartWindow{raw_window, SDL_DestroyWindow};
  this->renderer = SmartRenderer{raw_renderer, SDL_DestroyRenderer};

  SDL_Texture* raw_texture =
      SDL_CreateTexture(this->renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
                        DISPLAY_WIDTH, DISPLAY_HEIGHT);
  SDL_SetTextureScaleMode(raw_texture, SDL_SCALEMODE_NEAREST);
  this->texture = SmartTexture{raw_texture, SDL_DestroyTexture};
}

void Display::clear() {
  std::fill(this->framebuffer_array.begin(), this->framebuffer_array.end(), 0);
  this->draw_flag = true;
}

bool Display::draw_sprite(uint8_t x, uint8_t y, uint8_t sprite_byte) {
  bool collision = false;
  uint8_t y_warp = y % DISPLAY_HEIGHT;
  // could be replaced with y & (DISPLAY_HEIGHT - 1) since its a power of two
  for (uint8_t bit_index = 0; bit_index < 8; bit_index++) {
    uint8_t x_warp = (x + bit_index) % DISPLAY_WIDTH;
    size_t index = (y_warp * DISPLAY_WIDTH) + x_warp;
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
  std::array<uint32_t, DISPLAY_WIDTH * DISPLAY_HEIGHT> pixel_buffer;
  for (size_t i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
    pixel_buffer[i] = (this->framebuffer_array[i] == 1) ? 0xFFFFFFFF : 0x000000FF;
  }

  int pitch = DISPLAY_WIDTH * sizeof(uint32_t);
  SDL_UpdateTexture(this->texture.get(), nullptr, pixel_buffer.data(), pitch);
  SDL_RenderClear(this->renderer.get());
  SDL_RenderTexture(this->renderer.get(), this->texture.get(), nullptr, nullptr);
  SDL_RenderPresent(this->renderer.get());

  this->draw_flag = false;
}

bool Display::is_draw_flag_set() const {
  return draw_flag;
}
