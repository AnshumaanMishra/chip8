# CHIP-8 Emulator in C++

---

Basic CHIP-8 Emulator written in C++

## Specifications

CHIP-8 Has the following specifications:

- **Memory:** 4096 bytes
- **Display:** 64 × 32 pixels, monochrome
- A **Program Counter**
- One **16-bit Index Register**
- A **Stack** for 16-bit addresses
- An **8-bit delay timer**  
- An **8-bit sound timer**  
- 16 **General Purpose Variable Registers**

## Memory  

The memory map looks as follows:

- `0x000` -> `0x1FF` (0 to 511) : CHIP-8 Interpreter
- `0x050` -> `0x0A0` (75 to 150) : 4 × 5 pixel font set  
- `0x200` -> `0xFFF` (512 to 4095) : Program ROM and work RAM
