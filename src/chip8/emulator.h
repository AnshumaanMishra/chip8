#pragma once
#include <SDL3/SDL.h>

#include "../audio/audio.h"
#include "../common/constants.h"
#include "../common/shared_libs.h"
#include "../cpu/cpu.h"
#include "../display/display.h"
#include "../input/input.h"
#include "../memory/memory.h"

class Emulator {
 private:
  Memory memory;
  Display display;
  Keypad keypad;
  Audio audio;

  CPU cpu;

  bool is_running{false};
  bool headless{false};

  void handle_input();

 public:
  Emulator(bool is_headless = false)
      : display(is_headless),
        audio(is_headless),
        cpu(memory, display, keypad, audio),
        headless(is_headless) {}
  void load_rom(const std::string& filepath);
  void run();
};
