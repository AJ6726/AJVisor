#pragma once
#include "exceptions.h"

union event_injection
{
	uint64_t value;
	struct
	{
		exception_vector vector : 8;
		interrupt_type type : 3;
		uint64_t error_code_valid : 1;
		uint64_t reserved0 : 19;
		uint64_t valid : 1;
		uint64_t error_code : 32;
	};
};