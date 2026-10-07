#include "nes-6502.h"

static void set_lsb(uword *w, ubyte v) { *w = (*w & 0xFF00) | (v & 0xFF); }
static void set_msb(uword *w, ubyte v) { *w = (v << 8) | (*w & 0xFF); }

static ubyte get_lsb(uword *w) { return *w & 0xFF; }
static ubyte get_msb(uword *w) { return *w >> 8; }

void schedule_read(CPU_6502 *c, uword addr) {
  c->bus.addr = (uword)addr;
  c->bus.write = false;
}

void schedule_write(CPU_6502 *c, uword addr, ubyte val) {
  c->bus.addr = (uword)addr;
  c->bus.val = (ubyte)val;
  c->bus.write = true;
}

ubyte read_bus(CPU_6502 *c) { return c->bus.val; }
void schedule_pull(CPU_6502 *c) { schedule_read(c, 0x100 | (++c->reg.SP)); }
void schedule_push(CPU_6502 *c, ubyte val) {
  schedule_write(c, 0x100 | (c->reg.SP--), val);
}

ubyte get_pcl(CPU_6502 *c) { return get_lsb(&c->reg.PC); }
ubyte get_pch(CPU_6502 *c) { return get_msb(&c->reg.PC); }

void set_pcl(CPU_6502 *c, ubyte val) { set_lsb(&c->reg.PC, val); }
void set_pch(CPU_6502 *c, ubyte val) { set_msb(&c->reg.PC, val); }

void set_flag(CPU_6502 *c, ubyte f) { c->reg.P |= f; }
void clear_flag(CPU_6502 *c, ubyte f) { c->reg.P &= ~f; }
bool get_flag(CPU_6502 *c, ubyte f) { return (c->reg.P & f) != 0; }

bool interrupt(CPU_6502 *c) {
  bool v = c->interrupt.step > 0;
  v |= c->interrupt.breakStarted;
  v |= c->interrupt.nmiPending;
  v |= (c->interrupt.irqLine && !get_flag(c, flag_i));
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

    c->instr.addr = 0;
    c->instr.addr_fetched = false;
    c->instr.opcode = 0;
    c->instr.step = 0;

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

static void do_opcode_cycle(CPU_6502 *c) {
  switch (c->instr.opcode) {
  case 0x00: {
    c->interrupt.breakStarted = true;
    c->interrupt.step = 1;
    c->instr.step = -1;
    schedule_read(c, c->reg.PC);
    return;
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

  /** TODO:
   *  Needs implementation.
   *  To handle OAM-DMA and its timing.
   */

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
