#pragma warning(disable : 4996)
#include "cpu.h"

cpu::cpu(memory* memptr) {
	Memory = memptr;
	Memory->cart = fopen("C:\\Users\\zacse\\Downloads\\dillon-n64-tests-simpleboot\\addiu_simpleboot.z64", "rb");
	Memory->Pi->cart = fopen("C:\\Users\\zacse\\Downloads\\dillon-n64-tests-simpleboot\\addiu_simpleboot.z64", "rb");
	Memory->Pi->file_size = std::filesystem::file_size("C:\\Users\\zacse\\Downloads\\dillon-n64-tests-simpleboot\\addiu_simpleboot.z64");
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
	const u8 sa = ((instr >> 6) & 0x1f);
	instruction inst = { rs, rd, rt, imm, sa };

	printf("[CPU] pc 0x%08x: ", pc);

	if (branch_pc.has_value()) {
		pc = branch_pc.value();
		pc -= 4;
		branch_pc = std::nullopt;
	}

	switch ((instr >> 26) & 0x3f) {
	case instructions::SPECIAL: {
		switch (instr & 0x3f) {
		case instructions_special::SLL: sll(inst); break;
		case instructions_special::JR:  jr(inst); break;
		default:
			Helpers::panic("Unhandled special instruction 0x%02x\n", instr & 0x3f);
		}
		break;
	}
	case instructions::BNE:  bne(inst); break;
	case instructions::ANDI: andi(inst); break;
	case instructions::ORI:  ori(inst); break;
	case instructions::LUI:  lui(inst); break;
	case instructions::LW:   lw(inst); break;
	case instructions::SW:   sw(inst); break;
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

inline void cpu::branch(u32 offset, bool cond) {
	if (cond) branch_pc = pc + offset;
}
// Instructions
void cpu::sll(instruction instr) {
	Helpers::log("sll %s, %s, 0x%04x\n", gpr_names[instr.rd].c_str(), gpr_names[instr.rs].c_str(), instr.sa);
	s32 shifted = (s32)gprs[instr.rt] << instr.sa;
	gprs[instr.rd] = (s64)shifted;
}
void cpu::jr(instruction instr) {
	Helpers::log("jr %s\n", gpr_names[instr.rs].c_str());
	if ((gprs[instr.rs] & 3) != 0) {
		// Address exception
		printf("[CPU] UNIMPLEMENTED ADDRESS EXCEPTION\n");
		return;
	}
	branch_pc = gprs[instr.rs];
}

void cpu::bne(instruction instr) {
	s32 offset = (s32)(s16)(instr.imm << 2);
	Helpers::log("bne %s, %s, 0x%04x\n", gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str(), offset);
	branch(offset, gprs[instr.rs] != gprs[instr.rt]);
}
void cpu::andi(instruction instr) {
	Helpers::log("andi %s, %s, 0x%04x\n", gpr_names[instr.rt].c_str(), gpr_names[instr.rs].c_str(), instr.imm);
	gprs[instr.rt] = (gprs[instr.rs] & instr.imm);
}
void cpu::ori(instruction instr) {
	Helpers::log("ori %s, %s, 0x%04x\n", gpr_names[instr.rt].c_str(), gpr_names[instr.rs].c_str(), instr.imm);
	gprs[instr.rt] = (gprs[instr.rs] | instr.imm);
}
void cpu::lui(instruction instr) {
	Helpers::log("lui %s, 0x%04x\n", gpr_names[instr.rt].c_str(), instr.imm);
	s32 imm_shifted = instr.imm << 16;
	gprs[instr.rt] = (s64)(imm_shifted);
}
void cpu::lw(instruction instr) {
	s32 offset = (s32)(s16)instr.imm;
	u32 addr = gprs[instr.rs] + offset;
	Helpers::log("lw %s, 0x%04x(%s)  ;  %s <- [0x%08x]\n", gpr_names[instr.rt].c_str(), offset, gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str(), addr);
	gprs[instr.rt] = Memory->read<u32>(addr);
}
void cpu::sw(instruction instr) {
	s32 offset = (s32)(s16)instr.imm;
	u32 addr = gprs[instr.rs] + offset;
	Helpers::log("sw %s, 0x%04x(%s)  ;  [0x%08x] <- 0x%08x\n", gpr_names[instr.rt].c_str(), offset, gpr_names[instr.rs].c_str(), addr, gprs[instr.rt]);
	Memory->write<u32>(addr, gprs[instr.rt]);
}