#pragma warning(disable : 4996)
#include "cpu.h"

cpu::cpu(memory* memptr) {
	//const char* path = "C:\\Users\\zacse\\Downloads\\dillon-n64-tests-simpleboot\\addiu_simpleboot.z64";
	//const char* path = "C:\\Users\\zacse\\Downloads\\dillon-n64-tests-simpleboot\\basic_simpleboot.z64";
	const char* path = "C:\\Users\\zacse\\Downloads\\dillon-n64-tests-simpleboot\\sll_simpleboot.z64";
	const char* path = "H:\\Games\\roms\\N64\\Namco Museum 64 (USA).n64";
	Memory = memptr;
	Memory->cart = fopen(path, "rb");
	Memory->PI->cart = fopen(path, "rb");
	int size = std::filesystem::file_size(path);
	Memory->PI->file_size = pow(2, ceil(log(size) / log(2)));
	simulate_pif_rom();
}

void cpu::step() {
	gprs[0] = 0;
	cycles += 2;

	const u32 instr = Memory->read<u32>(pc);
	const u8 rs = (instr >> 21) & 0x1f;
	const u8 rd = (instr >> 11) & 0x1f;
	const u8 rt = (instr >> 16) & 0x1f;
	const u16 imm = instr & 0xffff;
	const u8 sa = ((instr >> 6) & 0x1f);
	instruction inst = { instr, rs, rd, rt, imm, sa };

	Helpers::log("[CPU] pc 0x%08x: ", pc);

	if (branch_pc.has_value()) {
		pc = branch_pc.value();
		pc -= 4;
		branch_pc = std::nullopt;
	}

	switch ((instr >> 26) & 0x3f) {
	case instructions::SPECIAL: {
		switch (instr & 0x3f) {
		case instructions_special::SLL:  sll(inst); break;
		case instructions_special::SRL:  srl(inst); break;
		case instructions_special::JR:   jr(inst); break;
		case instructions_special::JALR: jalr(inst); break;
		case instructions_special::ADD:  add(inst); break;
		case instructions_special::ADDU: addu(inst); break;
		case instructions_special::SLT:  slt(inst); break;
		default:
			Helpers::panic("Unhandled special instruction 0x%02x\n", instr & 0x3f);
		}
		break;
	}
	case instructions::J:     j(inst); break;
	case instructions::JAL:   jal(inst); break;
	case instructions::BEQ:   beq(inst); break;
	case instructions::BNE:   bne(inst); break;
	case instructions::BGTZ:  bgtz(inst); break;
	case instructions::ADDI:  addi(inst); break;
	case instructions::ADDIU: addiu(inst); break;
	case instructions::ANDI:  andi(inst); break;
	case instructions::ORI:   ori(inst); break;
	case instructions::LUI:   lui(inst); break;
	case instructions::LB:    lb(inst); break;
	case instructions::LH:    lh(inst); break;
	case instructions::LW:    lw(inst); break;
	case instructions::LBU:   lbu(inst); break;
	case instructions::LHU:   lhu(inst); break;
	case instructions::SH:    sh(inst); break;
	case instructions::SW:    sw(inst); break;
	case instructions::LD:    ld(inst); break;
	case instructions::SD:    sd(inst); break;
	
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
	if (cond) branch_pc = pc + offset + 4;
}
// Instructions
void cpu::sll(instruction instr) {
	Helpers::log("sll %s, %s, 0x%04x\n", gpr_names[instr.rd].c_str(), gpr_names[instr.rs].c_str(), instr.sa);
	s32 shifted = (s32)gprs[instr.rt] << instr.sa;
	gprs[instr.rd] = (s64)shifted;
}
void cpu::srl(instruction instr) {
	Helpers::log("srl %s, %s, 0x%04x\n", gpr_names[instr.rd].c_str(), gpr_names[instr.rs].c_str(), instr.sa);
	s32 shifted = (s32)gprs[instr.rt] >> instr.sa;
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
void cpu::jalr(instruction instr) {
	Helpers::log("jalr %s, %s\n", gpr_names[instr.rd].c_str(), gpr_names[instr.rs].c_str());
	if ((gprs[instr.rs] & 3) != 0) {
		// Address exception
		printf("[CPU] UNIMPLEMENTED ADDRESS EXCEPTION\n");
		return;
	}
	branch_pc = gprs[instr.rs];
	gprs[instr.rd] = pc + 8;
}
void cpu::add(instruction instr) {
	Helpers::log("add %s, %s, %s\n", gpr_names[instr.rd].c_str(), gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str());
	s32 a = (s32)gprs[instr.rs];
	s32 b = (s32)gprs[instr.rt];
	s32 res = a + b; // TODO: check for integer overflow
	gprs[instr.rd] = res;
}
void cpu::addu(instruction instr) {
	Helpers::log("addu %s, %s, %s\n", gpr_names[instr.rd].c_str(), gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str());
	s32 a = (s32)gprs[instr.rs];
	s32 b = (s32)gprs[instr.rt];
	s32 res = a + b;
	gprs[instr.rd] = res;
}
void cpu::slt(instruction instr) {
	Helpers::log("slt %s, %s, %s\n", gpr_names[instr.rd].c_str(), gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str());
	gprs[instr.rd] = (gprs[instr.rs] < gprs[instr.rt]) ? 1 : 0;
}

void cpu::j(instruction instr) {
	u32 target = instr.raw & 0x3ffffff;
	target <<= 2;
	u32 addr = (pc & 0xf0000000) | target;
	Helpers::log("j 0x%08x ; addr = 0x%08x\n", target, addr);
	branch_pc = addr;
}
void cpu::jal(instruction instr) {
	u32 target = instr.raw & 0x3ffffff;
	target <<= 2;
	u32 addr = (pc & 0xf0000000) | target;
	Helpers::log("jal 0x%08x ; addr = 0x%08x\n", target, addr);
	branch_pc = addr;
	gprs[31] = pc + 8;
}
void cpu::beq(instruction instr) {
	s32 offset = (s32)(s16)(instr.imm << 2);
	Helpers::log("beq %s, %s, 0x%04x\n", gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str(), offset);
	branch(offset, gprs[instr.rs] == gprs[instr.rt]);
}
void cpu::bne(instruction instr) {
	s32 offset = (s32)(s16)(instr.imm << 2);
	Helpers::log("bne %s, %s, 0x%04x\n", gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str(), offset);
	branch(offset, gprs[instr.rs] != gprs[instr.rt]);
}
void cpu::bgtz(instruction instr) {
	s32 offset = (s32)(s16)(instr.imm << 2);
	Helpers::log("bgtz %s, 0x%04x\n", gpr_names[instr.rs].c_str(), offset);
	branch(offset, gprs[instr.rs] > 0);
}
void cpu::addi(instruction instr) {
	Helpers::log("addi %s, %s, 0x%04x\n", gpr_names[instr.rt].c_str(), gpr_names[instr.rs].c_str(), instr.imm);
	s32 a = (s32)gprs[instr.rs];
	u32 b = (u32)(s16)instr.imm;
	s32 res = a + b; // TODO: check for integer overflow
	gprs[instr.rt] = res;
}
void cpu::addiu(instruction instr) {
	Helpers::log("addiu %s, %s, 0x%04x\n", gpr_names[instr.rt].c_str(), gpr_names[instr.rs].c_str(), instr.imm);
	s32 a = (s32)gprs[instr.rs];
	u32 b = (u32)(s16)instr.imm;
	s32 res = a + b;
	gprs[instr.rt] = res;
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
void cpu::lb(instruction instr) {
	s32 offset = (s32)(s16)instr.imm;
	u32 addr = gprs[instr.rs] + offset;
	Helpers::log("lb %s, 0x%04x(%s)  ;  %s <- [0x%08x]\n", gpr_names[instr.rt].c_str(), offset, gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str(), addr);
	gprs[instr.rt] = (s64)(s8)Memory->read<u8>(addr);
}
void cpu::lh(instruction instr) {
	s32 offset = (s32)(s16)instr.imm;
	u32 addr = gprs[instr.rs] + offset;
	Helpers::log("lh %s, 0x%04x(%s)  ;  %s <- [0x%08x]\n", gpr_names[instr.rt].c_str(), offset, gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str(), addr);
	gprs[instr.rt] = (s64)(s16)Memory->read<u16>(addr);
}
void cpu::lw(instruction instr) {
	s32 offset = (s32)(s16)instr.imm;
	u32 addr = gprs[instr.rs] + offset;
	Helpers::log("lw %s, 0x%04x(%s)  ;  %s <- [0x%08x]\n", gpr_names[instr.rt].c_str(), offset, gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str(), addr);
	gprs[instr.rt] = (s64)(s32)Memory->read<u32>(addr);
}
void cpu::lbu(instruction instr) {
	s32 offset = (s32)(s16)instr.imm;
	u32 addr = gprs[instr.rs] + offset;
	Helpers::log("lbu %s, 0x%04x(%s)  ;  %s <- [0x%08x]\n", gpr_names[instr.rt].c_str(), offset, gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str(), addr);
	gprs[instr.rt] = Memory->read<u8>(addr);
}
void cpu::lhu(instruction instr) {
	s32 offset = (s32)(s16)instr.imm;
	u32 addr = gprs[instr.rs] + offset;
	Helpers::log("lhu %s, 0x%04x(%s)  ;  %s <- [0x%08x]\n", gpr_names[instr.rt].c_str(), offset, gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str(), addr);
	gprs[instr.rt] = Memory->read<u16>(addr);
}
void cpu::sh(instruction instr) {
	s32 offset = (s32)(s16)instr.imm;
	u32 addr = gprs[instr.rs] + offset;
	Helpers::log("sh %s, 0x%04x(%s)  ;  [0x%08x] <- 0x%04x\n", gpr_names[instr.rt].c_str(), offset, gpr_names[instr.rs].c_str(), addr, gprs[instr.rt]);
	Memory->write<u16>(addr, gprs[instr.rt]);
}
void cpu::sw(instruction instr) {
	s32 offset = (s32)(s16)instr.imm;
	u32 addr = gprs[instr.rs] + offset;
	Helpers::log("sw %s, 0x%04x(%s)  ;  [0x%08x] <- 0x%08x\n", gpr_names[instr.rt].c_str(), offset, gpr_names[instr.rs].c_str(), addr, gprs[instr.rt]);
	Memory->write<u32>(addr, gprs[instr.rt]);
}
void cpu::ld(instruction instr) {
	s32 offset = (s32)(s16)instr.imm;
	u32 addr = gprs[instr.rs] + offset;
	Helpers::log("ld %s, 0x%04x(%s)  ;  %s <- [0x%08x]\n", gpr_names[instr.rt].c_str(), offset, gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str(), addr);
	gprs[instr.rt] = Memory->read<u64>(addr);
}
void cpu::sd(instruction instr) {
	s32 offset = (s32)(s16)instr.imm;
	u32 addr = gprs[instr.rs] + offset;
	Helpers::log("sd %s, 0x%04x(%s)  ;  [0x%08x] <- 0x%016x\n", gpr_names[instr.rt].c_str(), offset, gpr_names[instr.rs].c_str(), addr, gprs[instr.rt]);
	Memory->write<u64>(addr, gprs[instr.rt]);
}