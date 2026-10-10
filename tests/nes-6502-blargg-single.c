/**
 * Work only when
 * - ../assets/blargg-instr_test-v5/rom_singles/01-basics.nes
 * - ../assets/blargg-instr_test-v5/rom_singles/02-implied.nes
 * - ../assets/blargg-instr_test-v5/rom_singles/03-immediate.nes
 * - ../assets/blargg-instr_test-v5/rom_singles/04-zero_page.nes
 * - ../assets/blargg-instr_test-v5/rom_singles/05-zp_xy.nes
 * - ../assets/blargg-instr_test-v5/rom_singles/06-absolute.nes
 * - ../assets/blargg-instr_test-v5/rom_singles/07-abs_xy.nes
 * - ../assets/blargg-instr_test-v5/rom_singles/08-ind_x.nes
 * - ../assets/blargg-instr_test-v5/rom_singles/09-ind_y.nes
 * - ../assets/blargg-instr_test-v5/rom_singles/10-branches.nes
 * - ../assets/blargg-instr_test-v5/rom_singles/11-stack.nes
 * - ../assets/blargg-instr_test-v5/rom_singles/12-jmp_jsr.nes
 * - ../assets/blargg-instr_test-v5/rom_singles/13-rts.nes
 * - ../assets/blargg-instr_test-v5/rom_singles/14-rti.nes
 * - ../assets/blargg-instr_test-v5/rom_singles/15-brk.nes
 * - ../assets/blargg-instr_test-v5/rom_singles/16-special.nes
 * Are present.
 * Compiled with
 * - ../nes-6502.c
 * Using std C23+
 */

#include "../nes-6502.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define TEST_TIMEOUT 100000000ULL
#define RAM_SIZE 0x800
#define PRG_RAM_SIZE 0x2000
#define ROM_HEADER_SIZE 16
#define ROM_TRAINER_SIZE 512
#define TEST_STATUS 0x6000
#define TEST_MESSAGE 0x6004

static const ubyte BASICS_NES[] = {
#embed "../assets/blargg-instr_test-v5/rom_singles/01-basics.nes"
};

static const ubyte IMPLIED_NES[] = {
#embed "../assets/blargg-instr_test-v5/rom_singles/02-implied.nes"
};

static const ubyte IMMEDIATE_NES[] = {
#embed "../assets/blargg-instr_test-v5/rom_singles/03-immediate.nes"
};

static const ubyte ZERO_PAGE_NES[] = {
#embed "../assets/blargg-instr_test-v5/rom_singles/04-zero_page.nes"
};

static const ubyte ZP_XY_NES[] = {
#embed "../assets/blargg-instr_test-v5/rom_singles/05-zp_xy.nes"
};

static const ubyte ABSOLUTE_NES[] = {
#embed "../assets/blargg-instr_test-v5/rom_singles/06-absolute.nes"
};

static const ubyte ABS_XY_NES[] = {
#embed "../assets/blargg-instr_test-v5/rom_singles/07-abs_xy.nes"
};

static const ubyte IND_X_NES[] = {
#embed "../assets/blargg-instr_test-v5/rom_singles/08-ind_x.nes"
};

static const ubyte IND_Y_NES[] = {
#embed "../assets/blargg-instr_test-v5/rom_singles/09-ind_y.nes"
};

static const ubyte BRANCHES_NES[] = {
#embed "../assets/blargg-instr_test-v5/rom_singles/10-branches.nes"
};

static const ubyte STACK_NES[] = {
#embed "../assets/blargg-instr_test-v5/rom_singles/11-stack.nes"
};

static const ubyte JMP_JSR_NES[] = {
#embed "../assets/blargg-instr_test-v5/rom_singles/12-jmp_jsr.nes"
};

static const ubyte RTS_NES[] = {
#embed "../assets/blargg-instr_test-v5/rom_singles/13-rts.nes"
};

static const ubyte RTI_NES[] = {
#embed "../assets/blargg-instr_test-v5/rom_singles/14-rti.nes"
};

static const ubyte BRK_NES[] = {
#embed "../assets/blargg-instr_test-v5/rom_singles/15-brk.nes"
};

static const ubyte SPECIAL_NES[] = {
#embed "../assets/blargg-instr_test-v5/rom_singles/16-special.nes"
};

typedef struct TestROM {
  const char *name;
  const ubyte *data;
  size_t size;
} TestROM;

#define TEST_ROM(name, data) {name, data, sizeof(data)}

static const TestROM TEST_ROMS[] = {
    TEST_ROM("basics", BASICS_NES),
    TEST_ROM("implied", IMPLIED_NES),
    TEST_ROM("immediate", IMMEDIATE_NES),
    TEST_ROM("zero_page", ZERO_PAGE_NES),
    TEST_ROM("zp_xy", ZP_XY_NES),
    TEST_ROM("absolute", ABSOLUTE_NES),
    TEST_ROM("abs_xy", ABS_XY_NES),
    TEST_ROM("ind_x", IND_X_NES),
    TEST_ROM("ind_y", IND_Y_NES),
    TEST_ROM("branches", BRANCHES_NES),
    TEST_ROM("stack", STACK_NES),
    TEST_ROM("jmp_jsr", JMP_JSR_NES),
    TEST_ROM("rts", RTS_NES),
    TEST_ROM("rti", RTI_NES),
    TEST_ROM("brk", BRK_NES),
    TEST_ROM("special", SPECIAL_NES),
};

typedef struct Memory {
  ubyte ram[RAM_SIZE];
  ubyte prg_ram[PRG_RAM_SIZE];
  CPU_6502 cpu;
  const ubyte *prg;
  size_t prg_size;
  const char *name;
  uint64_t cycles;
} Memory;

static bool reset_memory(Memory *m, const TestROM *test) {
  if (test->size < ROM_HEADER_SIZE)
    return false;

  const ubyte *rom = test->data;

  if (rom[0] != 'N' || rom[1] != 'E' || rom[2] != 'S' || rom[3] != 0x1A) {
    fprintf(stderr, "%s: invalid iNES header\n", test->name);
    return false;
  }

  if (rom[4] == 0) {
    fprintf(stderr, "%s: no PRG-ROM present\n", test->name);
    return false;
  }

  unsigned mapper = (rom[6] >> 4) | (rom[7] & 0xF0);

  if (mapper != 0) {
    fprintf(stderr, "%s: unsupported mapper %u\n", test->name, mapper);
    return false;
  }

  size_t offset = ROM_HEADER_SIZE;

  if (rom[6] & 0x04)
    offset += ROM_TRAINER_SIZE;

  size_t prg_size = (size_t)rom[4] * 0x4000;

  if (offset > test->size || prg_size > test->size - offset) {
    fprintf(stderr, "%s: truncated PRG-ROM\n", test->name);
    return false;
  }

  memset(m, 0, sizeof(*m));

  m->name = test->name;
  m->prg = rom + offset;
  m->prg_size = prg_size;

  m->cpu.reset.pending = true;
  return true;
}

static ubyte read_memory(const Memory *m, uint16_t addr) {
  if (addr < 0x2000)
    return m->ram[addr & 0x07FF];

  if (addr >= 0x6000 && addr < 0x8000)
    return m->prg_ram[addr - 0x6000];

  if (addr >= 0x8000) {
    size_t offset = (size_t)(addr - 0x8000);

    if (m->prg_size == 0x4000)
      offset &= 0x3FFF;
    else
      offset &= 0x7FFF;

    return m->prg[offset];
  }

  return 0;
}

static void write_memory(Memory *m, uint16_t addr, ubyte value) {
  if (addr < 0x2000) {
    m->ram[addr & 0x07FF] = value;
    return;
  }

  if (addr >= 0x6000 && addr < 0x8000)
    m->prg_ram[addr - 0x6000] = value;
}

static void clock_memory(Memory *m) {
  clock_cpu_6502(&m->cpu);

  if (m->cpu.bus.write)
    write_memory(m, m->cpu.bus.addr, m->cpu.bus.val);
  else
    m->cpu.bus.val = read_memory(m, m->cpu.bus.addr);

  m->cycles++;
}

static void print_test_message(const Memory *m) {
  const ubyte *message = m->prg_ram + (TEST_MESSAGE - 0x6000);
  size_t capacity = PRG_RAM_SIZE - (TEST_MESSAGE - 0x6000);
  size_t length = 0;

  while (length < capacity && message[length] != 0)
    length++;

  if (length != 0)
    printf("%.*s\n", (int)length, (const char *)message);
}

static bool start_test(Memory *m) {
  printf("Running %s...\n", m->name);
  fflush(stdout);

  bool started = false;
  bool reset_done = false;

  while (m->cycles < TEST_TIMEOUT) {
    clock_memory(m);

    if (m->cpu.jammed) {
      printf("CPU JAMMED\n");
      return false;
    }

    if (!reset_done && !m->cpu.reset.pending) {
      reset_done = true;
      printf("Reset completed. PC: $%04X, $PC: 0x%02X\n", m->cpu.reg.PC,
             read_memory(m, m->cpu.reg.PC));
    }

    ubyte status = m->prg_ram[TEST_STATUS - 0x6000];

    if (status == 0x80) {
      started = true;
      continue;
    }

    if (!started)
      continue;

    if (status == 0x81) {
      printf("PASS: %s (%llu CPU cycles)\n", m->name,
             (unsigned long long)m->cycles);
      print_test_message(m);
      return true;
    }

    if (status < 0x80) {
      fprintf(stderr, "FAIL: %s (status 0x%02X, %llu CPU cycles)\n", m->name,
              status, (unsigned long long)m->cycles);
      print_test_message(m);
      return false;
    }

    if (m->cpu.jammed) {
      fprintf(stderr, "FAIL: %s (CPU jammed at $%04X)\n", m->name,
              m->cpu.reg.PC);
      return false;
    }
  }

  fprintf(stderr, "TIMEOUT: %s after %llu CPU cycles\n", m->name,
          (unsigned long long)m->cycles);

  print_test_message(m);
  return false;
}

static const TestROM *find_test(const char *name) {
  for (size_t i = 0; i < sizeof(TEST_ROMS) / sizeof(TEST_ROMS[0]); i++) {
    if (strcmp(name, TEST_ROMS[i].name) == 0)
      return &TEST_ROMS[i];
  }

  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr,
            "Usage: %s "
            "<basics|implied|immediate|zero_page|zp_xy|absolute|abs_xy|ind_x|"
            "ind_y|branches|stack|jmp_jsr|rts|rti|brk|special>\n",
            argv[0]);
    return 2;
  }

  const TestROM *test = find_test(argv[1]);

  if (test == NULL) {
    fprintf(stderr, "Unknown test suite '%s'\n", argv[1]);
    return 2;
  }

  Memory mem;

  if (!reset_memory(&mem, test)) {
    fprintf(stderr, "Unable to initialize %s\n", test->name);
    return 2;
  }

  return start_test(&mem) ? 0 : 1;
}
