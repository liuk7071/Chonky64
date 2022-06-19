#include "../../Common/common.h"
#include "../../Core/Memory/memory.h"
#include <optional>

class cpu {
public:
	memory* Memory;
	cpu(memory* memptr);
	u32 pc = 0xBFC00000;
	std::optional<u32> branch_pc = std::nullopt;
	u64 gprs[32];
	std::string gpr_names[32] = { "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3","$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7","$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra" };

	void simulate_pif_rom();

	void step();

	enum instructions {
		SPECIAL = 0x00,
		J       = 0x02,
		JAL     = 0x03,
		BEQ     = 0x04,
		BNE     = 0x05,
		ADDIU   = 0x09,
		ANDI    = 0x0c,
		ORI     = 0x0d,
		LUI     = 0x0f,
		LH      = 0x21,
		LW      = 0x23,
		LBU     = 0x24,
		LHU     = 0x25,
		SH      = 0x29,
		SW      = 0x2b,
		LD      = 0x37,
		SD      = 0x3f
	};
	enum instructions_special {
		SLL  = 0x00,
		JR   = 0x08,
		JALR = 0x09,
		ADD  = 0x20,
		ADDU = 0x21
	};

	// Instructions
	struct instruction {
		u32 raw = 0;
		u8 rs = 0;
		u8 rd = 0;
		u8 rt = 0;
		u16 imm = 0;
		u8 sa = 0;
	};
	inline void branch(u32 offset, bool cond);

	void sll(instruction instr);
	void jr(instruction instr);
	void jalr(instruction instr);
	void add(instruction instr);
	void addu(instruction instr);

	void j(instruction instr);
	void jal(instruction instr);
	void beq(instruction instr);
	void bne(instruction instr);
	void addiu(instruction instr);
	void andi(instruction instr);
	void ori(instruction instr);
	void lui(instruction instr);
	void lh(instruction instr);
	void lw(instruction instr);
	void lbu(instruction instr);
	void lhu(instruction instr);
	void sh(instruction instr);
	void sw(instruction instr);
	void ld(instruction instr);
	void sd(instruction instr);
};