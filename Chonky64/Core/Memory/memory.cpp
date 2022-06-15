#include "memory.h"

u32 memory::vaddr_to_paddr(u32 vaddr) {
	u32 paddr = 0;
	// Direct mapped segments
	if (vaddr >= 0x80000000 && vaddr <= 0x9FFFFFFF) {  // KSEG0
		paddr = vaddr - 0x80000000;
	}
	else if (vaddr >= 0xA0000000 && vaddr <= 0xBFFFFFFF) { // KSEG1
		paddr = vaddr - 0xA0000000;
	}
	else Helpers::panic("Access to TLB mapped segment (0x%08x)", vaddr);
	
	return paddr;
}