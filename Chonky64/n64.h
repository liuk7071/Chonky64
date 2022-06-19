#include "Core/CPU/cpu.h"

class n64 {
public:
	pi Pi;
	memory Memory = memory(&Pi);
	cpu Cpu = cpu(&Memory);
};