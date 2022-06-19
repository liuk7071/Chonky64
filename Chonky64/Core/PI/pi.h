#include "../../Common/common.h"

class pi {
public:
	void do_dma();
	u8* rdram;
	FILE* cart;
	int file_size = 0;

	template<typename T> void write(u32 paddr, T data) {
		if constexpr (sizeof(T) == 4) {
			if (paddr == 0x04600000) dram_addr = data;
			else if (paddr == 0x04600004) cart_addr = data;
			else if (paddr == 0x0460000c) { wr_len = data; do_dma(); }
			else Helpers::panic("Unhandled PI 32bit write 0x%08x\n", paddr);
		}
		else if constexpr (sizeof(T) == 2) {
			Helpers::panic("Unhandled PI 16bit write 0x%08x\n", paddr);
		}
		else if constexpr (sizeof(T) == 1) {
			Helpers::panic("Unhandled PI 8bit write 0x%08x\n", paddr);
		}
	}
	template<typename T> T read(u32 paddr) {
		if constexpr (sizeof(T) == 4) {
			if (paddr == 0x04600000) return dram_addr;
			else if (paddr == 0x04600004) return cart_addr;
			else if (paddr == 0x0460000c) return wr_len;
			else if (paddr == 0x04600010) return status;
			else Helpers::panic("Unhandled PI 32bit read 0x%08x\n", paddr);
		}
		else if constexpr (sizeof(T) == 2) {
			Helpers::panic("Unhandled PI 16bit read 0x%08x\n", paddr);
		}
		else if constexpr (sizeof(T) == 1) {
			Helpers::panic("Unhandled PI 8bit read 0x%08x\n", paddr);
		}
	}
	u32 dram_addr = 0;
	u32 cart_addr = 0;
	u32 wr_len = 0;
	u32 status = 0;
};