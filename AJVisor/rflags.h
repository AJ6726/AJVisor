#pragma once
#include "types.h"

union rflags
{
	uint64_t value;

	struct
	{
		uint64_t cf : 1;
		uint64_t reserved0 : 1;
		uint64_t pf : 1;
		uint64_t reserved1 : 1;
		uint64_t af : 1;
		uint64_t reserved2 : 1;
		uint64_t zf : 1;
		uint64_t sf : 1;
		uint64_t tp : 1;
		uint64_t it : 1;
		uint64_t df : 1;
		uint64_t of : 1;
		uint64_t iopl : 1;
		uint64_t nt : 1;
		uint64_t reserved3 : 1;
		uint64_t rf : 1;
		uint64_t vm : 1;
		uint64_t ac : 1;
		uint64_t vif : 1;
		uint64_t vip : 1;
		uint64_t id : 1;
		uint64_t reserved4 : 42;
	};
};