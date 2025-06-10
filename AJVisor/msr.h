#pragma once
#include "types.h"

#pragma warning (disable : 4201)

enum class msr_number : uint32_t
{
	efer = 0xC0000080,
	vmcr = 0xC0010114,
	scet = 0x000006A2,
	lstar = 0xC0000082,
	cstar = 0xC0000083,
	sfstar = 0xC0000084,
	hsave_pa = 0xC0010117,
	pat = 0x00000277,
};

union efer
{
	uint64_t value;
	struct
	{
		uint64_t sce : 1;
		uint64_t reserved0_raz : 7;
		uint64_t lme : 1;
		uint64_t reserved1_mbz : 1;
		uint64_t lma : 1;
		uint64_t nxe : 1;
		uint64_t svme : 1;
		uint64_t lmsle : 1;
		uint64_t ffxsr : 1;
		uint64_t tce : 1;
		uint64_t reserved2_mbz : 42;
	};
};

union vmcr
{
	uint64_t value;
	struct
	{
		uint64_t dpd : 1;
		uint64_t r_init : 1;
		uint64_t dis_a20m : 1;
		uint64_t lock : 1;
		uint64_t svm_dis : 1;
		uint64_t reserved0 : 59;
	};
};

union scet
{
	uint64_t value;
	struct
	{
		uint64_t sh_stk_en : 1;
		uint64_t wr_shsk_en : 1;
		uint64_t reserved1 : 62;
	};
};

union hsave_pa
{
	uint64_t value;
	struct
	{
		uint64_t mbz : 12;
		uint64_t host_save_area : 52;
	};
};

union star
{
	uint64_t value;
	struct
	{
		uint64_t syscall_target_eip : 32;
		uint64_t syscall_cs_ss : 16;
		uint64_t sysret_cs_ss : 16;
	};
};

union lstar
{
	uint64_t value;
	struct
	{
		uint64_t target_rip;
	};
};

union cstar
{
	uint64_t value;
	struct
	{
		uint64_t target_rip;
	};
};

union sfmask
{
	uint64_t value;
	struct
	{
		uint64_t syscall_flag_mask : 32;
		uint64_t reserved0 : 32;
	};
};


