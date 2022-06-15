#include "Core/CPU/cpu.h"

class n64 {
public:
	memory Memory;
	cpu Cpu = cpu(&Memory);
};