#pragma once
#include "types.h"
#include "msr.h"
#include "wrappers.h"

enum class msr_intercept : uint64_t
{
	rdmsr = 0,
	wrmsr = 1,
};

enum class msr_access : int32_t
{
	read = 0,
	write = 1
};

union alignas(0x1000) msr_pm
{
	//64k bits in total, 2 pages
	uint8_t bits[0x2000];

	struct
	{
		uint8_t vector1[2048]; //16 kilobits
		uint8_t vector2[2048];
		uint8_t vector3[2048];
		uint8_t vector4[2048];
	};

	void set(msr_number msr, msr_access access, bool value = true)
	{
		uint8_t* vector = nullptr;
		uint32_t index = 0;

		uint32_t msr_num = static_cast<uint32_t>(msr); 

		if (msr_num > 0x00000000 && msr_num <= 0x00001FFF)
		{
			vector = vector1;
			index = msr_num - 0x00000000;
		}
		if (msr_num >= 0xC0000000 && msr_num <= 0xC0001FFF)
		{
			vector = vector2;
			index = msr_num - 0xC0000000;
		}

		if (msr_num >= 0xC0010000 && msr_num <= 0xC0011FFF)
		{
			vector = vector3;
			index = msr_num - 0xC0010000;
		}

		if (!vector)
			return;

		index = (index * 2) + static_cast<int32_t>(access);

		if (value)
			vector[index / 8] |= (1ll << (index % 8));
		else
			vector[index / 8] &= ~(1ll << (index % 8));
	}
};