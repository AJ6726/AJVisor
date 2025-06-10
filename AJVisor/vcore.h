#pragma once
#include "msr_pm.h"
#include "vmcb.h"

struct alignas(16) vcore_storage
{
	union
	{
		uint64_t bitfield1;
		struct
		{
			uint64_t stop_hv : 1;
			uint64_t stop_nrip : 1;
			uint64_t in_vm : 1;
		};
	};

	efer shadow_efer;
	hsave_pa shadow_hsave_pa;
};

struct alignas(0x1000) vcore
{
	union
	{
		uint8_t host_stack[0x6000];

		struct
		{
			uint8_t stack_data[0x6000 - sizeof(context) - (sizeof(uint64_t) * 3) - sizeof(machine_frame) - sizeof(vcore_storage)];
			context guest_context;
			uint64_t guest_vmcb_pa;
			uint64_t host_vmcb_pa;
			vcore* self;
			machine_frame machine_frame;
			vcore_storage storage;
		};
	};

	vmcb guest_vmcb;
	vmcb host_vmcb;
	msr_pm msr_pm;
};