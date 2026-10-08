#include "nes-6502.h"

static inline void set_lsb(uword *w, ubyte v) { *w = (*w & 0xFF00) | v; }
static inline void set_msb(uword *w, ubyte v) { *w = (v << 8) | (*w & 0xFF); }
static inline ubyte get_lsb(const uword *w) { return *w & 0xFF; }
static inline ubyte get_msb(const uword *w) { return *w >> 8; }

static inline void schedule_read(CPU_6502 *c, uword addr) {
  c->bus.addr = (uword)addr;
  c->bus.write = false;
}

static inline void schedule_write(CPU_6502 *c, uword addr, ubyte val) {
  c->bus.addr = (uword)addr;
  c->bus.val = (ubyte)val;
  c->bus.write = true;
}

static inline ubyte read_bus(const CPU_6502 *c) { return c->bus.val; }

static inline void schedule_pull(CPU_6502 *c) {
  schedule_read(c, 0x100 | (++c->reg.SP));
}

static inline void schedule_push(CPU_6502 *c, ubyte val) {
  schedule_write(c, 0x100 | (c->reg.SP--), val);
}

static inline ubyte get_pcl(const CPU_6502 *c) { return get_lsb(&c->reg.PC); }
static inline ubyte get_pch(const CPU_6502 *c) { return get_msb(&c->reg.PC); }
static inline void set_pcl(CPU_6502 *c, ubyte val) { set_lsb(&c->reg.PC, val); }
static inline void set_pch(CPU_6502 *c, ubyte val) { set_msb(&c->reg.PC, val); }

static inline void set_flag(CPU_6502 *c, ubyte f) { c->reg.P |= f; }
static inline void clear_flag(CPU_6502 *c, ubyte f) { c->reg.P &= ~f; }
static inline bool get_flag(const CPU_6502 *c, ubyte f) {
  return (c->reg.P & f) != 0;
}

static inline void set_flag_zn(CPU_6502 *c, ubyte v) {
  if (v == 0) {
    set_flag(c, flag_z);
  } else {
    clear_flag(c, flag_z);
  }

  if (v & 0x80) {
    set_flag(c, flag_n);
  } else {
    clear_flag(c, flag_n);
  }
}

static inline bool interrupt(const CPU_6502 *c) {
  bool v = c->interrupt.step > 0;
  v |= c->interrupt.breakStarted;
  v |= c->interrupt.nmiPending;
  v |= (c->interrupt.irqLine && !get_flag(c, flag_i));
  v &= (c->instr.step == 0);
  return v;
}

static void do_reset_cycle(CPU_6502 *c) {
  switch (c->reset.step) {
  case 0: {
    schedule_read(c, c->reg.PC);
    break;
  }

  case 1: {
    c->interrupt.step = 0;
    c->interrupt.irqLine = false;
    c->interrupt.nmiPending = false;
    c->interrupt.breakStarted = false;

    c->instr.addr = 0;
    c->instr.ptr = 0;
    c->instr.addr_fetched = false;
    c->instr.opcode = 0;
    c->instr.step = 0;

    c->oamdma.active = false;
    c->oamdma.step = 0;
    c->oamdma.addr = 0;

    c->jammed = false;
    schedule_read(c, c->reg.PC);
    break;
  }

  case 2: {
    set_flag(c, flag_u);
    set_flag(c, flag_i);

    schedule_read(c, 0x100 | c->reg.SP);
    break;
  }

  case 3: {
    c->reg.SP--;
    schedule_read(c, 0x100 | c->reg.SP);
    break;
  }

  case 4: {
    c->reg.SP--;
    schedule_read(c, 0x100 | c->reg.SP);
    break;
  }

  case 5: {
    c->reg.SP--;
    schedule_read(c, 0xFFFC);
    break;
  }

  case 6: {
    set_pcl(c, read_bus(c));
    schedule_read(c, 0xFFFD);
    break;
  }

  case 7: {
    set_pch(c, read_bus(c));
    c->reset.step = -1;
    c->reset.pending = false;

    schedule_read(c, c->reg.PC);
    break;
  }
  }
}

static void do_interrupt_cycle(CPU_6502 *c) {
  switch (c->interrupt.step) {
  case 0: {
    schedule_read(c, c->reg.PC);
    break;
  }

  case 1: {
    schedule_read(c, c->reg.PC);
    if (c->interrupt.breakStarted) {
      c->reg.PC++;
    }

    break;
  }

  case 2: {
    schedule_push(c, get_pch(c));
    break;
  }

  case 3: {
    schedule_push(c, get_pcl(c));
    break;
  }

  case 4: {
    if (c->interrupt.breakStarted) {
      schedule_push(c, c->reg.P | flag_b);
    } else {
      schedule_push(c, c->reg.P & ~flag_b);
    }

    break;
  }

  case 5: {
    c->interrupt.breakStarted = false;
    set_flag(c, flag_i);

    if (c->interrupt.nmiPending) {
      c->interrupt.nmiPending = false;
      schedule_read(c, 0xFFFA);
    } else {
      schedule_read(c, 0xFFFE);
    }

    break;
  }

  case 6: {
    set_pcl(c, read_bus(c));
    schedule_read(c, c->bus.addr + 1);
    break;
  }

  case 7: {
    set_pch(c, read_bus(c));
    c->interrupt.step = -1;

    schedule_read(c, c->reg.PC);
    return;
  }
  }
}

static void do_oamdma_cycle(CPU_6502 *c) {
  if (c->oamdma.step == 0) {
    c->oamdma.step = 512 + (c->steps & 1);
    c->oamdma.addr = read_bus(c) << 8;
    schedule_read(c, c->oamdma.addr);
    return;
  }

  if (c->oamdma.step == 513) {
    schedule_read(c, c->oamdma.addr);
  } else if (c->oamdma.step & 1) {
    c->oamdma.addr++;
    schedule_write(c, 0x2004, read_bus(c));
  } else {
    schedule_read(c, c->oamdma.addr);
  }

  if (--c->oamdma.step == 0) {
    c->oamdma.active = false;
  }
}

static void fetch_imm(CPU_6502 *c) {
  c->instr.addr = c->reg.PC;
  c->reg.PC++;
  c->instr.addr_fetched = true;
}

static void fetch_zpg(CPU_6502 *c) {
  switch (c->instr.step) {
  case 0: {
    schedule_read(c, c->reg.PC);
    break;
  }

  case 1: {
    c->instr.addr = read_bus(c);
    c->reg.PC++;
    c->instr.addr_fetched = true;
    c->instr.step = 0;
    break;
  }
  }
}

static void __fetch_zp_n(CPU_6502 *c, ubyte *v) {
  switch (c->instr.step) {
  case 0: {
    schedule_read(c, c->reg.PC);
    break;
  }

  case 1: {
    c->instr.ptr = read_bus(c);
    c->reg.PC++;
    schedule_read(c, c->instr.ptr);
    break;
  }

  case 2: {
    c->instr.addr = (c->instr.ptr + *v) & 0xFF;
    c->instr.addr_fetched = true;
    c->instr.step = 0;
    break;
  }
  }
}

static inline void fetch_zpx(CPU_6502 *c) { __fetch_zp_n(c, &c->reg.X); }
static inline void fetch_zpy(CPU_6502 *c) { __fetch_zp_n(c, &c->reg.Y); }

static void fetch_abs(CPU_6502 *c) {
  switch (c->instr.step) {
  case 0: {
    schedule_read(c, c->reg.PC);
    break;
  }

  case 1: {
    set_lsb(&c->instr.addr, read_bus(c));
    c->reg.PC++;
    schedule_read(c, c->reg.PC);
    break;
  }

  case 2: {
    set_msb(&c->instr.addr, read_bus(c));
    c->reg.PC++;
    c->instr.addr_fetched = true;
    c->instr.step = 0;
    break;
  }
  }
}

static void __fetch_ab_n(CPU_6502 *c, ubyte *v, bool force_page_cross) {
  switch (c->instr.step) {
  case 0: {
    schedule_read(c, c->reg.PC);
    break;
  }

  case 1: {
    set_lsb(&c->instr.ptr, read_bus(c));
    c->reg.PC++;
    schedule_read(c, c->reg.PC);
    break;
  }

  case 2: {
    set_msb(&c->instr.ptr, read_bus(c));
    c->reg.PC++;
    c->instr.addr = c->instr.ptr + *v;

    if (!force_page_cross &&
        (c->instr.addr & 0xFF00) == (c->instr.ptr & 0xFF00)) {
      c->instr.addr_fetched = true;
      c->instr.step = 0;
      return;
    }

    schedule_read(c, (c->instr.ptr & 0xFF00) | get_lsb(&c->instr.addr));
    break;
  }

  case 3: {
    c->instr.addr_fetched = true;
    c->instr.step = 0;
    break;
  }
  }
}

static inline void fetch_abx(CPU_6502 *c, bool force_page_cross) {
  __fetch_ab_n(c, &c->reg.X, force_page_cross);
}

static inline void fetch_aby(CPU_6502 *c, bool force_page_cross) {
  __fetch_ab_n(c, &c->reg.Y, force_page_cross);
}

static void fetch_idx(CPU_6502 *c) {
  switch (c->instr.step) {
  case 0: {
    schedule_read(c, c->reg.PC);
    break;
  }

  case 1: {
    c->instr.ptr = (read_bus(c) + c->reg.X) & 0xFF;
    c->reg.PC++;
    schedule_read(c, c->instr.ptr);
    break;
  }

  case 2: {
    set_lsb(&c->instr.addr, read_bus(c));
    c->instr.ptr = (c->instr.ptr + 1) & 0xFF;
    schedule_read(c, c->instr.ptr);
    break;
  }

  case 3: {
    set_msb(&c->instr.addr, read_bus(c));
    c->instr.addr_fetched = true;
    c->instr.step = 0;
    break;
  }
  }
}

static void fetch_idy(CPU_6502 *c, bool force_page_cross) {
  switch (c->instr.step) {
  case 0: {
    schedule_read(c, c->reg.PC);
    break;
  }

  case 1: {
    c->instr.ptr = read_bus(c);
    c->reg.PC++;
    schedule_read(c, c->instr.ptr);
    break;
  }

  case 2: {
    set_lsb(&c->instr.addr, read_bus(c));
    c->instr.ptr = (c->instr.ptr + 1) & 0xFF;
    schedule_read(c, c->instr.ptr);
    break;
  }

  case 3: {
    set_msb(&c->instr.addr, read_bus(c));

    uword base = c->instr.addr;
    c->instr.addr += c->reg.Y;

    if (!force_page_cross && (base & 0xFF00) == (c->instr.addr & 0xFF00)) {
      c->instr.addr_fetched = true;
      c->instr.step = 0;
      return;
    }

    schedule_read(c, (base & 0xFF00) | get_lsb(&c->instr.addr));
    break;
  }

  case 4: {
    c->instr.addr_fetched = true;
    c->instr.step = 0;
    break;
  }
  }
}

static void execute_ora(CPU_6502 *c) {
  switch (c->instr.step) {
  case 0: {
    schedule_read(c, c->instr.addr);
    break;
  }

  case 1: {
    c->reg.A |= read_bus(c);
    set_flag_zn(c, c->reg.A);

    c->instr.step = -1;
    schedule_read(c, c->reg.PC);
    break;
  }
  }
}

static void execute_kil_jam(CPU_6502 *c) {
  switch (c->instr.step) {
  case 0: {
    schedule_read(c, c->reg.PC);
    break;
  }

  case 1: {
    c->jammed = true;
    schedule_read(c, c->reg.PC);
    break;
  }
  }
}

static void __execute_ld_n(CPU_6502 *c, ubyte *b) {
  switch (c->instr.step) {
  case 0: {
    schedule_read(c, c->reg.PC);
    break;
  }

  case 1: {
    *b = read_bus(c);
    set_flag_zn(c, *b);

    c->instr.step = -1;
    schedule_read(c, c->reg.PC);
    break;
  }
  }
}

static inline void execute_lda(CPU_6502 *c) { __execute_ld_n(c, &c->reg.A); }
static inline void execute_ldx(CPU_6502 *c) { __execute_ld_n(c, &c->reg.X); }
static inline void execute_ldy(CPU_6502 *c) { __execute_ld_n(c, &c->reg.Y); }

static void __execute_st_n(CPU_6502 *c, ubyte *b) {
  switch (c->instr.step) {
  case 0: {
    schedule_write(c, c->reg.PC, *b);
    break;
  }

  case 1: {
    c->instr.step = -1;
    schedule_read(c, c->reg.PC);
    break;
  }
  }
}

static inline void execute_sta(CPU_6502 *c) { __execute_st_n(c, &c->reg.A); }
static inline void execute_stx(CPU_6502 *c) { __execute_st_n(c, &c->reg.X); }
static inline void execute_sty(CPU_6502 *c) { __execute_st_n(c, &c->reg.Y); }

static void execute_nop(CPU_6502 *c) {
  switch (c->instr.step) {
  case 0: {
    schedule_read(c, c->instr.addr);
    break;
  }

  case 1: {
    c->instr.step = -1;
    schedule_read(c, c->reg.PC);
    break;
  }
  }
}

/** ::TODO::
 * IMPLEMENTED: 80
 * REMAINING:   176
 */
static void do_opcode_cycle(CPU_6502 *c) {
  switch (c->instr.opcode) {
  case 0x02:
  case 0x12:
  case 0x22:
  case 0x32:
  case 0x42:
  case 0x52:
  case 0x62:
  case 0x72:
  case 0x92:
  case 0xB2:
  case 0xD2:
  case 0xF2: { // KIL/JAM
    execute_kil_jam(c);
    break;
  }

  case 0x00: { // BRK $IMP
    c->interrupt.breakStarted = true;
    c->interrupt.step = 1;
    c->instr.step = -1;
    schedule_read(c, c->reg.PC);
    break;
  }

  case 0x01: { // ORA $IDX
    if (!c->instr.addr_fetched) {
      fetch_idx(c);
    }

    if (c->instr.addr_fetched) {
      execute_ora(c);
    }

    break;
  }

  case 0x05: { // ORA $ZPG
    if (!c->instr.addr_fetched) {
      fetch_zpg(c);
    }

    if (c->instr.addr_fetched) {
      execute_ora(c);
    }

    break;
  }

  case 0x09: { // ORA $IMM
    if (!c->instr.addr_fetched) {
      fetch_imm(c);
    }

    if (c->instr.addr_fetched) {
      execute_ora(c);
    }

    break;
  }

  case 0x0D: { // ORA $ABS
    if (!c->instr.addr_fetched) {
      fetch_abs(c);
    }

    if (c->instr.addr_fetched) {
      execute_ora(c);
    }

    break;
  }

  case 0x11: { // ORA $IDY
    if (!c->instr.addr_fetched) {
      fetch_idy(c, false);
    }

    if (c->instr.addr_fetched) {
      execute_ora(c);
    }

    break;
  }

  case 0x15: { // ORA $ZPX
    if (!c->instr.addr_fetched) {
      fetch_zpx(c);
    }

    if (c->instr.addr_fetched) {
      execute_ora(c);
    }

    break;
  }

  case 0x19: { // ORA $ABY
    if (!c->instr.addr_fetched) {
      fetch_aby(c, false);
    }

    if (c->instr.addr_fetched) {
      execute_ora(c);
    }

    break;
  }

  case 0x1D: { // ORA $ABX
    if (!c->instr.addr_fetched) {
      fetch_abx(c, false);
    }

    if (c->instr.addr_fetched) {
      execute_ora(c);
    }

    break;
  }

  case 0xA1: { // LDA $IDX
    if (!c->instr.addr_fetched) {
      fetch_idx(c);
    }

    if (c->instr.addr_fetched) {
      execute_lda(c);
    }

    break;
  }

  case 0xA5: { // LDA $ZPG
    if (!c->instr.addr_fetched) {
      fetch_zpg(c);
    }

    if (c->instr.addr_fetched) {
      execute_lda(c);
    }

    break;
  }

  case 0xA9: { // LDA $IMM
    if (!c->instr.addr_fetched) {
      fetch_imm(c);
    }

    if (c->instr.addr_fetched) {
      execute_lda(c);
    }

    break;
  }

  case 0xAD: { // LDA $ABS
    if (!c->instr.addr_fetched) {
      fetch_abs(c);
    }

    if (c->instr.addr_fetched) {
      execute_lda(c);
    }

    break;
  }

  case 0xB1: { // LDA $IDY
    if (!c->instr.addr_fetched) {
      fetch_idy(c, false);
    }

    if (c->instr.addr_fetched) {
      execute_lda(c);
    }

    break;
  }

  case 0xB5: { // LDA $ZPX
    if (!c->instr.addr_fetched) {
      fetch_zpx(c);
    }

    if (c->instr.addr_fetched) {
      execute_lda(c);
    }

    break;
  }

  case 0xB9: { // LDA $ABY
    if (!c->instr.addr_fetched) {
      fetch_aby(c, false);
    }

    if (c->instr.addr_fetched) {
      execute_lda(c);
    }

    break;
  }

  case 0xBD: { // LDA $ABX
    if (!c->instr.addr_fetched) {
      fetch_abx(c, false);
    }

    if (c->instr.addr_fetched) {
      execute_lda(c);
    }

    break;
  }

  case 0xA2: { // LDX $IMM
    if (!c->instr.addr_fetched) {
      fetch_imm(c);
    }

    if (c->instr.addr_fetched) {
      execute_ldx(c);
    }

    break;
  }

  case 0xA6: { // LDX $ZPG
    if (!c->instr.addr_fetched) {
      fetch_zpg(c);
    }

    if (c->instr.addr_fetched) {
      execute_ldx(c);
    }

    break;
  }

  case 0xAE: { // LDX $ABS
    if (!c->instr.addr_fetched) {
      fetch_abs(c);
    }

    if (c->instr.addr_fetched) {
      execute_ldx(c);
    }

    break;
  }

  case 0xB6: { // LDX $ZPY
    if (!c->instr.addr_fetched) {
      fetch_zpy(c);
    }

    if (c->instr.addr_fetched) {
      execute_ldx(c);
    }

    break;
  }

  case 0xBE: { // LDX $ABY
    if (!c->instr.addr_fetched) {
      fetch_aby(c, false);
    }

    if (c->instr.addr_fetched) {
      execute_ldx(c);
    }

    break;
  }

  case 0xA0: { // LDY $IMM
    if (!c->instr.addr_fetched) {
      fetch_imm(c);
    }

    if (c->instr.addr_fetched) {
      execute_ldy(c);
    }

    break;
  }

  case 0xA4: { // LDY $ZPG
    if (!c->instr.addr_fetched) {
      fetch_zpg(c);
    }

    if (c->instr.addr_fetched) {
      execute_ldy(c);
    }

    break;
  }

  case 0xAC: { // LDY $ABS
    if (!c->instr.addr_fetched) {
      fetch_abs(c);
    }

    if (c->instr.addr_fetched) {
      execute_ldy(c);
    }

    break;
  }

  case 0xB4: { // LDY $ZPX
    if (!c->instr.addr_fetched) {
      fetch_zpx(c);
    }

    if (c->instr.addr_fetched) {
      execute_ldy(c);
    }

    break;
  }

  case 0xBC: { // LDY $ABX
    if (!c->instr.addr_fetched) {
      fetch_abx(c, false);
    }

    if (c->instr.addr_fetched) {
      execute_ldy(c);
    }

    break;
  }

  case 0x81: { // STA $IDX
    if (!c->instr.addr_fetched) {
      fetch_idx(c);
    }

    if (c->instr.addr_fetched) {
      execute_sta(c);
    }

    break;
  }

  case 0x85: { // STA $ZPG
    if (!c->instr.addr_fetched) {
      fetch_zpg(c);
    }

    if (c->instr.addr_fetched) {
      execute_sta(c);
    }

    break;
  }

  case 0x8D: { // STA $ABS
    if (!c->instr.addr_fetched) {
      fetch_abs(c);
    }

    if (c->instr.addr_fetched) {
      execute_sta(c);
    }

    break;
  }

  case 0x91: { // STA $IDY
    if (!c->instr.addr_fetched) {
      fetch_idy(c, true);
    }

    if (c->instr.addr_fetched) {
      execute_sta(c);
    }

    break;
  }

  case 0x95: { // STA $ZPX
    if (!c->instr.addr_fetched) {
      fetch_zpx(c);
    }

    if (c->instr.addr_fetched) {
      execute_sta(c);
    }

    break;
  }

  case 0x99: { // STA $ABY
    if (!c->instr.addr_fetched) {
      fetch_aby(c, true);
    }

    if (c->instr.addr_fetched) {
      execute_sta(c);
    }

    break;
  }

  case 0x9D: { // STA $ABX
    if (!c->instr.addr_fetched) {
      fetch_abx(c, true);
    }

    if (c->instr.addr_fetched) {
      execute_sta(c);
    }

    break;
  }

  case 0x86: { // STX $ZPG
    if (!c->instr.addr_fetched) {
      fetch_zpg(c);
    }

    if (c->instr.addr_fetched) {
      execute_stx(c);
    }

    break;
  }

  case 0x8E: { // STX $ABS
    if (!c->instr.addr_fetched) {
      fetch_abs(c);
    }

    if (c->instr.addr_fetched) {
      execute_stx(c);
    }

    break;
  }

  case 0x96: { // STX $ZPY
    if (!c->instr.addr_fetched) {
      fetch_zpy(c);
    }

    if (c->instr.addr_fetched) {
      execute_stx(c);
    }

    break;
  }

  case 0x84: { // STY $ZPG
    if (!c->instr.addr_fetched) {
      fetch_zpg(c);
    }

    if (c->instr.addr_fetched) {
      execute_sty(c);
    }

    break;
  }

  case 0x8C: { // STY $ABS
    if (!c->instr.addr_fetched) {
      fetch_abs(c);
    }

    if (c->instr.addr_fetched) {
      execute_sty(c);
    }

    break;
  }

  case 0x94: { // STY $ZPX
    if (!c->instr.addr_fetched) {
      fetch_zpx(c);
    }

    if (c->instr.addr_fetched) {
      execute_sty(c);
    }

    break;
  }

  case 0x1A:
  case 0x3A:
  case 0x5A:
  case 0x7A:
  case 0xDA:
  case 0xEA:
  case 0xFA: { // NOP $IMP
    if (c->instr.step == 0) {
      c->instr.addr = c->reg.PC;
    }

    execute_nop(c);
    break;
  }

  case 0x80:
  case 0x82:
  case 0x89:
  case 0xC2:
  case 0xE2: { // NOP $IMM
    if (!c->instr.addr_fetched) {
      fetch_imm(c);
    }

    execute_nop(c);
    break;
  }

  case 0x04:
  case 0x44:
  case 0x64: { // NOP $ZPG
    if (!c->instr.addr_fetched) {
      fetch_zpg(c);
    }

    if (c->instr.addr_fetched) {
      execute_nop(c);
    }

    break;
  }

  case 0x14:
  case 0x34:
  case 0x54:
  case 0x74:
  case 0xD4:
  case 0xF4: { // NOP $ZPX
    if (!c->instr.addr_fetched) {
      fetch_zpx(c);
    }

    if (c->instr.addr_fetched) {
      execute_nop(c);
    }

    break;
  }

  case 0x0C: { // NOP $ABS
    if (!c->instr.addr_fetched) {
      fetch_abs(c);
    }

    if (c->instr.addr_fetched) {
      execute_nop(c);
    }

    break;
  }

  case 0x1C:
  case 0x3C:
  case 0x5C:
  case 0x7C:
  case 0xDC:
  case 0xFC: { // NOP $ABX
    if (!c->instr.addr_fetched) {
      fetch_abx(c, false);
    }

    if (c->instr.addr_fetched) {
      execute_nop(c);
    }

    break;
  }

  default: {
    switch (c->instr.step) {
    case 0: {
      schedule_read(c, c->reg.PC);
      break;
    }

    case 1: {
      c->instr.step = -1;
      schedule_read(c, c->reg.PC);
      break;
    }
    }

    break;
  }
  }
}

void clock_cpu_6502(CPU_6502 *c) {
  if (!c) {
    return;
  }

  c->steps++;

  if (c->reset.pending) {
    do_reset_cycle(c);
    c->reset.step++;
    return;
  }

  if (c->jammed) {
    schedule_read(c, c->reg.PC);
    return;
  }

  if (c->oamdma.active) {
    do_oamdma_cycle(c);
    return;
  }

  if (interrupt(c)) {
    do_interrupt_cycle(c);
    c->interrupt.step++;
    return;
  }

  if (c->instr.step == 0) {
    c->instr.opcode = read_bus(c);
    c->reg.PC++;
  }

  do_opcode_cycle(c);
  c->instr.step++;
}
