#include "Core/CPU/cpu.h"

class n64 {
public:
	pi PI;
	vi VI;
	memory Memory = memory(&PI, &VI);
	cpu Cpu = cpu(&Memory);
};