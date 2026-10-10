# NES-6502

A small 6502 CPU emulator written in C for experimenting with NES-style memory mapping.

Features:

- Minimal 6502 core
- Bus-based memory access
- Interrupt/reset handling

Logging:

- iNES ROM loading
- CPU step logging

This project is intended for learning and debugging the 6502 instruction cycle.
It is not a complete NES emulator or a production-ready game console implementation.

## Typical usage

The library is designed to be embedded into a larger emulator or debugger. The public interface exposes a clock function to advance the CPU state:

- `CPU_6502 cpu = {0}; cpu.reset.pending = true;`
- `clock_cpu_6502(&cpu);`

This is intended to be used when there is bus access available.

```c
// After every clock, handle read or write.
while (1) {
    clock_cpu_6502(&cpu);

    if (cpu.bus.write) {
        uword addr = cpu.bus.addr;
        ubyte val = cpu.bus.val;

        // Handle write.
        // OAM DMA is enabled internally.
    } else {
        uword addr = cpu.bus.addr;

        // Handle read.
        ubyte val = read(addr); // Your own method.
        cpu.bus.val = val;

        // DON'T set val if read is not possible
        // to mimic open-bus behavior.
    }
}
```
