/**
 * Work only when
 * - ../assets/blargg-instr_test-v5/all_instrs.nes
 * - ../assets/blargg-instr_test-v5/official_only.nes
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

#define TEST_TIMEOUT 1000000000ULL
#define RAM_SIZE 0x800
#define PRG_RAM_SIZE 0x2000
#define ROM_HEADER_SIZE 16
#define ROM_TRAINER_SIZE 512
#define TEST_STATUS 0x6000
#define TEST_MESSAGE 0x6004

static const ubyte ALL_NES[] = {
#embed "../assets/blargg-instr_test-v5/all_instrs.nes"
};

static const ubyte OFFICIAL_NES[] = {
#embed "../assets/blargg-instr_test-v5/official_only.nes"
};

typedef struct TestROM {
  const char *name;
  const ubyte *data;
  size_t size;
} TestROM;

#define TEST_ROM(name, data) {name, data, sizeof(data)}

static const TestROM TEST_ROMS[] = {
    TEST_ROM("all", ALL_NES),
    TEST_ROM("official", OFFICIAL_NES),
};

typedef struct MMC1 {
  ubyte shift;
  ubyte control;
  ubyte chr_bank0;
  ubyte chr_bank1;
  ubyte prg_bank;
} MMC1;

typedef struct Memory {
  ubyte ram[RAM_SIZE];
  ubyte prg_ram[PRG_RAM_SIZE];
  CPU_6502 cpu;
  const ubyte *prg;
  size_t prg_size;
  size_t prg_banks;
  unsigned mapper;
  MMC1 mmc1;
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

  if ((rom[7] & 0x0C) == 0x08) {
    mapper |= (rom[8] & 0x0F) << 8;
  }

  if (mapper != 0 && mapper != 1) {
    fprintf(stderr, "%s: unsupported mapper %u\n", test->name, mapper);
    return false;
  }

  size_t offset = ROM_HEADER_SIZE;

  if (rom[6] & 0x04)
    offset += ROM_TRAINER_SIZE;

  size_t prg_size;

  if ((rom[7] & 0x0C) == 0x08) {
    unsigned exponent = rom[9] & 0x0F;
    unsigned multiplier = ((rom[9] >> 4) & 0x0F) * 2 + 1;

    if (exponent >= sizeof(size_t) * 8)
      return false;

    prg_size = ((size_t)1 << exponent) * multiplier;
  } else {
    prg_size = (size_t)rom[4] * 0x4000;
  }

  if (offset > test->size || prg_size > test->size - offset) {
    fprintf(stderr, "%s: truncated PRG-ROM\n", test->name);
    return false;
  }

  memset(m, 0, sizeof(*m));

  m->name = test->name;
  m->prg = rom + offset;
  m->prg_size = prg_size;
  m->prg_banks = prg_size / 0x4000;
  m->mapper = mapper;

  if (m->prg_banks == 0 || prg_size % 0x4000 != 0) {
    fprintf(stderr, "%s: invalid PRG-ROM size\n", test->name);
    return false;
  }

  m->mmc1.shift = 0x10;
  m->mmc1.control = 0x0C;

  m->cpu.reset.pending = true;

  return true;
}

static void mmc1_write(Memory *m, uint16_t addr, ubyte value) {
  MMC1 *mmc1 = &m->mmc1;

  if (value & 0x80) {
    mmc1->shift = 0x10;
    mmc1->control |= 0x0C;
    return;
  }

  bool complete = (mmc1->shift & 1) != 0;

  mmc1->shift >>= 1;
  mmc1->shift |= (value & 1) << 4;

  if (!complete)
    return;

  ubyte data = mmc1->shift;

  switch ((addr >> 13) & 3) {
  case 0:
    mmc1->control = data;
    break;
  case 1:
    mmc1->chr_bank0 = data;
    break;
  case 2:
    mmc1->chr_bank1 = data;
    break;
  case 3:
    mmc1->prg_bank = data;
    break;
  }

  mmc1->shift = 0x10;
}

static ubyte read_prg(const Memory *m, uint16_t addr) {
  size_t offset = (size_t)(addr - 0x8000);

  if (m->mapper == 0) {
    if (m->prg_size == 0x4000)
      offset &= 0x3FFF;

    return m->prg[offset];
  }

  const MMC1 *mmc1 = &m->mmc1;
  size_t bank = mmc1->prg_bank & 0x0F;
  size_t slot = offset >> 14;
  size_t mode = (mmc1->control >> 2) & 3;

  switch (mode) {
  case 0:
  case 1:
    bank = (bank & ~(size_t)1) + slot;
    break;

  case 2:
    bank = slot == 0 ? 0 : bank;
    break;

  case 3:
    bank = slot == 0 ? bank : m->prg_banks - 1;
    break;
  }

  bank %= m->prg_banks;

  return m->prg[bank * 0x4000 + (offset & 0x3FFF)];
}

static ubyte read_memory(const Memory *m, uint16_t addr) {
  if (addr < 0x2000)
    return m->ram[addr & 0x07FF];

  if (addr >= 0x6000 && addr < 0x8000)
    return m->prg_ram[addr - 0x6000];

  if (addr >= 0x8000)
    return read_prg(m, addr);

  return 0;
}

static void write_memory(Memory *m, uint16_t addr, ubyte value) {
  if (addr < 0x2000) {
    m->ram[addr & 0x07FF] = value;
    return;
  }

  if (addr >= 0x6000 && addr < 0x8000) {
    if (m->mapper == 1 && (m->mmc1.prg_bank & 0x10))
      return;

    m->prg_ram[addr - 0x6000] = value;
    return;
  }

  if (addr >= 0x8000 && m->mapper == 1)
    mmc1_write(m, addr, value);
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

  if (length)
    printf("%.*s\n", (int)length, (const char *)message);
}

static bool start_test(Memory *m) {
  printf("Running %s (mapper %u)...\n", m->name, m->mapper);
  fflush(stdout);

  while (m->cycles < TEST_TIMEOUT) {
    clock_memory(m);

    if (m->cpu.jammed) {
      fprintf(stderr, "CPU JAMMED at $%04X\n", m->cpu.reg.PC);
      return false;
    }

    ubyte status = m->prg_ram[TEST_STATUS - 0x6000];

    if (status == 0x81) {
      printf("PASS: %s (%llu CPU cycles)\n", m->name,
             (unsigned long long)m->cycles);
      print_test_message(m);
      return true;
    }

    if (status != 0 && status != 0x80) {
      fprintf(stderr, "FAIL: %s (status 0x%02X, PC=$%04X, %llu CPU cycles)\n",
              m->name, status, m->cpu.reg.PC, (unsigned long long)m->cycles);
      print_test_message(m);
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
    fprintf(stderr, "Usage: %s <all|official>\n", argv[0]);
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
