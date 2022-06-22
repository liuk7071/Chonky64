#include "../../Common/common.h"
#include "../../Core/Memory/memory.h"
#include <optional>

#define COP0_STATUS 12
#define COP0_CAUSE 13
#define COP0_EPC 14
#define COP0_ERROREPC 30

class cpu {
public:
	memory* Memory;
	cpu(memory* memptr);
	u32 pc = 0xBFC00000;
	std::optional<u32> branch_pc = std::nullopt;
	u64 gprs[32];
	u64 hi = 0, lo = 0;
	u64 cop0_regs[32];
	std::string gpr_names[32] = { "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3","$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7","$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra" };

	void simulate_pif_rom();

	inline void check_vi_intr();
	void handle_interrupts();
	void step();
	int cycles = 0;

	enum exceptions {
		INT = 0x00
	};
	void exception(exceptions exce);

	enum instructions {
		SPECIAL = 0x00,
		REGIMM  = 0x01,
		J       = 0x02,
		JAL     = 0x03,
		BEQ     = 0x04,
		BNE     = 0x05,
		BLEZ    = 0x06,
		BGTZ    = 0x07,
		ADDI    = 0x08,
		ADDIU   = 0x09,
		SLTI    = 0x0a,
		SLTIU   = 0x0b,
		ANDI    = 0x0c,
		ORI     = 0x0d,
		XORI    = 0x0e,
		LUI     = 0x0f,
		COP0    = 0x10,
		FPU     = 0x11,
		BEQL    = 0x14,
		BNEL    = 0x15,
		BLEZL   = 0x16,
		LB      = 0x20,
		LH      = 0x21,
		LW      = 0x23,
		LBU     = 0x24,
		LHU     = 0x25,
		LWU     = 0x27,
		SB      = 0x28,
		SH      = 0x29,
		SW      = 0x2b,
		CACHE   = 0x2f,
		LWC1    = 0x31,
		LD      = 0x37,
		SD      = 0x3f
	};
	enum instructions_special {
		SLL    = 0x00,
		SRL    = 0x02,
		SRA    = 0x03,
		SLLV   = 0x04,
		SRLV   = 0x06,
		JR     = 0x08,
		JALR   = 0x09,
		MFHI   = 0x10,
		MTHI   = 0x11,
		MFLO   = 0x12,
		MTLO   = 0x13,
		MULT   = 0x18,
		MULTU  = 0x19,
		DIV    = 0x1a,
		DIVU   = 0x1b,
		ADD    = 0x20,
		ADDU   = 0x21,
		SUBU   = 0x23,
		AND    = 0x24,
		OR     = 0x25,
		XOR    = 0x26,
		NOR    = 0x27,
		SLT    = 0x2a,
		SLTU   = 0x2b,
		DSLL   = 0x38,
		DSLL32 = 0x3c,
		DSRA32 = 0x3f
	};
	enum instructions_regimm {
		BGEZ   = 0x01,
		BGEZL  = 0x03,
		BGEZAL = 0x11
	};
	enum instructions_cop0 {
		MFC0 = 0x00,
		MTC0 = 0x04,
		CO   = 0x10
	};
	enum operations_cop0 {
		TLBWI = 0x02,
		ERET  = 0x18
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
	inline void branch(u32 offset, bool cond, bool likely = false);

	void sll(instruction instr);
	void srl(instruction instr);
	void sra(instruction instr);
	void sllv(instruction instr);
	void srlv(instruction instr);
	void jr(instruction instr);
	void jalr(instruction instr);
	void mfhi(instruction instr);
	void mflo(instruction instr);
	void mthi(instruction instr);
	void mtlo(instruction instr);
	void mult(instruction instr);
	void multu(instruction instr);
	void div(instruction instr);
	void divu(instruction instr);
	void add(instruction instr);
	void addu(instruction instr);
	void subu(instruction instr);
	void and_(instruction instr);
	void or_(instruction instr);
	void xor_(instruction instr);
	void nor(instruction instr);
	void slt(instruction instr);
	void sltu(instruction instr);
	void dsll(instruction instr);
	void dsll32(instruction instr);
	void dsra32(instruction instr);

	void bgez(instruction instr);
	void bgezl(instruction instr);
	void bgezal(instruction instr);

	void mfc0(instruction instr);
	void mtc0(instruction instr);
	
	void eret(instruction instr);

	void j(instruction instr);
	void jal(instruction instr);
	void beq(instruction instr);
	void bne(instruction instr);
	void blez(instruction instr);
	void bgtz(instruction instr);
	void addi(instruction instr);
	void addiu(instruction instr);
	void slti(instruction instr);
	void sltiu(instruction instr);
	void andi(instruction instr);
	void ori(instruction instr);
	void xori(instruction instr);
	void lui(instruction instr);
	void beql(instruction instr);
	void bnel(instruction instr);
	void blezl(instruction instr);
	void lb(instruction instr);
	void lh(instruction instr);
	void lw(instruction instr);
	void lbu(instruction instr);
	void lhu(instruction instr);
	void lwu(instruction instr);
	void sb(instruction instr);
	void sh(instruction instr);
	void sw(instruction instr);
	void cache(instruction instr);
	void lwc1(instruction instr);
	void ld(instruction instr);
	void sd(instruction instr);
};