/**
 * Work only when compiled with:
 * - nes-6502.c
 */

#include "nes-6502.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Memory {
  ubyte ram[0x800];
  CPU_6502 cpu;
  ubyte *prg;
  size_t prg_size;
} Memory;

void read_cpu(Memory *m) {
  uword addr = m->cpu.bus.addr;

  if (addr <= 0x1FFF) {
    m->cpu.bus.val = m->ram[addr & 0x7FF];
  } else if (addr >= 0x8000 && addr <= 0xFFFF && m->prg) {
    size_t offset = addr - 0x8000;

    if (m->prg_size == 0x4000) {
      offset &= 0x3FFF;
    } else {
      offset %= m->prg_size;
    }

    m->cpu.bus.val = m->prg[offset];
  } else {
    m->cpu.bus.val = 0;
  }
}

void log_cpu(const CPU_6502 *c) {
  const char *g = "\033[32m";
  const char *r = "\033[31m";
  const char *x = "\033[0m";

  printf("STEP: %lu %s%s%s\n  $%04X A:%02X X:%02X Y:%02X SP:%02X\n  "
         "FLAGS: %sN %sV %s█ %sB %sD %sI %sZ %sC%s\n  %sRESET%s %u\n  %sIRQ "
         "%sNMI %sBRK%s %u\n  OPCODE: %02X %s$%04X%s %u\n  BUS: $%04X %02X "
         "%s%s%s\n  %sOAMDMA%s $%04X %02X %s%s%s\n\n",
         c->steps, c->jammed ? r : g, c->jammed ? "JAMMED" : "RUNNING", x,
         c->reg.PC, c->reg.A, c->reg.X, c->reg.Y, c->reg.SP,
         c->reg.P & flag_n ? g : r, c->reg.P & flag_v ? g : r,
         c->reg.P & flag_u ? g : r, c->reg.P & flag_b ? g : r,
         c->reg.P & flag_d ? g : r, c->reg.P & flag_i ? g : r,
         c->reg.P & flag_z ? g : r, c->reg.P & flag_c ? g : r, x,
         c->reset.pending ? g : r, x, c->reset.step,
         c->interrupt.irqLine ? g : r, c->interrupt.nmiPending ? g : r,
         c->interrupt.breakStarted ? g : r, x, c->interrupt.step,
         c->instr.opcode, c->instr.addr_fetched ? g : r, c->instr.addr, x,
         c->instr.step, c->bus.addr, c->bus.val, c->bus.write ? g : r,
         c->bus.write ? "WRITE" : "READ", x, c->oamdma.active ? g : r, x,
         c->oamdma.addr, c->bus.val, c->oamdma.step & 1 ? r : g,
         c->oamdma.step & 1 ? "READ" : "WRITE", x);
}

bool reset_memory(Memory *m, char *filename) {
  FILE *file = fopen(filename, "rb");

  if (!file) {
    perror(filename);
    return false;
  }

  unsigned char header[16];

  if (fread(header, 1, sizeof(header), file) != sizeof(header)) {
    printf("Error: Failed to read iNES header.\n");
    fclose(file);
    return false;
  }

  if (memcmp(header, "NES\x1A", 4) != 0) {
    printf("Error: Invalid iNES file.\n");
    fclose(file);
    return false;
  }

  size_t prg_size = (size_t)header[4] * 0x4000;
  size_t chr_size = (size_t)header[5] * 0x2000;
  size_t trainer_size = (header[6] & 0x04) ? 512 : 0;

  if (prg_size == 0) {
    printf("Error: ROM contains no PRG ROM.\n");
    fclose(file);
    return false;
  }

  if (fseek(file, (long)trainer_size, SEEK_CUR) != 0) {
    printf("Error: Failed to seek past trainer.\n");
    fclose(file);
    return false;
  }

  m->prg = malloc(prg_size);

  if (!m->prg) {
    printf("Error: Failed to allocate PRG ROM.\n");
    fclose(file);
    return false;
  }

  if (fread(m->prg, 1, prg_size, file) != prg_size) {
    printf("Error: Failed to read PRG ROM.\n");
    free(m->prg);
    m->prg = NULL;
    fclose(file);
    return false;
  }

  fclose(file);
  m->prg_size = prg_size;
  return true;
}

int main(int argc, char **argv) {
  if (argc < 2 || argc > 3) {
    printf("Usage: %s <nes-file> [--step]\n", argv[0]);
    printf("Emulate CPU with mapper 0 like address mapping.\n");
    return 0;
  }

  char *filename = argv[1];
  bool step = false;

  if (argc == 3) {
    if (strcmp(argv[2], "--step") == 0) {
      step = true;
    } else {
      printf("Invalid option %s\n", argv[2]);
      return 1;
    }
  }

  Memory mem = {0};

  if (reset_memory(&mem, filename)) {
    mem.cpu.reg.P |= flag_u;

    while (true) {
      clock_cpu_6502(&mem.cpu);
      if (!mem.cpu.bus.write) {
        read_cpu(&mem);
      }

      log_cpu(&mem.cpu);

      if (step) {
        char input[2];
        printf("Choose option: RESET [1], NMI [2], IRQ HI [3], IRQ LO [4]  ");
        fgets(input, sizeof(input), stdin);

        if (input[0] == '1') {
          mem.cpu.reset.pending |= true;
        } else if (input[0] == '2') {
          mem.cpu.interrupt.nmiPending |= true;
        } else if (input[0] == '3') {
          mem.cpu.interrupt.irqLine |= true;
        } else if (input[0] == '4') {
          mem.cpu.interrupt.irqLine &= false;
        }
      }
    }

    free(mem.prg);
    return 0;
  }

  return 1;
}
