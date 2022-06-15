#include <iostream>
#include "n64.h"

int main() {
    n64 N64;
    u32 format = (N64.Memory.dmem[0] << 24) | (N64.Memory.dmem[1] << 16) | (N64.Memory.dmem[2] << 8) | (N64.Memory.dmem[3]);
    printf("0x%08x\n", format);
    for (;;) N64.Cpu.step();
}