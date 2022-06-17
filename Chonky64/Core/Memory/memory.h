#include "../../Common/common.h"

class memory {
public:
	u8* rdram = new u8[0x400000];
	u8 dmem[0x1000];

	u32 vaddr_to_paddr(u32 vaddr);

	template<typename T>
	T read(u32 vaddr) {
		u32 paddr = vaddr_to_paddr(vaddr);
		T ret = 0;

		if (paddr >= 0x04000000 && paddr <= 0x04000FFF) ret = Helpers::access<T>(&dmem[paddr & 0xfff]);
		else Helpers::panic("Unhandled read from address 0x%08x\n", paddr);

		return ret;
	}

	template<typename T>
	void write(u32 vaddr, T data) {
		u32 paddr = vaddr_to_paddr(vaddr);

		Helpers::panic("Unhandled write to address 0x%08x\n", paddr);
	}

	FILE* cart;
};