// clang-format off
/**
 * OFFICIAL OPCODES
 *
 * ADC: 0x69(imm) 0x65(zpg) 0x75(zpx) 0x6D(abs) 0x7D(abx) 0x79(aby) 0x61(idx) 0x71(idy)
 * AND: 0x29(imm) 0x25(zpg) 0x35(zpx) 0x2D(abs) 0x3D(abx) 0x39(aby) 0x21(idx) 0x31(idy)
 * ASL: 0x0A(acc) 0x06(zpg) 0x16(zpx) 0x0E(abs) 0x1E(abx)                               ::IMPLEMENTED::
 * BCC: 0x90(rel)                                                                       ::IMPLEMENTED::
 * BCS: 0xB0(rel)                                                                       ::IMPLEMENTED::
 * BEQ: 0xF0(rel)                                                                       ::IMPLEMENTED::
 * BIT: 0x24(zpg) 0x2C(abs)
 * BMI: 0x30(rel)                                                                       ::IMPLEMENTED::
 * BNE: 0xD0(rel)                                                                       ::IMPLEMENTED::
 * BPL: 0x10(rel)                                                                       ::IMPLEMENTED::
 * BRK: 0x00(imp)
 * BVC: 0x50(rel)                                                                       ::IMPLEMENTED::
 * BVS: 0x70(rel)                                                                       ::IMPLEMENTED::
 * CLC: 0x18(imp)
 * CLD: 0xD8(imp)
 * CLI: 0x58(imp)
 * CLV: 0xB8(imp)
 * CMP: 0xC9(imm) 0xC5(zpg) 0xD5(zpx) 0xCD(abs) 0xDD(abx) 0xD9(aby) 0xC1(idx) 0xD1(idy)
 * CPX: 0xE0(imm) 0xE4(zpg) 0xEC(abs)
 * CPY: 0xC0(imm) 0xC4(zpg) 0xCC(abs)
 * DEC: 0xC6(zpg) 0xD6(zpx) 0xCE(abs) 0xDE(abx)
 * DEX: 0xCA(imp)
 * DEY: 0x88(imp)
 * EOR: 0x49(imm) 0x45(zpg) 0x55(zpx) 0x4D(abs) 0x5D(abx) 0x59(aby) 0x41(idx) 0x51(idy)
 * INC: 0xE6(zpg) 0xF6(zpx) 0xEE(abs) 0xFE(abx)
 * INX: 0xE8(imp)
 * INY: 0xC8(imp)
 * JMP: 0x4C(abs) 0x6C(ind)                                                             ::IMPLEMENTED::
 * JSR: 0x20(abs)                                                                       ::IMPLEMENTED::
 * LDA: 0xA9(imm) 0xA5(zpg) 0xB5(zpx) 0xAD(abs) 0xBD(abx) 0xB9(aby) 0xA1(idx) 0xB1(idy) ::IMPLEMENTED::
 * LDX: 0xA2(imm) 0xA6(zpg) 0xB6(zpy) 0xAE(abs) 0xBE(aby)                               ::IMPLEMENTED::
 * LDY: 0xA0(imm) 0xA4(zpg) 0xB4(zpx) 0xAC(abs) 0xBC(abx)                               ::IMPLEMENTED::
 * LSR: 0x4A(acc) 0x46(zpg) 0x56(zpx) 0x4E(abs) 0x5E(abx)                               ::IMPLEMENTED::
 * NOP: 0xEA(imp)                                                                       ::IMPLEMENTED::
 * ORA: 0x09(imm) 0x05(zpg) 0x15(zpx) 0x0D(abs) 0x1D(abx) 0x19(aby) 0x01(idx) 0x11(idy) ::IMPLEMENTED::
 * PHA: 0x48(imp)
 * PHP: 0x08(imp)
 * PLA: 0x68(imp)
 * PLP: 0x28(imp)
 * ROL: 0x2A(acc) 0x26(zpg) 0x36(zpx) 0x2E(abs) 0x3E(abx)                               ::IMPLEMENTED::
 * ROR: 0x6A(acc) 0x66(zpg) 0x76(zpx) 0x6E(abs) 0x7E(abx)                               ::IMPLEMENTED::
 * RTI: 0x40(imp)
 * RTS: 0x60(imp)
 * SBC: 0xE9(imm) 0xE5(zpg) 0xF5(zpx) 0xED(abs) 0xFD(abx) 0xF9(aby) 0xE1(idx) 0xF1(idy)
 * SEC: 0x38(imp)
 * SED: 0xF8(imp)
 * SEI: 0x78(imp)
 * STA: 0x85(zpg) 0x95(zpx) 0x8D(abs) 0x9D(abx) 0x99(aby) 0x81(idx) 0x91(idy)           ::IMPLEMENTED::
 * STX: 0x86(zpg) 0x96(zpy) 0x8E(abs)                                                   ::IMPLEMENTED::
 * STY: 0x84(zpg) 0x94(zpx) 0x8C(abs)                                                   ::IMPLEMENTED::
 * TAX: 0xAA(imp)
 * TAY: 0xA8(imp)
 * TSX: 0xBA(imp)
 * TXA: 0x8A(imp)
 * TXS: 0x9A(imp)
 * TYA: 0x98(imp)
 *
 * UNOFFICIAL OPCODES
 *
 * AHX: 0x93(idy) 0x9F(aby)
 * ALR: 0x4B(imm)
 * ANC: 0x0B(imm) 0x2B(imm)
 * ARR: 0x6B(imm)
 * AXS: 0xCB(imm)
 * DCP: 0xC3(idx) 0xC7(zpg) 0xCF(abs) 0xD3(idy) 0xD7(zpx) 0xDB(aby) 0xDF(abx)
 * ISC: 0xE3(idx) 0xE7(zpg) 0xEF(abs) 0xF3(idy) 0xF7(zpx) 0xFB(aby) 0xFF(abx)
 *
 * KIL: 0x02(imp) 0x12(imp) 0x22(imp) 0x32(imp) 0x42(imp) 0x52(imp)                     ::IMPLEMENTED::
 * * *  0x62(imp) 0x72(imp) 0x92(imp) 0xB2(imp) 0xD2(imp) 0xF2(imp)                     ::IMPLEMENTED::
 *
 * LAS: 0xBB(aby)
 * LAX: 0xA3(idx) 0xA7(zpg) 0xAF(abs) 0xB3(idy) 0xB7(zpy) 0xBF(aby)
 * RLA: 0x23(idx) 0x27(zpg) 0x2F(abs) 0x33(idy) 0x37(zpx) 0x3B(aby) 0x3F(abx)
 * RRA: 0x63(idx) 0x67(zpg) 0x6F(abs) 0x73(idy) 0x77(zpx) 0x7B(aby) 0x7F(abx)
 * SAX: 0x83(idx) 0x87(zpg) 0x8F(abs) 0x97(zpy)
 * SBC: 0xEB(imm)
 * SHX: 0x9E(aby)
 * SHY: 0x9C(abx)
 * SLO: 0x03(idx) 0x07(zpg) 0x0F(abs) 0x13(idy) 0x17(zpx) 0x1B(aby) 0x1F(abx)
 * SRE: 0x43(idx) 0x47(zpg) 0x4F(abs) 0x53(idy) 0x57(zpx) 0x5B(aby) 0x5F(abx)
 * TAS: 0x9B(aby)
 * XAA: 0x8B(imm)
 *
 * UNOFFICIAL NOP OPCODES
 *
 * NOP: 0x1A(imp) 0x3A(imp) 0x5A(imp) 0x7A(imp) 0xDA(imp) 0xFA(imp)                     ::IMPLEMENTED::
 * NOP: 0x80(imm) 0x82(imm) 0x89(imm) 0xC2(imm) 0xE2(imm)                               ::IMPLEMENTED::
 * NOP: 0x04(zpg) 0x44(zpg) 0x64(zpg)                                                   ::IMPLEMENTED::
 * NOP: 0x14(zpx) 0x34(zpx) 0x54(zpx) 0x74(zpx) 0xD4(zpx) 0xF4(zpx)                     ::IMPLEMENTED::
 * NOP: 0x0C(abs)                                                                       ::IMPLEMENTED::
 * NOP: 0x1C(abx) 0x3C(abx) 0x5C(abx) 0x7C(abx) 0xDC(abx) 0xFC(abx)                     ::IMPLEMENTED::
 *
 * ADDRESSING MODES
 *
 * acc: Accumulator
 * abs: Absolute
 * abx: Absolute, indexed by X
 * aby: Absolute, indexed by Y
 * imm: Immediate
 * imp: Implied
 * ind: Indirect
 * idx: Indexed-indirect
 * idy: Indirect-indexed
 * rel: Relative
 * zpg: Zero page
 * zpx: Zero page, indexed by X
 * zpy: Zero page, indexed by Y
 */
// clang-format on

#include "nes-6502.h"

static inline void set_lsb(uword *w, ubyte v) { *w = (*w & 0xFF00) | v; }
static inline void set_msb(uword *w, ubyte v) { *w = (v << 8) | (*w & 0xFF); }
static inline ubyte get_lsb(const uword w) { return w & 0xFF; }
static inline ubyte get_msb(const uword w) { return w >> 8; }

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

static inline ubyte get_pcl(const CPU_6502 *c) { return get_lsb(c->reg.PC); }
static inline ubyte get_pch(const CPU_6502 *c) { return get_msb(c->reg.PC); }
static inline void set_pcl(CPU_6502 *c, ubyte val) { set_lsb(&c->reg.PC, val); }
static inline void set_pch(CPU_6502 *c, ubyte val) { set_msb(&c->reg.PC, val); }

static inline void set_flag(CPU_6502 *c, ubyte f, bool w) {
  c->reg.P = w ? c->reg.P | f : c->reg.P & ~f;
}

static inline bool get_flag(const CPU_6502 *c, ubyte f) {
  return (c->reg.P & f) != 0;
}

static inline void set_flag_zn(CPU_6502 *c, ubyte v) {
  set_flag(c, flag_z, v == 0);
  set_flag(c, flag_n, v & 0x80);
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
    c->interrupt.step = 0;
    c->interrupt.irqLine = false;
    c->interrupt.nmiPending = false;
    c->interrupt.breakStarted = false;

    c->instr.addr = 0;
    c->instr.ptr = 0;
    c->instr.addr_fetched = false;
    c->instr.opcode = 0;
    c->instr.step = 0;
    c->instr.finished = false;

    c->oamdma.active = false;
    c->oamdma.step = 0;
    c->oamdma.addr = 0;

    c->jammed = false;
    schedule_read(c, c->reg.PC);
    break;
  }

  case 1: {
    set_flag(c, flag_u, true);
    set_flag(c, flag_i, true);

    schedule_read(c, 0x100 | c->reg.SP);
    break;
  }

  case 2: {
    c->reg.SP--;
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
    schedule_read(c, 0xFFFC);
    break;
  }

  case 5: {
    set_pcl(c, read_bus(c));
    schedule_read(c, 0xFFFD);
    break;
  }

  case 6: {
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
    if (c->interrupt.breakStarted) {
      c->reg.PC++;
    }

    break;
  }

  case 1: {
    schedule_push(c, get_pch(c));
    break;
  }

  case 2: {
    schedule_push(c, get_pcl(c));
    break;
  }

  case 3: {
    if (c->interrupt.breakStarted) {
      schedule_push(c, c->reg.P | flag_b);
    } else {
      schedule_push(c, c->reg.P & ~flag_b);
    }

    break;
  }

  case 4: {
    c->interrupt.breakStarted = false;
    set_flag(c, flag_i, true);

    if (c->interrupt.nmiPending) {
      c->interrupt.nmiPending = false;
      schedule_read(c, 0xFFFA);
    } else {
      schedule_read(c, 0xFFFE);
    }

    break;
  }

  case 5: {
    set_pcl(c, read_bus(c));
    schedule_read(c, c->bus.addr + 1);
    break;
  }

  case 6: {
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

    if (!force_page_cross && get_msb(c->instr.addr) == get_msb(c->instr.ptr)) {
      c->instr.addr_fetched = true;
      c->instr.step = 0;
      return;
    }

    schedule_read(c, (c->instr.ptr & 0xFF00) | get_lsb(c->instr.addr));
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

    if (!force_page_cross && get_msb(base) == get_msb(c->instr.addr)) {
      c->instr.addr_fetched = true;
      c->instr.step = 0;
      return;
    }

    schedule_read(c, (base & 0xFF00) | get_lsb(c->instr.addr));
    break;
  }

  case 4: {
    c->instr.addr_fetched = true;
    c->instr.step = 0;
    break;
  }
  }
}

static inline void end_opcode_execution(CPU_6502 *c) {
  c->instr.step = -1;
  c->instr.finished = true;
  schedule_read(c, c->reg.PC);
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

    end_opcode_execution(c);
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
    schedule_read(c, c->instr.addr);
    break;
  }

  case 1: {
    *b = read_bus(c);
    set_flag_zn(c, *b);

    end_opcode_execution(c);
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
    schedule_write(c, c->instr.addr, *b);
    break;
  }

  case 1: {
    end_opcode_execution(c);
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
    end_opcode_execution(c);
    break;
  }
  }
}

static inline bool is_branch_taken(const CPU_6502 *c) {
  switch (c->instr.opcode) {
  case 0x10:
    return !get_flag(c, flag_n);
  case 0x30:
    return get_flag(c, flag_n);
  case 0x50:
    return !get_flag(c, flag_v);
  case 0x70:
    return get_flag(c, flag_v);
  case 0x90:
    return !get_flag(c, flag_c);
  case 0xB0:
    return get_flag(c, flag_c);
  case 0xD0:
    return !get_flag(c, flag_z);
  case 0xF0:
    return get_flag(c, flag_z);
  }

  return false;
}

static void handle_branch(CPU_6502 *c) {
  switch (c->instr.step) {
  case 0: {
    schedule_read(c, c->reg.PC);
    break;
  }

  case 1: {
    byte operand = (byte)read_bus(c);
    c->reg.PC++;

    if (!is_branch_taken(c)) {
      end_opcode_execution(c);
      return;
    }

    c->instr.addr = c->reg.PC + operand;
    schedule_read(c, c->reg.PC);
    break;
  }

  case 2: {
    uword base = c->reg.PC;
    c->reg.PC = c->instr.addr;

    if (get_msb(base) == get_msb(c->reg.PC)) {
      end_opcode_execution(c);
      return;
    }

    set_msb(&c->instr.addr, get_msb(base));
    schedule_read(c, c->instr.addr);
    break;
  }

  case 3: {
    end_opcode_execution(c);
    break;
  }
  }
}

static void execute_jmp(CPU_6502 *c) {
  c->reg.PC = c->instr.addr;
  end_opcode_execution(c);
}

static void execute_asl(CPU_6502 *c) {
  switch (c->instr.step) {
  case 0: {
    schedule_read(c, c->instr.addr);
    break;
  }

  case 1: {
    c->instr.ptr = read_bus(c);
    schedule_write(c, c->instr.addr, (ubyte)c->instr.ptr);
    break;
  }

  case 2: {
    ubyte val = ((ubyte)c->instr.ptr << 1) & 0xFF;
    set_flag(c, flag_c, (ubyte)c->instr.ptr & 0x80);
    set_flag_zn(c, val);
    schedule_write(c, c->instr.addr, val);
    break;
  }

  case 3: {
    end_opcode_execution(c);
    break;
  }
  }
}

static void execute_lsr(CPU_6502 *c) {
  switch (c->instr.step) {
  case 0: {
    schedule_read(c, c->instr.addr);
    break;
  }

  case 1: {
    c->instr.ptr = read_bus(c);
    schedule_write(c, c->instr.addr, (ubyte)c->instr.ptr);
    break;
  }

  case 2: {
    ubyte val = ((ubyte)c->instr.ptr >> 1) & 0xFF;
    set_flag(c, flag_c, (ubyte)c->instr.ptr & 1);
    set_flag_zn(c, val);
    schedule_write(c, c->instr.addr, val);
    break;
  }

  case 3: {
    end_opcode_execution(c);
    break;
  }
  }
}

static void execute_rol(CPU_6502 *c) {
  switch (c->instr.step) {
  case 0: {
    schedule_read(c, c->instr.addr);
    break;
  }

  case 1: {
    c->instr.ptr = read_bus(c);
    schedule_write(c, c->instr.addr, (ubyte)c->instr.ptr);
    break;
  }

  case 2: {
    ubyte val = ((ubyte)(c->instr.ptr << 1) | get_flag(c, flag_c)) & 0xFF;
    set_flag(c, flag_c, (ubyte)c->instr.ptr & 0x80);
    set_flag_zn(c, val);
    schedule_write(c, c->instr.addr, val);
    break;
  }

  case 3: {
    end_opcode_execution(c);
    break;
  }
  }
}

static void execute_ror(CPU_6502 *c) {
  switch (c->instr.step) {
  case 0: {
    schedule_read(c, c->instr.addr);
    break;
  }

  case 1: {
    c->instr.ptr = read_bus(c);
    schedule_write(c, c->instr.addr, (ubyte)c->instr.ptr);
    break;
  }

  case 2: {
    bool carry = get_flag(c, flag_c);
    ubyte val = ((ubyte)(c->instr.ptr >> 1) | (carry << 7)) & 0xFF;
    set_flag(c, flag_c, (ubyte)c->instr.ptr & 1);
    set_flag_zn(c, val);
    schedule_write(c, c->instr.addr, val);
    break;
  }

  case 3: {
    end_opcode_execution(c);
    break;
  }
  }
}

/** ::TODO::
 * IMPLEMENTED: 111 (43.36%)
 * REMAINING:   145
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

  case 0x10:   // BPL $REL
  case 0x30:   // BMI $REL
  case 0x50:   // BVC $REL
  case 0x70:   // BVS $REL
  case 0x90:   // BCC $REL
  case 0xB0:   // BCS $REL
  case 0xD0:   // BNE $REL
  case 0xF0: { // BEQ $REL
    handle_branch(c);
    break;
  }

  case 0x00: { // BRK $IMP
    c->interrupt.breakStarted = true;
    end_opcode_execution(c);
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

    execute_ora(c);
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

    execute_lda(c);
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

    execute_ldx(c);
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

    execute_ldy(c);
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

  case 0x4C: { // JMP $ABS
    if (!c->instr.addr_fetched) {
      fetch_abs(c);
    }

    if (c->instr.addr_fetched) {
      execute_jmp(c);
    }

    break;
  }

  case 0x6C: { // JMP $IND
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
      schedule_read(c, c->instr.ptr);
      break;
    }

    case 3: {
      set_pcl(c, read_bus(c));
      set_lsb(&c->instr.ptr, get_lsb(c->instr.ptr) + 1);
      schedule_read(c, c->instr.ptr);
      break;
    }

    case 4: {
      set_pch(c, read_bus(c));
      end_opcode_execution(c);
      break;
    }
    }

    break;
  }

  case 0x20: { // JSR $ABS/$JSR
    switch (c->instr.step) {
    case 0: {
      schedule_read(c, c->reg.PC);
      break;
    }

    case 1: {
      set_lsb(&c->instr.addr, read_bus(c));
      c->reg.PC++;
      schedule_read(c, 0x0100 | c->reg.SP);
      break;
    }

    case 2: {
      read_bus(c);
      schedule_push(c, get_pch(c));
      break;
    }

    case 3: {
      schedule_push(c, get_pcl(c));
      break;
    }

    case 4: {
      schedule_read(c, c->reg.PC);
      break;
    }

    case 5: {
      set_msb(&c->instr.addr, read_bus(c));
      c->reg.PC = c->instr.addr;
      end_opcode_execution(c);
      break;
    }
    }

    break;
  }

  case 0x06: { // ASL $ZPG
    if (!c->instr.addr_fetched) {
      fetch_zpg(c);
    }

    if (c->instr.addr_fetched) {
      execute_asl(c);
    }

    break;
  }

  case 0x0A: { // ASL $ACC
    switch (c->instr.step) {
    case 0: {
      schedule_read(c, c->reg.PC);
      break;
    }

    case 1: {
      ubyte old = c->reg.A;
      c->reg.A = old << 1;

      set_flag(c, flag_c, old & 0x80);
      set_flag_zn(c, c->reg.A);
      end_opcode_execution(c);
      break;
    }
    }

    break;
  }

  case 0x0E: { // ASL $ABS
    if (!c->instr.addr_fetched) {
      fetch_abs(c);
    }

    if (c->instr.addr_fetched) {
      execute_asl(c);
    }

    break;
  }

  case 0x16: { // ASL $ZPX
    if (!c->instr.addr_fetched) {
      fetch_zpx(c);
    }

    if (c->instr.addr_fetched) {
      execute_asl(c);
    }

    break;
  }

  case 0x1E: { // ASL $ABX
    if (!c->instr.addr_fetched) {
      fetch_abx(c, true);
    }

    if (c->instr.addr_fetched) {
      execute_asl(c);
    }

    break;
  }

  case 0x46: { // LSR $ZPG
    if (!c->instr.addr_fetched) {
      fetch_zpg(c);
    }

    if (c->instr.addr_fetched) {
      execute_lsr(c);
    }

    break;
  }

  case 0x4A: { // LSR $ACC
    switch (c->instr.step) {
    case 0: {
      schedule_read(c, c->reg.PC);
      break;
    }

    case 1: {
      ubyte old = c->reg.A;
      c->reg.A = old >> 1;

      set_flag(c, flag_c, old & 1);
      set_flag_zn(c, c->reg.A);
      end_opcode_execution(c);
      break;
    }
    }

    break;
  }

  case 0x4E: { // LSR $ABS
    if (!c->instr.addr_fetched) {
      fetch_abs(c);
    }

    if (c->instr.addr_fetched) {
      execute_lsr(c);
    }

    break;
  }

  case 0x56: { // LSR $ZPX
    if (!c->instr.addr_fetched) {
      fetch_zpx(c);
    }

    if (c->instr.addr_fetched) {
      execute_lsr(c);
    }

    break;
  }

  case 0x5E: { // LSR $ABX
    if (!c->instr.addr_fetched) {
      fetch_abx(c, true);
    }

    if (c->instr.addr_fetched) {
      execute_lsr(c);
    }

    break;
  }

  case 0x26: { // ROL $ZPG
    if (!c->instr.addr_fetched) {
      fetch_zpg(c);
    }

    if (c->instr.addr_fetched) {
      execute_rol(c);
    }

    break;
  }

  case 0x2A: { // ROL $ACC
    switch (c->instr.step) {
    case 0: {
      schedule_read(c, c->reg.PC);
      break;
    }

    case 1: {
      ubyte old = c->reg.A;
      bool carry = get_flag(c, flag_c);
      c->reg.A = (old << 1) | carry;

      set_flag(c, flag_c, old & 0x80);
      set_flag_zn(c, c->reg.A);
      end_opcode_execution(c);
      break;
    }
    }

    break;
  }

  case 0x2E: { // ROL $ABS
    if (!c->instr.addr_fetched) {
      fetch_abs(c);
    }

    if (c->instr.addr_fetched) {
      execute_rol(c);
    }

    break;
  }

  case 0x36: { // ROL $ZPX
    if (!c->instr.addr_fetched) {
      fetch_zpx(c);
    }

    if (c->instr.addr_fetched) {
      execute_rol(c);
    }

    break;
  }

  case 0x3E: { // ROL $ABX
    if (!c->instr.addr_fetched) {
      fetch_abx(c, true);
    }

    if (c->instr.addr_fetched) {
      execute_rol(c);
    }

    break;
  }

  case 0x66: { // ROR $ZPG
    if (!c->instr.addr_fetched) {
      fetch_zpg(c);
    }

    if (c->instr.addr_fetched) {
      execute_ror(c);
    }

    break;
  }

  case 0x6A: { // ROR $ACC
    switch (c->instr.step) {
    case 0: {
      schedule_read(c, c->reg.PC);
      break;
    }

    case 1: {
      ubyte old = c->reg.A;
      bool carry = get_flag(c, flag_c);
      c->reg.A = (old >> 1) | (carry << 7);

      set_flag(c, flag_c, old & 1);
      set_flag_zn(c, c->reg.A);
      end_opcode_execution(c);
      break;
    }
    }

    break;
  }

  case 0x6E: { // ROR $ABS
    if (!c->instr.addr_fetched) {
      fetch_abs(c);
    }

    if (c->instr.addr_fetched) {
      execute_ror(c);
    }

    break;
  }

  case 0x76: { // ROR $ZPX
    if (!c->instr.addr_fetched) {
      fetch_zpx(c);
    }

    if (c->instr.addr_fetched) {
      execute_ror(c);
    }

    break;
  }

  case 0x7E: { // ROR $ABX
    if (!c->instr.addr_fetched) {
      fetch_abx(c, true);
    }

    if (c->instr.addr_fetched) {
      execute_ror(c);
    }

    break;
  }

    /** ::TODO::
     * ::IMPLEMENT:: ::HERE::
     */

  default: { // NOP-Like $IMM
    if (!c->instr.addr_fetched) {
      fetch_imm(c);
    }

    execute_nop(c);
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
    c->instr.finished = false;
    c->instr.addr_fetched = false;
    c->instr.opcode = read_bus(c);
    c->reg.PC++;
  }

  do_opcode_cycle(c);
  c->instr.step++;
}
