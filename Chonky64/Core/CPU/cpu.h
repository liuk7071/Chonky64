#include "../../Common/common.h"
#include "../../Core/Memory/memory.h"

class cpu {
public:
	memory* Memory;
	cpu(memory* memptr);
	u32 pc = 0xBFC00000;
	u64 gprs[32];

	void simulate_pif_rom();

	void step();
};