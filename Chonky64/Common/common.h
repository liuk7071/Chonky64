#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <cstdarg>
#include <intrin.h>
#include <WinSock2.h>
#include <filesystem>
#pragma comment(lib, "Ws2_32.lib")

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

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