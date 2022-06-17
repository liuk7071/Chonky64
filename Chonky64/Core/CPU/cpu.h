#include "../../Common/common.h"
#include "../../Core/Memory/memory.h"

class cpu {
public:
	memory* Memory;
	cpu(memory* memptr);
	u32 pc = 0xBFC00000;
	u64 gprs[32];
	std::string gpr_names[32] = { "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3","$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7","$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra" };

	void simulate_pif_rom();

	void step();

	enum instructions {
		ORI = 0x0d,
		LUI = 0x0f,
		SW  = 0x2b
	};

	// Instructions
	struct instruction {
		u8 rs = 0;
		u8 rd = 0;
		u8 rt = 0;
		u16 imm = 0;
	};

	void ori(instruction instr);
	void lui(instruction instr);
	void sw(instruction instr);
};