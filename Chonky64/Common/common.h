#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <cstdarg>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

#ifndef HELPERS
#define HELPERS
namespace Helpers {
	inline void panic(const char* err, ...) {
		va_list args;
		va_start(args, err);
		vprintf(err, args);
		va_end(args);
		exit(1);
	}
}
#endif