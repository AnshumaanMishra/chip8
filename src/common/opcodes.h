#include "shared_libs.h"

// 01. 0x0NNN: Calls Machine Code Routine at address 0x0NNN
struct CallMCR {
  uint16_t address;
};

// 02. 0x00E0: Display Clear
struct DisplayClear {};

// 03. 0x00EE: Flow Return from a subroutine
struct FlowReturn {};

// 04. 0x1NNN: Flow Goto address 0x0NNN
struct FlowGoto {
  uint16_t address;
};

// 05. 0x2NNN: Calls Subroutine at address 0xNNN
struct FlowCall {
  uint16_t address;
};

// 06. 0x3XNN: Skips the next instruction if Register at 0xX has value 0xNN
// Usually succeeded with some Goto statement
struct SkipIfXNN {
  uint8_t X;
  uint8_t NN;
};

// 07. 0x4NN: Skips next instruction if value register 0xX is NOT equal to 0xNN
struct SkipIfNotXNN {
  uint8_t X;
  uint8_t NN;
};

// 08. 0x5XY0: Skips the next instruction if Register at 0xX has same value as Register at 0xY
struct SkipIfXY {
  uint8_t X;
  uint8_t Y;
};

// 09. 0x6XNN: Set VX = 0xNN
struct SetX {
  uint8_t X;
  uint8_t NN;
};

// 10. 0x7XNN: Increment VX by 0xNN
struct IncrementX {
  uint8_t X;
  uint8_t NN;
};

// 11. 0x8XY0: Sets VX = VY
struct AssignXY {
  uint8_t X;
  uint8_t Y;
};

// 12. 0x8XY1: OrEq, VX = VX | VY
struct OrEQ {
  uint8_t X;
  uint8_t Y;
};

// 13. 0x8XY2: AndEQ, VX = VX & VY
struct AndEQ {
  uint8_t X;
  uint8_t Y;
};

// 14. 0x8XY3: AndEQ, VX = VX ^ VY
struct XorEQ {
  uint8_t X;
  uint8_t Y;
};

// 15. 0x8XY4: AddEQ, VX = VX + VY
struct AddEQ {
  uint8_t X;
  uint8_t Y;
};

// 16. 0x8XY5: SubEQ VX -= VY
struct SubEQ {
  uint8_t X;
  uint8_t Y;
};

// 17. 0x8XY6: RShift1 VX >>= 1
// If the LEGACY Macro is defined, it functions as VX = VY >>= 1
struct RShift1 {
  uint8_t X;
  uint8_t Y;
};

// 18. 0x8XY7: VX = VY - VX
struct SubYX {
  uint8_t X;
  uint8_t Y;
};

// 19. 0x8XYE: LShift1 VX <<= 1
// If LEGACY macro is defined, it works as VX = VY <<= 1
struct LShift1 {
  uint8_t X;
  uint8_t Y;
};

// 20. 0x9XY0: Skips the next instruction if Register at 0xX does not have same value as Register at
// 0xY
struct SkipIfNotXY {
  uint8_t X;
  uint8_t Y;
};

// 21. 0xANNN: Sets Index Register to 0x0NNN
struct SetI {
  uint16_t value;
};

// 22. 0xBNNN: Jump to address V0 + 0xNNN
struct FlowJump {
  uint16_t value;
};

// 23. 0xCXNN: Vx = rand() & 0xNN
struct RandAnd {
  uint8_t X;
  uint8_t NN;
};

// 24. 0xDXYN: Draw
struct Draw {
  uint8_t X;
  uint8_t Y;
  uint8_t N;
};
// Draws an 8 by N sprite at (VX, VY)
// Each row of 8 pixels is read as bit-coded starting from memory location I;
// I value does not change after the execution of this instruction.
// VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn,
// and to 0 if that does not happen.

// 25. 0xEX9E: Skips the next instruction if the key stored in VX(only consider the lowest nibble)
// is pressed
struct SkipIfKey {
  uint8_t X;
};

// 26. 0xEXA1: Not(SkipIfKey)
struct SkipIfNotKey {
  uint8_t X;
};

// 27. 0xFX07: VX = DelayTimer
struct GetDelay {
  uint8_t X;
};

// 28. 0xFX0A: Record KeyPress into VX
struct GetKey {
  uint8_t X;
};

// 29. 0xFX15: Set Delay as VX
struct SetDelay {
  uint8_t X;
};

// 30. 0xFX18: Set Sound timer to VX
struct SetSound {
  uint8_t X;
};

// 31. 0xFX1E: I += VX, VF is not affected, i.e. does not trigger overflow flag
struct IncI {
  uint8_t X;
};

// 32. 0xFX29: Sets I to the location of the sprite for the
// character in VX(only consider the lowest nibble).
struct SetSpriteLoc {
  uint8_t X;
};

// 33. 0xFX33: Stores the binary-coded decimal representation of VX,
// with the hundreds digit in memory at location in I,
// the tens digit at location I+1,
// and the ones digit at location I+2.
struct SetBCD {
  uint8_t X;
};

// 34. 0xFX55: Stores from V0 to VX (including VX) in memory,
// starting at address I. The offset from I is increased
// by 1 for each value written, but I itself is left unmodified.
struct RegDump {
  uint8_t X;
};

// 35. 0xFX65: Reverse of RegDump, I is still unmodified
struct RegLoad {
  uint8_t X;
};

// Variant Definition
// The Algebraic Data Type wrapping all 35 CHIP-8 instruction payloads
using Opcode = std::variant<CallMCR, DisplayClear, FlowReturn, FlowGoto, FlowCall, SkipIfXNN,
                            SkipIfNotXNN, SkipIfXY, SetX, IncrementX, AssignXY, OrEQ, AndEQ, XorEQ,
                            AddEQ, SubEQ, RShift1, SubYX, LShift1, SkipIfNotXY, SetI, FlowJump,
                            RandAnd, Draw, SkipIfKey, SkipIfNotKey, GetDelay, GetKey, SetDelay,
                            SetSound, IncI, SetSpriteLoc, SetBCD, RegDump, RegLoad>;
