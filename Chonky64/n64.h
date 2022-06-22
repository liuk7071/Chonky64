#pragma warning(disable : 4996)
#include "Core/CPU/cpu.h"

class n64 {
public:
	void step() {
		Cpu.step();
		Cpu.Memory->VI->step();
	}

	n64() {
		//const char* path = "C:\\Users\\zacse\\Downloads\\dillon-n64-tests-simpleboot\\addiu_simpleboot.z64";
		//const char* path = "C:\\Users\\zacse\\Downloads\\dillon-n64-tests-simpleboot\\basic_simpleboot.z64";
		//const char* path = "C:\\Users\\zacse\\Downloads\\dillon-n64-tests-simpleboot\\or_simpleboot.z64";
		//const char* path = "C:\\Users\\zacse\\Downloads\\dillon-n64-tests\\nor.z64";
		const char* path = "H:\\Games\\roms\\N64\\Namco Museum 64 (USA).n64";
		//const char* path = "C:\\Users\\zacse\\Downloads\\krom tests\\CPUDIV.n64";
		//const char* path = "H:\\Games\\roms\\N64\\Super Mario 64 (USA).n64";

		Memory.cart = fopen(path, "rb");
		Memory.PI->cart = fopen(path, "rb");
		int size = std::filesystem::file_size(path);
		Memory.PI->file_mask = pow(2, ceil(log(size) / log(2)));

		Cpu.simulate_pif_rom();
		Memory.PI->load_cart(path);
	}
	pi PI;
	vi VI;
	memory Memory = memory(&PI, &VI);
	cpu Cpu = cpu(&Memory);
};