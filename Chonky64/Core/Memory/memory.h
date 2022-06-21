#include "../../Common/common.h"
#include "../PI/Pi.h"
#include "../VI/vi.h"

class memory {
public:
	u8* rdram = new u8[0x800000];
	pi* PI;
	vi* VI;
	memory(pi* piptr, vi* viptr) {
		PI = piptr;
		VI = viptr;
		PI->rdram = this->rdram;
		for (int i = 0; i < 0x400000; i++) rdram[i] = 0;
	}

	u8 dmem[0x1000];
	u8 imem[0x1000];
	u8 pif_ram[0x40];

	u32 vaddr_to_paddr(u32 vaddr);

	template<typename T>
	T read(u32 vaddr) {
		u32 paddr = vaddr_to_paddr(vaddr);
		T ret = 0;

		if (paddr >= 0x00000000 && paddr <= 0x007FFFFF) ret = Helpers::read<T>(&rdram[paddr & 0x7fffff]);
		else if (paddr >= 0x03F00000 && paddr <= 0x03FFFFFF) ret = 0; // "RDRAM MMIO, configures timings, etc. Irrelevant for emulation."
		else if (paddr >= 0x04000000 && paddr <= 0x04000FFF) ret = Helpers::read<T>(&dmem[paddr & 0xfff]);
		else if (paddr >= 0x04001000 && paddr <= 0x04001FFF) ret = Helpers::read<T>(&imem[paddr & 0xfff]);
		else if (paddr >= 0x04040000 && paddr <= 0x040FFFFF) { ret = 0; printf("[SP] IGNORED SP REGS READ 0x%08x\n", paddr); }
		else if (paddr >= 0x04300000 && paddr <= 0x043FFFFF) { ret = 0;  printf("[MI] IGNORED MI READ 0x%08x\n", paddr); }
		else if (paddr >= 0x04400000 && paddr <= 0x044FFFFF) ret = VI->read<T>(paddr);
		else if (paddr >= 0x04600000 && paddr <= 0x046FFFFF) ret = PI->read<T>(paddr);
		else if (paddr >= 0x04700000 && paddr <= 0x047FFFFF) ret = 0; // "Control RDRAM settings (timings?) Irrelevant for emulation."
		else if (paddr >= 0x04800000 && paddr <= 0x048FFFFF) printf("[SI] IGNORED SI READ 0x%08x\n", paddr);
		else if (paddr >= 0x10000000 && paddr <= 0x1FBFFFFF) ret = Helpers::read<T>(&PI->cart_data[paddr & (PI->file_mask - 1)]); 
		else if (paddr >= 0x1FC007C0 && paddr <= 0x1FC007FF) ret = Helpers::read<T>(&pif_ram[paddr & 0x3f]);
		else Helpers::panic("Unhandled read from address 0x%08x\n", paddr);

		return ret;
	}

	template<typename T>
	void write(u32 vaddr, T data) {
		u32 paddr = vaddr_to_paddr(vaddr);

		if (paddr >= 0x00000000 && paddr <= 0x003FFFFF) Helpers::write<T>(&rdram[paddr & 0x3fffff], data);
		else if (paddr >= 0x03F00000 && paddr <= 0x03FFFFFF) return; // "RDRAM MMIO, configures timings, etc. Irrelevant for emulation."
		else if (paddr >= 0x04000000 && paddr <= 0x04000FFF) Helpers::write<T>(&dmem[paddr & 0xfff], data);
		else if (paddr >= 0x04001000 && paddr <= 0x04001FFF) Helpers::write<T>(&imem[paddr & 0xfff], data);
		else if (paddr >= 0x04040000 && paddr <= 0x040FFFFF) printf("[SP] IGNORED SP REGS WRITE 0x%08x\n", paddr);
		else if (paddr >= 0x04300000 && paddr <= 0x043FFFFF) printf("[MI] IGNORED MI WRITE 0x%08x\n", paddr);
		else if (paddr >= 0x04400000 && paddr <= 0x044FFFFF) VI->write<T>(paddr, data);
		else if (paddr >= 0x04500000 && paddr <= 0x045FFFFF) printf("[AI] IGNORED AI WRITE 0x%08x\n", paddr);
		else if (paddr >= 0x04600000 && paddr <= 0x046FFFFF) PI->write<T>(paddr, data);
		else if (paddr >= 0x04700000 && paddr <= 0x047FFFFF) return; // "Control RDRAM settings (timings?) Irrelevant for emulation."
		else if (paddr >= 0x04800000 && paddr <= 0x048FFFFF) printf("[SI] IGNORED SI WRITE 0x%08x\n", paddr);
		else if (paddr >= 0x1FC007C0 && paddr <= 0x1FC007FF) Helpers::write<T>(&pif_ram[paddr & 0x3f], data);
		else Helpers::panic("Unhandled write to address 0x%08x\n", paddr);
	}

	FILE* cart;
};