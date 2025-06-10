#pragma once
#include <wdm.h>
#include "Types.h"

union register_64
{
	uint64_t value;

	struct
	{
		uint32_t low;
		uint32_t high;
	};

	struct
	{
		uint64_t qword;
	};

	struct
	{
		uint32_t dword1;
		uint32_t dword2;
	};

	struct {
		uint16_t word1;
		uint16_t word2;
		uint16_t word3;
		uint16_t word4;
	};

	struct
	{
		uint8_t byte1;
		uint8_t byte2;
		uint8_t byte3;
		uint8_t byte4;
		uint8_t byte5;
		uint8_t byte6;
		uint8_t byte7;
		uint8_t byte8;
	};
};

struct alignas(16) xmm128
{
	uint64_t low;
	int64_t high;
}; 

struct context
{
	union {
		register_64 gpr[16];
		struct {
			register_64 rax;
			register_64 rcx;
			register_64 rdx;
			register_64 rbx;
			register_64 rsp;
			register_64 rbp;
			register_64 rsi;
			register_64 rdi;
			register_64 r8;
			register_64 r9;
			register_64 r10;
			register_64 r11;
			register_64 r12;
			register_64 r13;
			register_64 r14;
			register_64 r15;
		};
	};


	xmm128 xmm1280;
	xmm128 xmm1281;
	xmm128 xmm1282;
	xmm128 xmm1283;
	xmm128 xmm1284;
	xmm128 xmm1285;
	xmm128 xmm1286;
	xmm128 xmm1287;
	xmm128 xmm1288;
	xmm128 xmm1289;
	xmm128 xmm12810;
	xmm128 xmm12811;
	xmm128 xmm12812;
	xmm128 xmm12813;
	xmm128 xmm12814;
	xmm128 xmm12815;
};