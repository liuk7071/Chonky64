#include "../../Common/common.h"

class pi {
public:
	void load_cart(const char* path);
	void do_dma();
	u8* rdram;
	FILE* cart;
	u32 cart_format = 0;
	int file_mask = 0;
	std::vector<u8> cart_data;

	template<typename T> void write(u32 paddr, T data) {
		if constexpr (sizeof(T) == 4) {
			if (paddr == 0x04600000) dram_addr = data;
			else if (paddr == 0x04600004) cart_addr = data;
			else if (paddr == 0x0460000c) { wr_len = data; do_dma(); }
			else if (paddr == 0x04600010) printf("[PI] Write to status\n"); // Should acknowledge interrupt
			else if (paddr == 0x04600014) return;
			else if (paddr == 0x04600024) return;
			else if (paddr == 0x04600018) return;
			else if (paddr == 0x04600028) return;
			else if (paddr == 0x0460001c) return;
			else if (paddr == 0x0460002c) return;
			else if (paddr == 0x04600020) return;
			else if (paddr == 0x04600030) return;
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
		if constexpr (sizeof(T) == 8) {
			Helpers::panic("Unhandled PI 64bit read 0x%08x\n", paddr);
			return 0;
		}
		else if constexpr (sizeof(T) == 4) {
			if (paddr == 0x04600000) return dram_addr;
			else if (paddr == 0x04600004) return cart_addr;
			else if (paddr == 0x0460000c) return wr_len;
			else if (paddr == 0x04600010) return status;
			else if (paddr == 0x04600014) return 0;
			else if (paddr == 0x04600024) return 0;
			else if (paddr == 0x04600018) return 0;
			else if (paddr == 0x04600028) return 0;
			else if (paddr == 0x0460001c) return 0;
			else if (paddr == 0x0460002c) return 0;
			else if (paddr == 0x04600020) return 0;
			else if (paddr == 0x04600030) return 0;
			else Helpers::panic("Unhandled PI 32bit read 0x%08x\n", paddr);
		}
		else if constexpr (sizeof(T) == 2) {
			Helpers::panic("Unhandled PI 16bit read 0x%08x\n", paddr);
			return 0;
		}
		else if constexpr (sizeof(T) == 1) {
			Helpers::panic("Unhandled PI 8bit read 0x%08x\n", paddr);
			return 0;
		}
	}
	u32 dram_addr = 0;
	u32 cart_addr = 0;
	u32 wr_len = 0;
	u32 status = 0;
};