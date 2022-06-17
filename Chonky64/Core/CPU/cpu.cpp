#pragma warning(disable : 4996)
#include "cpu.h"

cpu::cpu(memory* memptr) {
	Memory = memptr;
	Memory->cart = fopen("C:\\Users\\zacse\\Downloads\\dillon-n64-tests-simpleboot\\addiu_simpleboot.z64", "rb");
	//Memory->cart = fopen("H:\\Games\\roms\\N64\\Super Mario 64 (USA).n64", "rb");
	//Memory->cart = fopen("H:\\Games\\roms\\N64\\Namco Museum 64 (USA).n64", "rb");
	simulate_pif_rom();
}

void cpu::step() {
	const u32 instr = Memory->read<u32>(pc);
	const u8 rs = (instr >> 21) & 0x1f;
	const u8 rd = (instr >> 11) & 0x1f;
	const u8 rt = (instr >> 16) & 0x1f;
	const u16 imm = instr & 0xffff;
	instruction inst = { rs, rd, rt, imm };

	switch ((instr >> 26) & 0x3f) {
	case instructions::LUI: lui(inst); break;
	case instructions::ORI: ori(inst); break;
	case instructions::SW:  sw(inst);  break;
	default:
		Helpers::panic("Unhandled instruction 0x%02x\n", (instr >> 26) & 0x3f);
	}

	pc += 4;
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

// Instructions
void cpu::ori(instruction instr) {
	Helpers::log("[CPU] ori %s, %s, 0x%04x\n", gpr_names[instr.rt].c_str(), gpr_names[instr.rs].c_str(), instr.imm);
	gprs[instr.rt] = (gprs[instr.rs] | instr.imm);
}
void cpu::lui(instruction instr) {
	Helpers::log("[CPU] lui %s, 0x%04x\n", gpr_names[instr.rt].c_str(), instr.imm);
	s32 imm_shifted = instr.imm << 16;
	gprs[instr.rt] = (s64)(imm_shifted);
}
void cpu::sw(instruction instr) {
	s32 offset = (s32)(s16)instr.imm;
	u32 addr = gprs[instr.rs] + offset;
	Helpers::log("[CPU] sw %s, 0x%04x(%s)  ;  [0x%08x] <- 0x%08x\n", gpr_names[instr.rt].c_str(), offset, gpr_names[instr.rs].c_str(), addr, gprs[instr.rt]);
	Memory->write(addr, gprs[instr.rt]);
}