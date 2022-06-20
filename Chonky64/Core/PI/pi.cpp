#include "pi.h"

void pi::load_cart(const char* path) {
	std::ifstream file(path, std::ios::binary);
	if (!file.is_open()) {
		std::cout << "Couldn't find ROM at " << path << "\n";
	}

	file.unsetf(std::ios::skipws);
	std::streampos fileSize;
	file.seekg(0, std::ios::end);
	fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	cart_data.insert(cart_data.begin(),
		std::istream_iterator<uint8_t>(file),
		std::istream_iterator<uint8_t>());
	file.close();

	switch (cart_format) {
	case ROM_FORMAT_V64: printf("Detected V64 cart\n"); Helpers::swap_region_v64(cart_data.data(), cart_data.size());
	}
}

void pi::do_dma() {
	u32 length = (wr_len & 0xffffff) + 1;
	fseek(cart, cart_addr & (file_mask - 1), SEEK_SET);
	fread(&rdram[dram_addr & 0x7fffff], sizeof(u8), length, cart);
	switch (cart_format) {
	case ROM_FORMAT_V64: Helpers::swap_region_v64(&rdram[dram_addr & 0x7fffff], length);
	}
	printf("[PI] rdram[0x%08x] <- cart[0x%08x], len: %d\n", dram_addr & 0x7fffff, cart_addr & (file_mask - 1), length);
}