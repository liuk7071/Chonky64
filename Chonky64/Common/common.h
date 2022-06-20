#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <cstdarg>
#include <intrin.h>
#include <WinSock2.h>
#include <filesystem>
#include <vector>
#include <fstream>
#pragma comment(lib, "Ws2_32.lib")

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

#define ROM_FORMAT_Z64 0x80371240
#define ROM_FORMAT_N64 0x40123780
#define ROM_FORMAT_V64 0x37804012

#define LOG
#undef LOG

#ifndef HELPERS
#define HELPERS
namespace Helpers {

	template <typename T>
	T htobe(T val) {
		if constexpr (sizeof(T) == 2) {
			return htons(val);
		}
		else if constexpr (sizeof(T) == 4) {
			return htonl(val);
		}
		else if constexpr (sizeof(T) == 8) {
			return htonll(val);
		}
		else if constexpr (sizeof(T) == 1) return val;
	}
	template <typename T>
	T betoh(T val) {
		if constexpr (sizeof(T) == 2) {
			return ntohs(val);
		}
		else if constexpr (sizeof(T) == 4) {
			return ntohl(val);
		}
		else if constexpr (sizeof(T) == 8) {
			return ntohll(val);
		}
		else if constexpr (sizeof(T) == 1) return val;
	}

	template <typename T>
	T read(u8* data) {
		T result;
		memcpy(&result, data, sizeof(T));
		return betoh<T>(result);
	}

	template <typename T>
	void write(u8* data, T val = 0) {
		T temp = htobe<T>(val);
		memcpy(data, &temp, sizeof(T));
	}

	inline void swap_region_v64(u8* data, int length) {
		for (int i = 0; i < length; i += 2) {
			u8 a = data[i];
			u8 b = data[i + 1];
			data[i] = b;
			data[i + 1] = a;
		}
	}

	inline void log(const char* fmt, ...) {
#ifdef LOG
		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
#endif
	}

	inline void panic(const char* err, ...) {
		va_list args;
		va_start(args, err);
		vprintf(err, args);
		va_end(args);
		exit(1);
	}
}
#endif