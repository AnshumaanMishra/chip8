#include "cpu.h"

#include <cstdint>
#include <stdexcept>

#include "memory/memory.h"

// Opcode Executor
struct OpcodeExecutor {
  CPU& cpu;

  void operator()(const CallMCR& op) {}
  void operator()(const DisplayClear& op) {}
  void operator()(const FlowReturn& op) {}
  void operator()(const FlowGoto& op) {}
  void operator()(const FlowCall& op) {}
  void operator()(const SkipIfXNN& op) {}
  void operator()(const SkipIfNotXNN& op) {}
  void operator()(const SkipIfXY& op) {}
  void operator()(const SetX& op) {}
  void operator()(const IncrementX& op) {}
  void operator()(const AssignXY& op) {}
  void operator()(const OrEQ& op) {}
  void operator()(const AndEQ& op) {}
  void operator()(const XorEQ& op) {}
  void operator()(const AddEQ& op) {}
  void operator()(const SubEQ& op) {}
  void operator()(const RShift1& op) {}
  void operator()(const SubYX& op) {}
  void operator()(const LShift1& op) {}
  void operator()(const SkipIfNotXY& op) {}
  void operator()(const SetI& op) {}
  void operator()(const FlowJump& op) {}
  void operator()(const RandAnd& op) {}
  void operator()(const Draw& op) {}
  void operator()(const SkipIfKey& op) {}
  void operator()(const SkipIfNotKey& op) {}
  void operator()(const GetDelay& op) {}
  void operator()(const GetKey& op) {}
  void operator()(const SetDelay& op) {}
  void operator()(const SetSound& op) {}
  void operator()(const IncI& op) {}
  void operator()(const SetSpriteLoc& op) {}
  void operator()(const SetBCD& op) {}
  void operator()(const RegDump& op) {}
  void operator()(const RegLoad& op) {}
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
