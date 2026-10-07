#include "nes-6502.h"

static inline void set_lsb(uword *w, ubyte v) {
  *w = (*w & 0xFF00) | (v & 0xFF);
}

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

static void fetch_idx(CPU_6502 *c) {
  switch (c->interrupt.step) {
  case 0: {
    schedule_read(c, c->reg.PC);
    break;
  }

  case 1: {
    c->instr.ptr = (read_bus(c) + c->reg.X) & 0xFF;
    schedule_read(c, c->instr.ptr);
    break;
  }

  case 2: {
    set_lsb(&c->instr.addr, read_bus(c));
    c->instr.ptr++;
    c->instr.ptr &= 0xFF;
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

static void do_opcode_cycle(CPU_6502 *c) {
  switch (c->instr.opcode) {
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

  case 0x02: { // KIL/JAM
    execute_kil_jam(c);
    break;
  }

    /** TODO:
     *  Needs implementation.
     *  To implement all other
     *  opcodes.
     */

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
