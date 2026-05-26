#pragma once
#include "../common/constants.h"
#include "../common/opcodes.h"
#include "../common/shared_libs.h"

class Memory;
class Display;
class Keypad;
class Audio;

// The main Emulator Class
class CPU {
 private:
  // To store the current opcode, we need a data type that allows 2 bytes
  // The opcode will be returned from the functions

  // Allocating the general purpose registers
  std::array<uint8_t, Chip8::NUM_REGISTERS> registers{0};
  // The 16th Register is the VF register that contains the carry flag
  // If a pixel is turned off as a part of drawing, that register is turned off
  // Used for Collision Detection

  // Index Register
  uint16_t index_register{0};
  // Program Counter
  uint16_t program_counter{Chip8::ROM_START};

  // Timer Registers
  uint8_t delay_timer{0};
  uint8_t sound_timer{0};
  // Whenever sound_timer reaches 0, system buzzes

  // Stack
  // CHIP-8 has 16 levels of stack and a stack pointer
  std::array<uint16_t, Chip8::STACK_SIZE> stack{0};
  uint8_t stack_pointer{0};

  // Opcode Dispatch Table
  // Essentially an array of functions acting as a map
  static std::array<std::function<Opcode(uint16_t)>, 16> decode_table;

  Memory& memory;
  Display& display;
  Keypad& keypad;
  Audio& audio;

  friend struct OpcodeExecutor;
  std::mt19937 rng{std::random_device{}()};
  std::uniform_int_distribution<uint16_t> dist{0, 255};

 public:
  uint8_t get_register(size_t index) const {
    return registers[index];
  }
  void set_register(size_t index, uint8_t val) {
    registers[index] = val;
  }

  uint16_t get_pc() const {
    return program_counter;
  }
  void set_pc(uint16_t val) {
    program_counter = val;
  }
  explicit CPU(Memory& mem_ref, Display& disp_ref, Keypad& key_ref, Audio& aud_ref)
      : memory(mem_ref), display(disp_ref), keypad(key_ref), audio(aud_ref) {}

  void stack_push(uint16_t value);
  uint16_t stack_pop();

  uint16_t fetch();
  Opcode decode(uint16_t extracted_opcode);
  void execute();

  void cycle();

  void update_timers(Audio& audio);
};
