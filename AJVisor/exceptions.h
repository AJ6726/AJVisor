#pragma once
#include "types.h"

enum class interrupt_type : uint64_t
{
	external = 0,
	nmi = 2,
	hardware = 3,
	software = 4,
};

enum class exception_vector : uint64_t
{
	division_error = 0,
	debug = 1,
	non_maskable_interrupt = 2,
	breakpoint = 3,
	overflow = 4,
	bound_range_exceeded = 5,
	invalid_opcode = 6,
	unavailable_device = 7,
	double_fault = 8,
	coprocessor_segment_overrun = 9,
	invalid_tss = 10,
	segment_not_present = 11,
	stack_segment = 12,
	general_protection = 13,
	page_fault = 14,
	x87_floating_point = 16,
	alignment_check = 17,
	machine_check = 18,
	simd_floating_point = 19,
	virtualization = 20,
	control_protection = 21,
	hypervisor_injection = 28,
	vmm_communication = 29,
	security = 30
};
