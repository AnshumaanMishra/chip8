#include <SDL3/SDL.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "./chip8/emulator.h"

int main(int argc, char* argv[]) {
  // 1. Check for the ROM file argument
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <path_to_rom.ch8>\n";
    return EXIT_FAILURE;
  }

  // 2. Global SDL Initialization (Video + Audio)
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    std::cerr << "Failed to initialize SDL3: " << SDL_GetError() << '\n';
    return EXIT_FAILURE;
  }

  // 3. The Safety Net & Boot Sequence
  try {
    Emulator emulator;

    std::cout << "Loading ROM: " << argv[1] << "...\n";
    emulator.load_rom(argv[1]);

    std::cout << "Booting CHIP-8 Emulator...\n";
    emulator.run();

  } catch (const std::exception& e) {
    std::cerr << "Fatal Emulator Error: " << e.what() << '\n';
  }

  // 4. Safe Teardown
  SDL_Quit();
  return EXIT_SUCCESS;
}
