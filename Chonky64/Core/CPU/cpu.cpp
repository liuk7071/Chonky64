#pragma warning(disable : 4996)
#include "cpu.h"

cpu::cpu(memory* memptr) {
	//const char* path = "C:\\Users\\zacse\\Downloads\\dillon-n64-tests-simpleboot\\addiu_simpleboot.z64";
	//const char* path = "C:\\Users\\zacse\\Downloads\\dillon-n64-tests-simpleboot\\basic_simpleboot.z64";
	//const char* path = "C:\\Users\\zacse\\Downloads\\dillon-n64-tests-simpleboot\\or_simpleboot.z64";
	//const char* path = "C:\\Users\\zacse\\Downloads\\dillon-n64-tests\\basic.z64";
	const char* path = "H:\\Games\\roms\\N64\\Namco Museum 64 (USA).n64";
	//const char* path = "C:\\Users\\zacse\\Downloads\\krom tests\\CPULB.n64";
	//const char* path = "H:\\Games\\roms\\N64\\Super Mario 64 (USA).n64";
	Memory = memptr;
	Memory->cart = fopen(path, "rb");
	Memory->PI->cart = fopen(path, "rb");
	int size = std::filesystem::file_size(path);
	Memory->PI->file_mask = pow(2, ceil(log(size) / log(2)));

	simulate_pif_rom();
	Memory->PI->load_cart(path);
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
		case instructions_special::SLL:    sll(inst); break;
		case instructions_special::SRL:    srl(inst); break;
		case instructions_special::SRA:    sra(inst); break;
		case instructions_special::SLLV:   sllv(inst); break;
		case instructions_special::SRLV:   srlv(inst); break;
		case instructions_special::JR:     jr(inst); break;
		case instructions_special::JALR:   jalr(inst); break;
		case instructions_special::MFHI:   mfhi(inst); break;
		case instructions_special::MTHI:   mthi(inst); break;
		case instructions_special::MFLO:   mflo(inst); break;
		case instructions_special::MTLO:   mtlo(inst); break;
		case instructions_special::MULT:   mult(inst); break;
		case instructions_special::MULTU:  multu(inst); break;
		case instructions_special::DIVU:   divu(inst); break;
		case instructions_special::ADD:    add(inst); break;
		case instructions_special::ADDU:   addu(inst); break;
		case instructions_special::SUBU:   subu(inst); break;
		case instructions_special::AND:    and_(inst); break;
		case instructions_special::OR:     or_(inst); break;
		case instructions_special::XOR:    xor_(inst); break;
		case instructions_special::SLT:    slt(inst); break;
		case instructions_special::SLTU:   sltu(inst); break;
		case instructions_special::DSRA32: dsra32(inst); break;
		default:
			Helpers::panic("Unhandled special instruction 0x%02x\n", instr & 0x3f);
		}
		break;
	}
	case instructions::REGIMM: {
		switch ((instr >> 16) & 0x1f) {
		case instructions_regimm::BGEZ:   bgez(inst); break;
		case instructions_regimm::BGEZL:  bgezl(inst); break;
		case instructions_regimm::BGEZAL: bgezal(inst); break;
		default:
			Helpers::panic("Unhandled regimm instruction 0x%02x\n", (instr >> 16) & 0x1f);
		}
		break;
	}
	case instructions::J:     j(inst); break;
	case instructions::JAL:   jal(inst); break;
	case instructions::BEQ:   beq(inst); break;
	case instructions::BNE:   bne(inst); break;
	case instructions::BLEZ:  blez(inst); break;
	case instructions::BGTZ:  bgtz(inst); break;
	case instructions::ADDI:  addi(inst); break;
	case instructions::ADDIU: addiu(inst); break;
	case instructions::SLTI:  slti(inst); break;
	case instructions::SLTIU: sltiu(inst); break;
	case instructions::ANDI:  andi(inst); break;
	case instructions::ORI:   ori(inst); break;
	case instructions::XORI:  xori(inst); break;
	case instructions::LUI:   lui(inst); break;
	case instructions::COP0: {
		switch ((instr >> 21) & 0x1f) {
		case instructions_cop0::MFC0: mfc0(inst); break;
		case instructions_cop0::MTC0: mtc0(inst); break;
		case 0x10: break;
		default:
			Helpers::panic("Unhandled cop0 instruction 0x%02x\n", (instr >> 21) & 0x1f);
		}
		break;
	}
	case instructions::FPU:   break; // Ignore FPU for now
	case instructions::BEQL:  beql(inst); break;
	case instructions::BNEL:  bnel(inst); break;
	case instructions::BLEZL: blezl(inst); break;
	case instructions::LB:    lb(inst); break;
	case instructions::LH:    lh(inst); break;
	case instructions::LW:    lw(inst); break;
	case instructions::LBU:   lbu(inst); break;
	case instructions::LHU:   lhu(inst); break;
	case instructions::LWU:   lwu(inst); break;
	case instructions::SB:    sb(inst); break;
	case instructions::SH:    sh(inst); break;
	case instructions::SW:    sw(inst); break;
	case instructions::CACHE: cache(inst); break;
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

	u32 format = (Memory->dmem[0] << 24) | (Memory->dmem[1] << 16) | (Memory->dmem[2] << 8) | (Memory->dmem[3]);
	Memory->PI->cart_format = format;

	printf("0x%08x\n", format);
	switch (format) {
	case ROM_FORMAT_V64: Helpers::swap_region_v64(Memory->dmem, 0x1000);
	}
	format = (Memory->dmem[0] << 24) | (Memory->dmem[1] << 16) | (Memory->dmem[2] << 8) | (Memory->dmem[3]);
	printf("0x%08x\n", format);
}

inline void cpu::branch(u32 offset, bool cond, bool likely) {
	if (cond) branch_pc = pc + offset + 4;
	else if (likely) pc += 4;
}
// Instructions
void cpu::sll(instruction instr) {
	Helpers::log("sll %s, %s, 0x%04x\n", gpr_names[instr.rd].c_str(), gpr_names[instr.rt].c_str(), instr.sa);
	s32 shifted = (u32)gprs[instr.rt] << instr.sa;
	gprs[instr.rd] = (s64)shifted;
}
void cpu::srl(instruction instr) {
	Helpers::log("srl %s, %s, 0x%04x\n", gpr_names[instr.rd].c_str(), gpr_names[instr.rt].c_str(), instr.sa);
	s32 shifted = (u32)gprs[instr.rt] >> instr.sa;
	gprs[instr.rd] = (s64)shifted;
}
void cpu::sra(instruction instr) {
	Helpers::log("sra %s, %s, 0x%04x\n", gpr_names[instr.rd].c_str(), gpr_names[instr.rt].c_str(), instr.sa);
	s32 shifted = (s32)gprs[instr.rt] >> instr.sa;
	gprs[instr.rd] = (s64)shifted;
}
void cpu::sllv(instruction instr) {
	Helpers::log("sllv %s, %s, %s\n", gpr_names[instr.rd].c_str(), gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str());
	s32 shifted = (u32)gprs[instr.rt] << (gprs[instr.rs] & 0x1f);
	gprs[instr.rd] = (s64)shifted;
}
void cpu::srlv(instruction instr) {
	Helpers::log("srlv %s, %s, %s\n", gpr_names[instr.rd].c_str(), gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str());
	s32 shifted = (u32)gprs[instr.rt] >> (gprs[instr.rs] & 0x1f);
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
void cpu::mfhi(instruction instr) {
	Helpers::log("mfhi %s\n", gpr_names[instr.rd].c_str());
	gprs[instr.rd] = hi;
}
void cpu::mthi(instruction instr) {
	Helpers::log("mthi %s\n", gpr_names[instr.rs].c_str());
	hi = gprs[instr.rs];
}
void cpu::mflo(instruction instr) {
	Helpers::log("mflo %s\n", gpr_names[instr.rd].c_str());
	gprs[instr.rd] = lo;
}
void cpu::mtlo(instruction instr) {
	Helpers::log("mtlo %s\n", gpr_names[instr.rs].c_str());
	lo = gprs[instr.rs];
}
void cpu::mult(instruction instr) {
	Helpers::log("mult %s, %s\n", gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str());
	s32 a = (s32)gprs[instr.rs];
	s32 b = (s32)gprs[instr.rt];
	s64 res = (s64)a * (s64)b;
	hi = (s64)(s32)((res >> 32) & 0xffffffff);
	lo = (s64)(s32)(res & 0xffffffff);
}
void cpu::multu(instruction instr) {
	Helpers::log("multu %s, %s\n", gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str());
	u32 a = (u32)gprs[instr.rs];
	u32 b = (u32)gprs[instr.rt];
	s64 res = (u64)a * (u64)b;
	hi = (s64)(s32)((res >> 32) & 0xffffffff);
	lo = (s64)(s32)(res & 0xffffffff);
}
void cpu::divu(instruction instr) {
	Helpers::log("divu %s, %s\n", gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str());
	u32 a = (u32)gprs[instr.rs];
	u32 b = (u32)gprs[instr.rt];
	if (b == 0) {
		hi = a;
		lo = -1;
		return;
	}
	s64 res = (u64)a / (u64)b;
	s64 rem = (u64)a % (u64)b;
	hi = rem;
	lo = res;
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
void cpu::subu(instruction instr) {
	Helpers::log("subu %s, %s, %s\n", gpr_names[instr.rd].c_str(), gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str());
	s32 a = (s32)gprs[instr.rs];
	s32 b = (s32)gprs[instr.rt];
	s32 res = a - b;
	gprs[instr.rd] = res;
}
void cpu::and_(instruction instr) {
	Helpers::log("and %s, %s, %s\n", gpr_names[instr.rd].c_str(), gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str());
	gprs[instr.rd] = gprs[instr.rs] & gprs[instr.rt];
}
void cpu::or_(instruction instr) {
	Helpers::log("or %s, %s, %s\n", gpr_names[instr.rd].c_str(), gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str());
	gprs[instr.rd] = gprs[instr.rs] | gprs[instr.rt];
}
void cpu::xor_(instruction instr) {
	Helpers::log("xor %s, %s, %s\n", gpr_names[instr.rd].c_str(), gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str());
	gprs[instr.rd] = gprs[instr.rs] ^ gprs[instr.rt];
}
void cpu::slt(instruction instr) {
	Helpers::log("slt %s, %s, %s\n", gpr_names[instr.rd].c_str(), gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str());
	gprs[instr.rd] = ((s64)gprs[instr.rs] < (s64)gprs[instr.rt]);
}
void cpu::sltu(instruction instr) {
	Helpers::log("sltu %s, %s, %s\n", gpr_names[instr.rd].c_str(), gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str());
	gprs[instr.rd] = (gprs[instr.rs] < gprs[instr.rt]);
}
void cpu::dsra32(instruction instr) {
	Helpers::log("dsra32 %s, %s, 0x%04x\n", gpr_names[instr.rd].c_str(), gpr_names[instr.rt].c_str(), instr.sa);
	s64 shifted = (s64)gprs[instr.rt] >> (instr.sa + 32);
	gprs[instr.rd] = shifted;
}

void cpu::bgez(instruction instr) {
	s32 offset = (s32)(s16)(instr.imm);
	offset <<= 2;
	Helpers::log("bgez %s, 0x%04x\n", gpr_names[instr.rs].c_str(), offset);
	branch(offset, (s64)gprs[instr.rs] >= 0, false);
}
void cpu::bgezl(instruction instr) {
	s32 offset = (s32)(s16)(instr.imm);
	offset <<= 2;
	Helpers::log("bgezl %s, 0x%04x\n", gpr_names[instr.rs].c_str(), offset);
	branch(offset, (s64)gprs[instr.rs] >= 0, true);
}
void cpu::bgezal(instruction instr) {
	s32 offset = (s32)(s16)(instr.imm);
	offset <<= 2;
	Helpers::log("bgezl %s, 0x%04x\n", gpr_names[instr.rs].c_str(), offset);
	branch(offset, (s64)gprs[instr.rs] >= 0);
	gprs[31] = pc + 8;
}

void cpu::mfc0(instruction instr) {
	Helpers::log("mfc0 %s, cop0r%d\n", gpr_names[instr.rt].c_str(), gprs[instr.rd]);
	gprs[instr.rt] = (u64)(s32)cop0_regs[instr.rd];
}
void cpu::mtc0(instruction instr) {
	Helpers::log("mtc0 %s, cop0r%d\n", gpr_names[instr.rt].c_str(), gprs[instr.rd]);
	cop0_regs[instr.rd] = gprs[instr.rt];
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
	s32 offset = (s32)(s16)(instr.imm);
	offset <<= 2;
	Helpers::log("beq %s, %s, 0x%04x\n", gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str(), offset);
	branch(offset, gprs[instr.rs] == gprs[instr.rt]);
}
void cpu::bne(instruction instr) {
	s32 offset = (s32)(s16)(instr.imm);
	offset <<= 2;
	Helpers::log("bne %s, %s, 0x%04x\n", gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str(), offset);
	branch(offset, gprs[instr.rs] != gprs[instr.rt]);
}
void cpu::blez(instruction instr) {
	s32 offset = (s32)(s16)(instr.imm);
	offset <<= 2;
	Helpers::log("blez %s, 0x%04x\n", gpr_names[instr.rs].c_str(), offset);
	branch(offset, (s64)gprs[instr.rs] <= 0);
}
void cpu::bgtz(instruction instr) {
	s32 offset = (s32)(s16)(instr.imm);
	offset <<= 2;
	Helpers::log("bgtz %s, 0x%04x\n", gpr_names[instr.rs].c_str(), offset);
	branch(offset, (s64)gprs[instr.rs] > 0);
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
void cpu::slti(instruction instr) {
	Helpers::log("slti %s, %s, 0x%04x\n", gpr_names[instr.rt].c_str(), gpr_names[instr.rs].c_str(), instr.imm);
	gprs[instr.rt] = (s64)gprs[instr.rs] < (s64)(s16)instr.imm;
}
void cpu::sltiu(instruction instr) {
	Helpers::log("sltiu %s, %s, 0x%04x\n", gpr_names[instr.rt].c_str(), gpr_names[instr.rs].c_str(), instr.imm);
	gprs[instr.rt] = gprs[instr.rs] < (u64)(s16)instr.imm;
}
void cpu::andi(instruction instr) {
	Helpers::log("andi %s, %s, 0x%04x\n", gpr_names[instr.rt].c_str(), gpr_names[instr.rs].c_str(), instr.imm);
	gprs[instr.rt] = (gprs[instr.rs] & instr.imm);
}
void cpu::ori(instruction instr) {
	Helpers::log("ori %s, %s, 0x%04x\n", gpr_names[instr.rt].c_str(), gpr_names[instr.rs].c_str(), instr.imm);
	gprs[instr.rt] = (gprs[instr.rs] | instr.imm);
}
void cpu::xori(instruction instr) {
	Helpers::log("xori %s, %s, 0x%04x\n", gpr_names[instr.rt].c_str(), gpr_names[instr.rs].c_str(), instr.imm);
	gprs[instr.rt] = (gprs[instr.rs] ^ instr.imm);
}
void cpu::lui(instruction instr) {
	Helpers::log("lui %s, 0x%04x\n", gpr_names[instr.rt].c_str(), instr.imm);
	s32 imm_shifted = instr.imm << 16;
	gprs[instr.rt] = (s64)(imm_shifted);
}
void cpu::beql(instruction instr) {
	s32 offset = (s32)(s16)(instr.imm);
	offset <<= 2;
	Helpers::log("beql %s, %s, 0x%04x\n", gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str(), offset);
	branch(offset, gprs[instr.rs] == gprs[instr.rt], true);
}
void cpu::bnel(instruction instr) {
	s32 offset = (s32)(s16)(instr.imm);
	offset <<= 2;
	Helpers::log("bnel %s, %s, 0x%04x\n", gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str(), offset);
	branch(offset, gprs[instr.rs] != gprs[instr.rt], true);
}
void cpu::blezl(instruction instr) {
	s32 offset = (s32)(s16)(instr.imm);
	offset <<= 2;
	Helpers::log("blezl %s, 0x%04x\n", gpr_names[instr.rs].c_str(), offset);
	branch(offset, (s64)gprs[instr.rs] <= 0, true);
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
void cpu::lwu(instruction instr) {
	s32 offset = (s32)(s16)instr.imm;
	u32 addr = gprs[instr.rs] + offset;
	Helpers::log("lwu %s, 0x%04x(%s)  ;  %s <- [0x%08x]\n", gpr_names[instr.rt].c_str(), offset, gpr_names[instr.rs].c_str(), gpr_names[instr.rt].c_str(), addr);
	gprs[instr.rt] = Memory->read<u32>(addr);
}
void cpu::sb(instruction instr) {
	s32 offset = (s32)(s16)instr.imm;
	u32 addr = gprs[instr.rs] + offset;
	Helpers::log("sb %s, 0x%04x(%s)  ;  [0x%08x] <- 0x%02x\n", gpr_names[instr.rt].c_str(), offset, gpr_names[instr.rs].c_str(), addr, gprs[instr.rt]);
	Memory->write<u8>(addr, gprs[instr.rt]);
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
void cpu::cache(instruction instr) {
	Helpers::log("cache (unimplemented)");
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