#pragma once

#include "types.h"

enum class function_id : uint32_t
{
	vendor = 0x80000000,
	identifiers = 0x80000001,
	svm_features = 0x8000000a,
	hv_interface_id = 0x40000001,
};

union cpuid
{
	int32_t regs[4];

	struct
	{
		uint32_t eax;
		uint32_t ebx;
		uint32_t ecx;
		uint32_t edx;
	} registers;

	struct
	{
		uint64_t largest_standard_function : 32;
		uint64_t v1 : 32;
		uint64_t v2 : 32;
		uint64_t v3 : 32;
	} vendor;

	struct
	{
		struct
		{
			uint32_t stepping : 4;
			uint32_t base_model : 4;
			uint32_t base_family : 4;
			uint32_t reserved0 : 4;
			uint32_t ext_model : 4;
			uint32_t ext_family : 8;
			uint32_t reserved1 : 4;
		} model;

		struct
		{
			uint32_t brand_id : 16;
			uint32_t reserved0 : 12;
			uint32_t pkg_type : 4;
		} brand_id;

		struct
		{
			uint32_t lahf_sahf : 1;
			uint32_t cmp_legacy : 1;
			uint32_t svm : 1;
			uint32_t ext_apic_space : 1;
			uint32_t alt_move_cr8 : 1;
			uint32_t abm : 1;
			uint32_t ssea4a : 1;
			uint32_t misalign_sse : 1;
			uint32_t now_prefetch : 1;
			uint32_t osvw : 1;
			uint32_t ibs : 1;
			uint32_t xop : 1;
			uint32_t skinit : 1;
			uint32_t wdt : 1;
			uint32_t reserved0 : 1;
			uint32_t lwp : 1;
			uint32_t fma4 : 1;
			uint32_t tce : 1;
			uint32_t reserved1 : 1;
			uint32_t reserved2 : 1;
			uint32_t reserved3 : 1;
			uint32_t tbm : 1;
			uint32_t topology_extension : 1;
			uint32_t perf_ctr_ext_core : 1;
			uint32_t perf_ctr_ext_nb : 1;
			uint32_t reserved4 : 1;
			uint32_t data_bkpt_ext : 1;
			uint32_t perf_tsc : 1;
			uint32_t perf_ctr_ext_llc : 1;
			uint32_t monitorx : 1;
			uint32_t addr_mask_ext : 1;
			uint32_t reserved5 : 1;
		} feature;

		struct
		{
			uint32_t fpu : 1;
			uint32_t vme : 1;
			uint32_t de : 1;
			uint32_t pse : 1;
			uint32_t tsc : 1;
			uint32_t msr : 1;
			uint32_t pae : 1;
			uint32_t mce : 1;
			uint32_t cmp_xchg8b : 1;
			uint32_t apic : 1;
			uint32_t reserved0 : 1;
			uint32_t sys_call_sys_ret : 1;
			uint32_t mtrr : 1;
			uint32_t pge : 1;
			uint32_t mca : 1;
			uint32_t cmove : 1;
			uint32_t pat : 1;
			uint32_t pse36 : 1;
			uint32_t reserved1 : 2;
			uint32_t nx : 1;
			uint32_t reserved2 : 1;
			uint32_t mmx_ext : 1;
			uint32_t mmx : 1;
			uint32_t fxsr : 1;
			uint32_t ffxsr : 1;
			uint32_t page1gb : 1;
			uint32_t rdtscp : 1;
			uint32_t reserved3 : 1;
			uint32_t now_ext : 1;
			uint32_t now : 1;
		} feature_ext;

	} identifiers;

	struct
	{
		struct
		{
			uint32_t svm_rev : 8;
			uint32_t reserved0 : 24;
		} revision;

		struct
		{
			uint32_t nasid;
		} revision_ex;

		struct
		{
			uint32_t reserved0;
		} reserved;

		struct
		{
			uint32_t np : 1;
			uint32_t lbr_virt : 1;
			uint32_t svm_l : 1;
			uint32_t nrips : 1;
			uint32_t tsc_rate_msr;
			uint32_t vmcb_clean : 1;
			uint32_t flush_by_asid : 1;
			uint32_t decode_assist : 1;
			uint32_t reserved0 : 2;
			uint32_t pause_filter : 1;
			uint32_t reserved1 : 1;
			uint32_t pause_filter_threshold : 1;
			uint32_t avic : 1;
			uint32_t reserved2 : 1;
			uint32_t vmsave_virt : 1;
			uint32_t vgif : 1;
			uint32_t gmet : 1;
			uint32_t x2_avic : 1;
			uint32_t spec_ctrl : 1;
			uint32_t rog_gpt : 1;
			uint32_t reserved3 : 1;
			uint32_t host_mce_override : 1;
			uint32_t tlbi_ctl : 1;
			uint32_t vnmi : 1;
			uint32_t ibs_virt : 1;
			uint32_t ext_lvt_avic_access_chg : 1;
			uint32_t nested_virt_vmcb_addr_chk : 1;
			uint32_t bus_lock_threshold : 1;
			uint32_t reserved4 : 2;
		} feature;

	} svm;

	struct
	{
		uint32_t signature;
		uint32_t ebx;
		uint32_t ecx;
		uint32_t edx;
	} hv_interface;
};
