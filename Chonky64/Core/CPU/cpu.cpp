#pragma warning(disable : 4996)
#include "cpu.h"

cpu::cpu(memory* memptr) {
	Memory = memptr;
	Memory->cart = fopen("C:\\Users\\zacse\\Downloads\\dillon-n64-tests-simpleboot\\addiu_simpleboot.z64", "rb");
	simulate_pif_rom();
}

void cpu::step() {
	u32 instr = Memory->read<u32>(pc);

	switch (instr) {
	default:
		Helpers::panic("Unhandled instruction 0x%08x\n", instr);
	}
}

void cpu::simulate_pif_rom() {
	gprs[11] = 0xFFFFFFFFA4000040;
	gprs[20] = 0x0000000000000001;
	gprs[22] = 0x000000000000003F;
	gprs[29] = 0xFFFFFFFFA4001FF0;
	pc = 0xA4000040;

	fseek(Memory->cart, 0, SEEK_SET);
	fread(Memory->dmem, sizeof(u8), 0x1000, Memory->cart);
}