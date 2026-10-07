#ifndef NES_6502_H
#define NES_6502_H

#ifndef NES_BIN_DATATYPES
#define NES_BIN_DATATYPES

#include <stdint.h>

typedef int8_t byte;
typedef int16_t word;
typedef uint8_t ubyte;
typedef uint16_t uword;
typedef uint64_t counter;

#define bool ubyte
#define false 0
#define true 1

#endif // NES_BIN_DATATYPES

#define flag_n 0x80
#define flag_v 0x40
#define flag_u 0x20
#define flag_b 0x10
#define flag_d 0x08
#define flag_i 0x04
#define flag_z 0x02
#define flag_c 0x01

typedef struct CPU_6502 {
  counter steps;

  struct reg {
    uword PC;
    ubyte A;
    ubyte X;
    ubyte Y;
    ubyte P;
    ubyte SP;
  } reg;

  struct bus {
    uword addr;
    ubyte val;
    bool write;
  } bus;

  struct instr {
    uword addr;
    bool addr_fetched;
    byte step;
    ubyte opcode;
  } instr;

  struct reset {
    ubyte step;
    bool pending;
  } reset;

  struct interrupt {
    bool breakStarted;
    bool nmiPending;
    bool irqLine;
    byte step;
  } interrupt;

  bool jammed;
} CPU_6502;

void clock_cpu_6502(CPU_6502 *c);

#endif // NES_6502_H
