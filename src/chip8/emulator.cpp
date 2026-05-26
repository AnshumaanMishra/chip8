#include "emulator.h"

void Emulator::load_rom(const std::string& filepath) {
  std::ifstream file(filepath, std::ios::binary | std::ios::ate);

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open ROM file: " + filepath);
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  if (static_cast<size_t>(size) > (Chip8::MEMORY_SIZE - Chip8::ROM_START)) {
    throw std::runtime_error("ROM file is too large to fit in memory!");
  }

  std::vector<char> buffer(size);
  if (file.read(buffer.data(), size)) {
    for (std::streamsize i = 0; i < size; ++i) {
      this->memory.write(Chip8::ROM_START + i, static_cast<uint8_t>(buffer[i]));
    }
  } else {
    throw std::runtime_error("Failed to read ROM file data.");
  }
}

void Emulator::handle_input() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      this->is_running = false;
    } else if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
      bool is_pressed = (event.type == SDL_EVENT_KEY_DOWN);

      uint8_t key = 0xFF;
      switch (event.key.key) {
        case SDLK_1:
          key = 0x1;
          break;
        case SDLK_2:
          key = 0x2;
          break;
        case SDLK_3:
          key = 0x3;
          break;
        case SDLK_4:
          key = 0xC;
          break;
        case SDLK_Q:
          key = 0x4;
          break;
        case SDLK_W:
          key = 0x5;
          break;
        case SDLK_E:
          key = 0x6;
          break;
        case SDLK_R:
          key = 0xD;
          break;
        case SDLK_A:
          key = 0x7;
          break;
        case SDLK_S:
          key = 0x8;
          break;
        case SDLK_D:
          key = 0x9;
          break;
        case SDLK_F:
          key = 0xE;
          break;
        case SDLK_Z:
          key = 0xA;
          break;
        case SDLK_X:
          key = 0x0;
          break;
        case SDLK_C:
          key = 0xB;
          break;
        case SDLK_V:
          key = 0xF;
          break;
      }

      if (key != 0xFF) {
        if (is_pressed)
          this->keypad.press_key(key);
        else
          this->keypad.release_key(key);
      }
    }
  }
}

void Emulator::run() {
  this->is_running = true;

  constexpr double cpu_tick_rate = 1000.0 / Chip8::CPU_HZ;
  constexpr double timer_tick_rate = 1000.0 / Chip8::TIMER_HZ;

  auto last_time = std::chrono::high_resolution_clock::now();
  double cpu_accumulator = 0.0;
  double timer_accumulator = 0.0;

  while (this->is_running) {
    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = current_time - last_time;
    last_time = current_time;

    cpu_accumulator += elapsed.count();
    timer_accumulator += elapsed.count();

    handle_input();

    while (cpu_accumulator >= cpu_tick_rate) {
      this->cpu.cycle();
      cpu_accumulator -= cpu_tick_rate;
    }

    while (timer_accumulator >= timer_tick_rate) {
      this->cpu.update_timers(this->audio);

      if (this->display.requires_render()) {
        this->display.render();
      }

      timer_accumulator -= timer_tick_rate;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}
