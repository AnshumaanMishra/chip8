#include "cpu.h"

#include <cstdint>

#include "common/constants.h"

// Opcode Executor
struct OpcodeExecutor {
  CPU& cpu;

  void operator()(const CallMCR&) {
    // Pass, ignored in emulation
  }
  void operator()(const DisplayClear& op) {
    // TODO:
  }
  void operator()(const FlowReturn&) {
    cpu.program_counter = cpu.stack_pop();
  }
  void operator()(const FlowGoto& op) {
#ifndef NDEBUG
    if (op.address < ROM_START) {
      std::cerr << "Warning: FlowGoto attempting to jump to interpreter memory space at 0x"
                << std::hex << op.address << std::endl;
    }
#endif
    cpu.program_counter = op.address;
  }
  void operator()(const FlowCall& op) {
    cpu.stack_push(cpu.program_counter);
    cpu.program_counter = op.address;
  }
  void operator()(const SkipIfXNN& op) {
    if (cpu.registers[op.X] == op.NN) {
      cpu.program_counter += 2;
    }
  }
  void operator()(const SkipIfNotXNN& op) {
    if (cpu.registers[op.X] != op.NN) {
      cpu.program_counter += 2;
    }
  }
  void operator()(const SkipIfXY& op) {
    if (cpu.registers[op.X] == cpu.registers[op.Y]) {
      cpu.program_counter += 2;
    }
  }
  void operator()(const SetX& op) {
    cpu.registers[op.X] = op.NN;
  }
  void operator()(const IncrementX& op) {
    cpu.registers[op.X] += op.NN;
  }
  void operator()(const AssignXY& op) {
    cpu.registers[op.X] = cpu.registers[op.Y];
  }
  void operator()(const OrEQ& op) {
    cpu.registers[op.X] |= cpu.registers[op.Y];
  }
  void operator()(const AndEQ& op) {
    cpu.registers[op.X] &= cpu.registers[op.Y];
  }
  void operator()(const XorEQ& op) {
    cpu.registers[op.X] ^= cpu.registers[op.Y];
  }
  void operator()(const AddEQ& op) {
    uint8_t carry = (0xFF - cpu.registers[op.X] < cpu.registers[op.Y]) ? 1 : 0;
    cpu.registers[op.X] += cpu.registers[op.Y];
    cpu.registers[0xF] = carry;
  }
  void operator()(const SubEQ& op) {
    uint8_t borrow = (cpu.registers[op.X] >= cpu.registers[op.Y]) ? 1 : 0;
    cpu.registers[op.X] -= cpu.registers[op.Y];
    cpu.registers[0xF] = borrow;
  }
  void operator()(const RShift1& op) {
#ifdef LEGACY
    uint8_t shifted_out_bit = cpu.registers[op.Y] & 0x1;
    cpu.registers[op.X] = cpu.registers[op.Y] >> 1;
    cpu.registers[0xF] = shifted_out_bit;
#else
    uint8_t shifted_out_bit = cpu.registers[op.X] & 0x1;
    cpu.registers[op.X] = cpu.registers[op.X] >> 1;
    cpu.registers[0xF] = shifted_out_bit;
#endif
  }
  void operator()(const SubYX& op) {
    uint8_t borrow = (cpu.registers[op.X] <= cpu.registers[op.Y]) ? 1 : 0;
    cpu.registers[op.X] = cpu.registers[op.Y] - cpu.registers[op.X];
    cpu.registers[0xF] = borrow;
  }
  void operator()(const LShift1& op) {
#ifdef LEGACY
    uint8_t shifted_out_bit = (cpu.registers[op.Y] & 0b10000000) >> 7;
    cpu.registers[op.X] = cpu.registers[op.Y] << 1;
    cpu.registers[0xF] = shifted_out_bit;
#else
    uint8_t shifted_out_bit = (cpu.registers[op.X] & 0b10000000) >> 7;
    cpu.registers[op.X] = cpu.registers[op.X] << 1;
    cpu.registers[0xF] = shifted_out_bit;
#endif
  }
  void operator()(const SkipIfNotXY& op) {
    if (cpu.registers[op.X] != cpu.registers[op.Y]) {
      cpu.program_counter += 2;
    }
  }
  void operator()(const SetI& op) {
    cpu.index_register = op.value;
  }
  void operator()(const FlowJump& op) {
    cpu.program_counter = op.value + cpu.registers[0];
  }
  void operator()(const RandAnd& op) {
    uint8_t random_byte = cpu.dist(cpu.rng);
    cpu.registers[op.X] = random_byte & op.NN;
  }
  void operator()(const Draw& op) {
    uint8_t x_start = cpu.registers[op.X] % DISPLAY_WIDTH;
    uint8_t y_start = cpu.registers[op.Y] % DISPLAY_HEIGHT;

    // Collision Flag set to false
    cpu.registers[0xF] = 0;

    for (uint8_t row = 0; row < op.N; row++) {
      uint8_t sprite_byte = cpu.memory.read(cpu.index_register + row);

      for (uint8_t col = 0; col < 8; col++) {
        uint8_t sprite_pixel = (sprite_byte & (0x80 >> col));

        if (sprite_pixel) {
          if (x_start + col >= DISPLAY_WIDTH || y_start + row >= DISPLAY_HEIGHT) {
            continue;
          }

          uint16_t screen_idx = (x_start + col) + (DISPLAY_WIDTH * (y_start + row));

          if (cpu.graphics[screen_idx]) {
            cpu.registers[0xF] = 1;
          }

          cpu.graphics[screen_idx] ^= 1;
        }
      }
    }
  }
  void operator()(const SkipIfKey& op) {
    uint8_t key = cpu.registers[op.X];
    if (cpu.keypad[key]) {
      cpu.program_counter += 2;
    }
  }
  void operator()(const SkipIfNotKey& op) {
    uint8_t key = cpu.registers[op.X];
    if (!cpu.keypad[key]) {
      cpu.program_counter += 2;
    }
  }
  void operator()(const GetDelay& op) {
    cpu.registers[op.X] = cpu.delay_timer;
  }
  void operator()(const GetKey& op) {
    bool key_pressed = false;

    for (uint8_t i = 0; i < 16; ++i) {
      if (cpu.keypad[i]) {
        cpu.registers[op.X] = i;
        key_pressed = true;
        break;
      }
    }

    if (!key_pressed) {
      cpu.program_counter -= 2;
    }
  }
  void operator()(const SetDelay& op) {
    cpu.delay_timer = cpu.registers[op.X];
  }
  void operator()(const SetSound& op) {
    cpu.sound_timer = cpu.registers[op.X];
  }
  void operator()(const IncI& op) {
    cpu.index_register += cpu.registers[op.X];
  }
  void operator()(const SetSpriteLoc& op) {
    cpu.index_register = FONT_START + (cpu.registers[op.X] * 5);
  }
  void operator()(const SetBCD& op) {
    uint8_t val = cpu.registers[op.X];
    uint8_t hundreds = (val / 100) % 10;
    uint8_t tens = (val / 10) % 10;
    uint8_t ones = val % 10;

    cpu.memory.write(cpu.index_register, hundreds);
    cpu.memory.write(cpu.index_register + 1, tens);
    cpu.memory.write(cpu.index_register + 2, ones);
  }
  void operator()(const RegDump& op) {
    for (uint8_t i = 0; i <= op.X; ++i) {
      cpu.memory.write(cpu.index_register + i, cpu.registers[i]);
    }

#ifdef LEGACY
    cpu.index_register += op.X + 1;
#endif
  }
  void operator()(const RegLoad& op) {
    for (uint8_t i = 0; i <= op.X; ++i) {
      cpu.registers[i] = cpu.memory.read(cpu.index_register + i);
    }

#ifdef LEGACY
    cpu.index_register += op.X + 1;
#endif
  }
};

// Stack Operations

void CPU::stack_push(uint16_t value) {
  if (stack_pointer == STACK_SIZE) {
    throw std::runtime_error("Error: Stack Size Full");
  }

  stack[stack_pointer] = value;
  stack_pointer++;
}

uint16_t CPU::stack_pop() {
  if (stack_pointer == 0) {
    throw std::runtime_error("Error: Empty Stack");
  }
  stack_pointer--;
  return stack[stack_pointer];
}

// CPU Operations

// Decode Table
std::array<std::function<Opcode(uint16_t)>, 16> CPU::decode_table = [] {
  std::array<std::function<Opcode(uint16_t)>, 16> table;

  table[0x0] = [](uint16_t remaining_nibbles) -> Opcode {
    switch (remaining_nibbles) {
      case 0x0E0:
        return DisplayClear{};
      case 0x0EE:
        return FlowReturn{};
      default:
        return CallMCR{remaining_nibbles};
    }
  };
  table[0x1] = [](uint16_t remaining_nibbles) -> Opcode { return FlowGoto{remaining_nibbles}; };
  table[0x2] = [](uint16_t remaining_nibbles) -> Opcode { return FlowCall{remaining_nibbles}; };
  table[0x3] = [](uint16_t remaining_nibbles) -> Opcode {
    uint8_t x = (remaining_nibbles & 0xF00) >> 8;
    uint8_t nn = remaining_nibbles & 0xFF;
    return SkipIfXNN{x, nn};
  };
  table[0x4] = [](uint16_t remaining_nibbles) -> Opcode {
    uint8_t x = (remaining_nibbles & 0xF00) >> 8;
    uint8_t nn = remaining_nibbles & 0xFF;
    return SkipIfNotXNN{x, nn};
  };
  table[0x5] = [](uint16_t remaining_nibbles) -> Opcode {
    uint8_t x = (remaining_nibbles & 0xF00) >> 8;
    uint8_t y = (remaining_nibbles & 0xF0) >> 4;
    return SkipIfXY{x, y};
  };
  table[0x6] = [](uint16_t remaining_nibbles) -> Opcode {
    uint8_t x = (remaining_nibbles & 0xF00) >> 8;
    uint8_t nn = remaining_nibbles & 0xFF;
    return SetX{x, nn};
  };
  table[0x7] = [](uint16_t remaining_nibbles) -> Opcode {
    uint8_t x = (remaining_nibbles & 0xF00) >> 8;
    uint8_t nn = remaining_nibbles & 0xFF;
    return IncrementX{x, nn};
  };
  table[0x8] = [](uint16_t remaining_nibbles) -> Opcode {
    uint8_t x = (remaining_nibbles & 0xF00) >> 8;
    uint8_t y = (remaining_nibbles & 0xF0) >> 4;
    uint8_t last_nibble = remaining_nibbles & 0xF;
    switch (last_nibble) {
      case 0x0:
        return AssignXY{x, y};
      case 0x1:
        return OrEQ{x, y};
      case 0x2:
        return AndEQ{x, y};
      case 0x3:
        return XorEQ{x, y};
      case 0x4:
        return AddEQ{x, y};
      case 0x5:
        return SubEQ{x, y};
      case 0x6:
        return RShift1{x, y};
      case 0x7:
        return SubYX{x, y};
      case 0xE:
        return LShift1{x, y};
      default:
        throw std::runtime_error("Invalid OpCode!");
    }
  };
  table[0x9] = [](uint16_t remaining_nibbles) -> Opcode {
    uint8_t x = (remaining_nibbles & 0xF00) >> 8;
    uint8_t y = (remaining_nibbles & 0xF0) >> 4;
    return SkipIfNotXY{x, y};
  };
  table[0xA] = [](uint16_t remaining_nibbles) -> Opcode { return SetI{remaining_nibbles}; };
  table[0xB] = [](uint16_t remaining_nibbles) -> Opcode { return FlowJump{remaining_nibbles}; };
  table[0xC] = [](uint16_t remaining_nibbles) -> Opcode {
    uint8_t x = (remaining_nibbles & 0xF00) >> 8;
    uint8_t nn = remaining_nibbles & 0xFF;
    return RandAnd{x, nn};
  };
  table[0xD] = [](uint16_t remaining_nibbles) -> Opcode {
    uint8_t x = (remaining_nibbles & 0xF00) >> 8;
    uint8_t y = (remaining_nibbles & 0xF0) >> 4;
    uint8_t n = (remaining_nibbles & 0xF);
    return Draw{x, y, n};
  };
  table[0xE] = [](uint16_t remaining_nibbles) -> Opcode {
    uint8_t x = (remaining_nibbles & 0xF00) >> 8;
    uint8_t last_byte = remaining_nibbles & 0xFF;
    switch (last_byte) {
      case 0x9E:
        return SkipIfKey{x};
      case 0xA1:
        return SkipIfNotKey{x};
      default:
        throw std::runtime_error("Invlid Opcode");
    }
  };
  table[0xF] = [](uint16_t remaining_nibbles) -> Opcode {
    uint8_t x = (remaining_nibbles & 0xF00) >> 8;
    uint8_t last_byte = remaining_nibbles & 0xFF;
    switch (last_byte) {
      case 0x07:
        return GetDelay{x};
      case 0x0A:
        return GetKey{x};
      case 0x15:
        return SetDelay{x};
      case 0x18:
        return SetSound{x};
      case 0x1E:
        return IncI{x};
      case 0x29:
        return SetSpriteLoc{x};
      case 0x33:
        return SetBCD{x};
      case 0x55:
        return RegDump{x};
      case 0x65:
        return RegLoad{x};
      default:
        throw std::runtime_error("Invlid Opcode");
    }
  };

  return table;
}();

uint16_t CPU::fetch() {
  uint16_t opcode_1 = memory.read(program_counter);
  // Invalid Read guardrail already implemented in read function
  uint16_t opcode_2 = memory.read(program_counter + 1);

  program_counter += 2;

  return opcode_1 << 8 | opcode_2;
}

Opcode CPU::decode(uint16_t extracted_opcode) {
  uint8_t top_nibble = (extracted_opcode & 0xF000) >> 12;
  uint16_t remaining_nibbles = (extracted_opcode & 0xFFF);
  return decode_table[top_nibble](remaining_nibbles);
}

void CPU::cycle() {
  uint16_t extracted_opcode = fetch();
  Opcode decoded_op = decode(extracted_opcode);

  OpcodeExecutor visitor{*this};
  std::visit(visitor, decoded_op);
}
