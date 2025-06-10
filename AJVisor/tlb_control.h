#pragma once
#include "types.h"

enum class tlb_control_id : uint64_t
{
	do_nothing = 0,
	flush_entire_tlb = 1,
	flush_guest_tlb = 3,
	flush_guest_nonglobal_tlb = 7
};