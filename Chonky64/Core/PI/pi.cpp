#include "pi.h"

void pi::do_dma() {
	u32 length = wr_len + 1;
	fseek(cart, cart_addr & (file_size - 1), SEEK_SET);
	printf("0x%x\n", file_size - 1);
	fread(&rdram[dram_addr & 0x3fffff], sizeof(u8), length, cart);
	switch (cart_format) {
	case ROM_FORMAT_V64: Helpers::swap_region_v64(&rdram[dram_addr & 0x3fffff], length);
	}
	printf("[PI] rdram[0x%08x] <- cart[0x%08x], len: %d\n", dram_addr & 0x3fffff, cart_addr & (file_size - 1), length);
}