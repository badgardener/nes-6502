/**
 * Work only when compiled with:
 * - nes-6502.c
 * And "../assets/nestest.nes" is present.
 * Compile with C23+
 */

#include "../nes-6502.h"

#include <stdio.h>
#include <string.h>

static const ubyte FILE_BYTES[] = {
#embed "../assets/nestest.nes"
};

#define FILE_SIZE sizeof(FILE_BYTES)

typedef struct Memory {
  ubyte ram[0x800];
  CPU_6502 cpu;
  const ubyte *prg;
  size_t prg_size;
} Memory;

typedef enum {
  IMP,
  ACC,
  IMM,
  ZP0,
  ZPX,
  ZPY,
  REL,
  ABS,
  ABX,
  ABY,
  IND,
  IZX,
  IZY
} AddrMode;

typedef struct {
  const char *name;
  AddrMode mode;
  ubyte len;
} Opcode;

#define O(n, m, l) {n, m, l}

static const Opcode opcodes[256] = {
    O("BRK", IMP, 1), O("ORA", IZX, 2), O("KIL", IMP, 1), O("SLO", IZX, 2),
    O("NOP", ZP0, 2), O("ORA", ZP0, 2), O("ASL", ZP0, 2), O("SLO", ZP0, 2),
    O("PHP", IMP, 1), O("ORA", IMM, 2), O("ASL", ACC, 1), O("ANC", IMM, 2),
    O("NOP", ABS, 3), O("ORA", ABS, 3), O("ASL", ABS, 3), O("SLO", ABS, 3),
    O("BPL", REL, 2), O("ORA", IZY, 2), O("KIL", IMP, 1), O("SLO", IZY, 2),
    O("NOP", ZPX, 2), O("ORA", ZPX, 2), O("ASL", ZPX, 2), O("SLO", ZPX, 2),
    O("CLC", IMP, 1), O("ORA", ABY, 3), O("NOP", IMP, 1), O("SLO", ABY, 3),
    O("NOP", ABX, 3), O("ORA", ABX, 3), O("ASL", ABX, 3), O("SLO", ABX, 3),
    O("JSR", ABS, 3), O("AND", IZX, 2), O("KIL", IMP, 1), O("RLA", IZX, 2),
    O("BIT", ZP0, 2), O("AND", ZP0, 2), O("ROL", ZP0, 2), O("RLA", ZP0, 2),
    O("PLP", IMP, 1), O("AND", IMM, 2), O("ROL", ACC, 1), O("ANC", IMM, 2),
    O("BIT", ABS, 3), O("AND", ABS, 3), O("ROL", ABS, 3), O("RLA", ABS, 3),
    O("BMI", REL, 2), O("AND", IZY, 2), O("KIL", IMP, 1), O("RLA", IZY, 2),
    O("NOP", ZPX, 2), O("AND", ZPX, 2), O("ROL", ZPX, 2), O("RLA", ZPX, 2),
    O("SEC", IMP, 1), O("AND", ABY, 3), O("NOP", IMP, 1), O("RLA", ABY, 3),
    O("NOP", ABX, 3), O("AND", ABX, 3), O("ROL", ABX, 3), O("RLA", ABX, 3),
    O("RTI", IMP, 1), O("EOR", IZX, 2), O("KIL", IMP, 1), O("SRE", IZX, 2),
    O("NOP", ZP0, 2), O("EOR", ZP0, 2), O("LSR", ZP0, 2), O("SRE", ZP0, 2),
    O("PHA", IMP, 1), O("EOR", IMM, 2), O("LSR", ACC, 1), O("ALR", IMM, 2),
    O("JMP", ABS, 3), O("EOR", ABS, 3), O("LSR", ABS, 3), O("SRE", ABS, 3),
    O("BVC", REL, 2), O("EOR", IZY, 2), O("KIL", IMP, 1), O("SRE", IZY, 2),
    O("NOP", ZPX, 2), O("EOR", ZPX, 2), O("LSR", ZPX, 2), O("SRE", ZPX, 2),
    O("CLI", IMP, 1), O("EOR", ABY, 3), O("NOP", IMP, 1), O("SRE", ABY, 3),
    O("NOP", ABX, 3), O("EOR", ABX, 3), O("LSR", ABX, 3), O("SRE", ABX, 3),
    O("RTS", IMP, 1), O("ADC", IZX, 2), O("KIL", IMP, 1), O("RRA", IZX, 2),
    O("NOP", ZP0, 2), O("ADC", ZP0, 2), O("ROR", ZP0, 2), O("RRA", ZP0, 2),
    O("PLA", IMP, 1), O("ADC", IMM, 2), O("ROR", ACC, 1), O("ARR", IMM, 2),
    O("JMP", IND, 3), O("ADC", ABS, 3), O("ROR", ABS, 3), O("RRA", ABS, 3),
    O("BVS", REL, 2), O("ADC", IZY, 2), O("KIL", IMP, 1), O("RRA", IZY, 2),
    O("NOP", ZPX, 2), O("ADC", ZPX, 2), O("ROR", ZPX, 2), O("RRA", ZPX, 2),
    O("SEI", IMP, 1), O("ADC", ABY, 3), O("NOP", IMP, 1), O("RRA", ABY, 3),
    O("NOP", ABX, 3), O("ADC", ABX, 3), O("ROR", ABX, 3), O("RRA", ABX, 3),
    O("NOP", IMM, 2), O("STA", IZX, 2), O("NOP", IMM, 2), O("SAX", IZX, 2),
    O("STY", ZP0, 2), O("STA", ZP0, 2), O("STX", ZP0, 2), O("SAX", ZP0, 2),
    O("DEY", IMP, 1), O("NOP", IMM, 2), O("TXA", IMP, 1), O("XAA", IMM, 2),
    O("STY", ABS, 3), O("STA", ABS, 3), O("STX", ABS, 3), O("SAX", ABS, 3),
    O("BCC", REL, 2), O("STA", IZY, 2), O("KIL", IMP, 1), O("AHX", IZY, 2),
    O("STY", ZPX, 2), O("STA", ZPX, 2), O("STX", ZPY, 2), O("SAX", ZPY, 2),
    O("TYA", IMP, 1), O("STA", ABY, 3), O("TXS", IMP, 1), O("TAS", ABY, 3),
    O("SHY", ABX, 3), O("STA", ABX, 3), O("SHX", ABY, 3), O("AHX", ABY, 3),
    O("LDY", IMM, 2), O("LDA", IZX, 2), O("LDX", IMM, 2), O("LAX", IZX, 2),
    O("LDY", ZP0, 2), O("LDA", ZP0, 2), O("LDX", ZP0, 2), O("LAX", ZP0, 2),
    O("TAY", IMP, 1), O("LDA", IMM, 2), O("TAX", IMP, 1), O("LAX", IMM, 2),
    O("LDY", ABS, 3), O("LDA", ABS, 3), O("LDX", ABS, 3), O("LAX", ABS, 3),
    O("BCS", REL, 2), O("LDA", IZY, 2), O("KIL", IMP, 1), O("LAX", IZY, 2),
    O("LDY", ZPX, 2), O("LDA", ZPX, 2), O("LDX", ZPY, 2), O("LAX", ZPY, 2),
    O("CLV", IMP, 1), O("LDA", ABY, 3), O("TSX", IMP, 1), O("LAS", ABY, 3),
    O("LDY", ABX, 3), O("LDA", ABX, 3), O("LDX", ABY, 3), O("LAX", ABY, 3),
    O("CPY", IMM, 2), O("CMP", IZX, 2), O("NOP", IMM, 2), O("DCP", IZX, 2),
    O("CPY", ZP0, 2), O("CMP", ZP0, 2), O("DEC", ZP0, 2), O("DCP", ZP0, 2),
    O("INY", IMP, 1), O("CMP", IMM, 2), O("DEX", IMP, 1), O("AXS", IMM, 2),
    O("CPY", ABS, 3), O("CMP", ABS, 3), O("DEC", ABS, 3), O("DCP", ABS, 3),
    O("BNE", REL, 2), O("CMP", IZY, 2), O("KIL", IMP, 1), O("DCP", IZY, 2),
    O("NOP", ZPX, 2), O("CMP", ZPX, 2), O("DEC", ZPX, 2), O("DCP", ZPX, 2),
    O("CLD", IMP, 1), O("CMP", ABY, 3), O("NOP", IMP, 1), O("DCP", ABY, 3),
    O("NOP", ABX, 3), O("CMP", ABX, 3), O("DEC", ABX, 3), O("DCP", ABX, 3),
    O("CPX", IMM, 2), O("SBC", IZX, 2), O("NOP", IMM, 2), O("ISC", IZX, 2),
    O("CPX", ZP0, 2), O("SBC", ZP0, 2), O("INC", ZP0, 2), O("ISC", ZP0, 2),
    O("INX", IMP, 1), O("SBC", IMM, 2), O("NOP", IMP, 1), O("SBC", IMM, 2),
    O("CPX", ABS, 3), O("SBC", ABS, 3), O("INC", ABS, 3), O("ISC", ABS, 3),
    O("BEQ", REL, 2), O("SBC", IZY, 2), O("KIL", IMP, 1), O("ISC", IZY, 2),
    O("NOP", ZPX, 2), O("SBC", ZPX, 2), O("INC", ZPX, 2), O("ISC", ZPX, 2),
    O("SED", IMP, 1), O("SBC", ABY, 3), O("NOP", IMP, 1), O("ISC", ABY, 3),
    O("NOP", ABX, 3), O("SBC", ABX, 3), O("INC", ABX, 3), O("ISC", ABX, 3)};

static ubyte peek_cpu(const Memory *m, uword addr) {
  if (addr <= 0x1FFF)
    return m->ram[addr & 0x07FF];

  if (addr >= 0x8000 && m->prg) {
    size_t offset = (size_t)(addr - 0x8000);
    if (m->prg_size == 0x4000)
      offset &= 0x3FFF;
    else
      offset %= m->prg_size;
    return m->prg[offset];
  }

  return 0;
}

static void read_cpu(Memory *m) {
  m->cpu.bus.val = peek_cpu(m, m->cpu.bus.addr);
}

static bool reset_memory(Memory *m) {
  const ubyte *header = FILE_BYTES;
  size_t file_size = FILE_SIZE;

  if (file_size < 16 || memcmp(header, "NES\x1A", 4) != 0) {
    printf("Error: Invalid iNES file.\n");
    return false;
  }

  size_t prg_size = (size_t)header[4] * 0x4000;
  size_t trainer_size = (header[6] & 0x04) ? 512 : 0;
  size_t offset = 16 + trainer_size;

  if (!prg_size || offset > file_size || prg_size > file_size - offset) {
    printf("Error: Invalid PRG ROM size.\n");
    return false;
  }

  m->prg = FILE_BYTES + offset;
  m->prg_size = prg_size;
  return true;
}

static void disassemble(const Memory *m, uword pc, char *out, size_t size) {
  ubyte opcode = peek_cpu(m, pc);
  ubyte lo = peek_cpu(m, (uword)(pc + 1));
  ubyte hi = peek_cpu(m, (uword)(pc + 2));
  uword addr = (uword)(lo | (hi << 8));
  ubyte value = 0;
  uword effective = 0;
  const Opcode *op = &opcodes[opcode];

  switch (op->mode) {
  case IMP:
    snprintf(out, size, "%s", op->name);
    break;
  case ACC:
    snprintf(out, size, "%s A", op->name);
    break;
  case IMM:
    snprintf(out, size, "%s #$%02X", op->name, lo);
    break;
  case ZP0:
    effective = lo;
    value = peek_cpu(m, effective);
    snprintf(out, size, "%s $%02X = %02X", op->name, lo, value);
    break;
  case ZPX:
    effective = (ubyte)(lo + m->cpu.reg.X);
    value = peek_cpu(m, effective);
    snprintf(out, size, "%s $%02X,X @ %02X = %02X", op->name, lo,
             (ubyte)effective, value);
    break;
  case ZPY:
    effective = (ubyte)(lo + m->cpu.reg.Y);
    value = peek_cpu(m, effective);
    snprintf(out, size, "%s $%02X,Y @ %02X = %02X", op->name, lo,
             (ubyte)effective, value);
    break;
  case REL:
    effective = (uword)(pc + 2 + (int8_t)lo);
    snprintf(out, size, "%s $%04X", op->name, effective);
    break;
  case ABS:
    effective = addr;
    if (!strcmp(op->name, "JMP") || !strcmp(op->name, "JSR"))
      snprintf(out, size, "%s $%04X", op->name, addr);
    else {
      value = peek_cpu(m, effective);
      snprintf(out, size, "%s $%04X = %02X", op->name, addr, value);
    }
    break;
  case ABX:
    effective = (uword)(addr + m->cpu.reg.X);
    value = peek_cpu(m, effective);
    snprintf(out, size, "%s $%04X,X @ %04X = %02X", op->name, addr, effective,
             value);
    break;
  case ABY:
    effective = (uword)(addr + m->cpu.reg.Y);
    value = peek_cpu(m, effective);
    snprintf(out, size, "%s $%04X,Y @ %04X = %02X", op->name, addr, effective,
             value);
    break;
  case IND:
    effective =
        (uword)((peek_cpu(m, addr) & 0xFF) |
                (peek_cpu(m, (uword)((addr & 0xFF00) | ((addr + 1) & 0x00FF))
                                 << 8)));
    snprintf(out, size, "%s ($%04X) = %04X", op->name, addr, effective);
    break;
  case IZX:
    effective = (ubyte)(lo + m->cpu.reg.X);
    addr = (uword)(peek_cpu(m, (ubyte)effective) |
                   (peek_cpu(m, (ubyte)(effective + 1)) << 8));
    value = peek_cpu(m, addr);
    snprintf(out, size, "%s ($%02X,X) @ %02X = %04X = %02X", op->name, lo,
             (ubyte)effective, addr, value);
    break;
  case IZY:
    effective = (uword)(peek_cpu(m, lo) | (peek_cpu(m, (ubyte)(lo + 1)) << 8));
    addr = effective;
    effective = (uword)(addr + m->cpu.reg.Y);
    value = peek_cpu(m, effective);
    snprintf(out, size, "%s ($%02X),Y = %04X @ %04X = %02X", op->name, lo, addr,
             effective, value);
    break;
  }

  (void)op->len;
}

static void log_cpu(const Memory *m) {
  const CPU_6502 *c = &m->cpu;
  uword pc = c->reg.PC;
  ubyte opcode = peek_cpu(m, pc);
  ubyte lo = peek_cpu(m, (uword)(pc + 1));
  ubyte hi = peek_cpu(m, (uword)(pc + 2));
  const Opcode *op = &opcodes[opcode];
  char instruction[96];
  disassemble(m, pc, instruction, sizeof instruction);

  printf("%04X  %02X", pc, opcode);
  if (op->len >= 2)
    printf(" %02X", lo);
  else
    printf("   ");
  if (op->len >= 3)
    printf(" %02X", hi);
  else
    printf("   ");

  printf("  %-31s A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%lu\n", instruction,
         c->reg.A, c->reg.X, c->reg.Y, c->reg.P, c->reg.SP, c->steps);
}

static void clock_cpu(Memory *m) {
  m->cpu.instr.finished = false;

  do {
    clock_cpu_6502(&m->cpu);

    if (!m->cpu.bus.write)
      read_cpu(m);
  } while (!m->cpu.instr.finished || m->cpu.oamdma.active);
}

int main(void) {
  Memory mem = {0};

  if (!reset_memory(&mem))
    return 1;

  mem.cpu.reg.PC = 0xC000;
  mem.cpu.reg.P = 0x24;
  mem.cpu.bus.val = 0x4C;
  mem.cpu.reg.SP = 0xFD;
  mem.cpu.steps = 7;

  for (unsigned i = 0; i < 8991; i++) {
    log_cpu(&mem);
    clock_cpu(&mem);
  }

  return 0;
}
