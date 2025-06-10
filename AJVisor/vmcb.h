#include "context.h"
#include "cpuid.h"
#include "control_register.h"
#include "exceptions.h"
#include "exit_info.h"
#include "event_injection.h"
#include "machine_frame.h"
#include "msr.h"
#include "paging.h"
#include "rflags.h"
#include "segments.h"
#include "tlb_control.h"
#include "vmexit_codes.h"

struct control_area
{
	union
	{
		uint32_t value0;
		struct
		{
			uint32_t cr0_read : 1;
			uint32_t cr1_read : 1;
			uint32_t cr2_read : 1;
			uint32_t cr3_read : 1;
			uint32_t cr4_read : 1;
			uint32_t cr5_read : 1;
			uint32_t cr6_read : 1;
			uint32_t cr7_read : 1;
			uint32_t cr8_read : 1;
			uint32_t cr9_read : 1;
			uint32_t cr10_read : 1;
			uint32_t cr11_read : 1;
			uint32_t cr12_read : 1;
			uint32_t cr13_read : 1;
			uint32_t cr14_read : 1;
			uint32_t cr15_read : 1;
			uint32_t cr0_write : 1;
			uint32_t cr1_write : 1;
			uint32_t cr2_write : 1;
			uint32_t cr3_write : 1;
			uint32_t cr4_write : 1;
			uint32_t cr5_write : 1;
			uint32_t cr6_write : 1;
			uint32_t cr7_write : 1;
			uint32_t cr8_write : 1;
			uint32_t cr9_write : 1;
			uint32_t cr10_write : 1;
			uint32_t cr11_write : 1;
			uint32_t cr12_write : 1;
			uint32_t cr13_write : 1;
			uint32_t cr14_write : 1;
			uint32_t cr15_write : 1;
		};
	};

	union
	{
		uint32_t value4;
		struct
		{
			uint32_t dr0_read : 1;
			uint32_t dr1_read : 1;
			uint32_t dr2_read : 1;
			uint32_t dr3_read : 1;
			uint32_t dr4_read : 1;
			uint32_t dr5_read : 1;
			uint32_t dr6_read : 1;
			uint32_t dr7_read : 1;
			uint32_t dr8_read : 1;
			uint32_t dr9_read : 1;
			uint32_t dr10_read : 1;
			uint32_t dr11_read : 1;
			uint32_t dr12_read : 1;
			uint32_t dr13_read : 1;
			uint32_t dr14_read : 1;
			uint32_t dr15_read : 1;
			uint32_t dr0_write : 1;
			uint32_t dr1_write : 1;
			uint32_t dr2_write : 1;
			uint32_t dr3_write : 1;
			uint32_t dr4_write : 1;
			uint32_t dr5_write : 1;
			uint32_t dr6_write : 1;
			uint32_t dr7_write : 1;
			uint32_t dr8_write : 1;
			uint32_t dr9_write : 1;
			uint32_t dr10_write : 1;
			uint32_t dr11_write : 1;
			uint32_t dr12_write : 1;
			uint32_t dr13_write : 1;
			uint32_t dr14_write : 1;
			uint32_t dr15_write : 1;
		};
	};

	union
	{
		uint32_t value8;
		struct
		{
			uint32_t exception_de : 1;
			uint32_t exception_db : 1;
			uint32_t exception_nmi : 1;
			uint32_t exception_bp : 1;
			uint32_t exception_of : 1;
			uint32_t exception_br : 1;
			uint32_t exception_ud : 1;
			uint32_t exception_nm : 1;
			uint32_t exception_df : 1;
			uint32_t exception_cso : 1;
			uint32_t exception_ts : 1;
			uint32_t exception_np : 1;
			uint32_t exception_ss : 1;
			uint32_t exception_gp : 1;
			uint32_t exception_pf : 1;
			uint32_t exception_reserved0 : 1;
			uint32_t exception_mf : 1;
			uint32_t exception_ac : 1;
			uint32_t exception_mc : 1;
			uint32_t exception_xf : 1;
			uint32_t exception_reserved1 : 1;
			uint32_t exception_cp : 1;
			uint32_t exception_reserved2 : 1;
			uint32_t exception_reserved3 : 1;
			uint32_t exception_reserved4 : 1;
			uint32_t exception_reserved5 : 1;
			uint32_t exception_reserved6 : 1;
			uint32_t exception_reserved7 : 1;
			uint32_t exception_hv : 1;
			uint32_t exception_vc : 1;
			uint32_t exception_sx : 1;
			uint32_t exception_reserved8 : 1;
		};
	};

	union
	{
		uint32_t valueC;
		struct {
			uint32_t intr : 1;
			uint32_t nmi : 1;
			uint32_t smi : 1;
			uint32_t init : 1;
			uint32_t vintr : 1;
			uint32_t cr0 : 1;
			uint32_t read_idtr : 1;
			uint32_t read_gdtr : 1;
			uint32_t read_ldtr : 1;
			uint32_t read_tr : 1;
			uint32_t write_idtr : 1;
			uint32_t write_gdtr : 1;
			uint32_t write_ldtr : 1;
			uint32_t write_tr : 1;
			uint32_t rdtsc : 1;
			uint32_t rdpmc : 1;
			uint32_t pushf : 1;
			uint32_t popf : 1;
			uint32_t cpuid : 1;
			uint32_t rsm : 1;
			uint32_t iret : 1;
			uint32_t intn : 1;
			uint32_t invd : 1;
			uint32_t pause : 1;
			uint32_t hlt : 1;
			uint32_t invlpg : 1;
			uint32_t invlpga : 1;
			uint32_t ioio_prot : 1;
			uint32_t msr_prot : 1;
			uint32_t task_switch : 1;
			uint32_t ferr_freeze : 1;
			uint32_t shutdown : 1;
		};
	};

	union
	{
		uint32_t value10;
		struct
		{
			uint32_t vmrun : 1;
			uint32_t vmmcall : 1;
			uint32_t vmload : 1;
			uint32_t vmsave : 1;
			uint32_t stgi : 1;
			uint32_t clgi : 1;
			uint32_t skinit : 1;
			uint32_t rdtscp : 1;
			uint32_t icebp : 1;
			uint32_t wbinvd_wbnoinvd : 1;
			uint32_t monitor_monitor_x : 1;
			uint32_t mwait_mwait_x_uncond : 1;
			uint32_t mwait_mwait_x_cond : 1;
			uint32_t xsetbv : 1;
			uint32_t rdpru : 1;
			uint32_t efer_write_trap : 1;
			uint32_t cr0_write_trap : 1;
			uint32_t cr1_write_trap : 1;
			uint32_t cr2_write_trap : 1;
			uint32_t cr3_write_trap : 1;
			uint32_t cr4_write_trap : 1;
			uint32_t cr5_write_trap : 1;
			uint32_t cr6_write_trap : 1;
			uint32_t cr7_write_trap : 1;
			uint32_t cr8_write_trap : 1;
			uint32_t cr9_write_trap : 1;
			uint32_t cr10_write_trap : 1;
			uint32_t cr11_write_trap : 1;
			uint32_t cr12_write_trap : 1;
			uint32_t cr13_write_trap : 1;
			uint32_t cr14_write_trap : 1;
			uint32_t cr15_write_trap : 1;
		};
	};

	union
	{
		uint32_t value14;
		struct
		{
			uint32_t invlpgb : 1;
			uint32_t illegal_invlpgb : 1;
			uint32_t invpcid : 1;
			uint32_t mcommit : 1;
			uint32_t tlbsync : 1;
			uint32_t bus_lock : 1;
			uint32_t reserved0 : 26;
		};
	};

	uint8_t reserved1[36];
	uint16_t pause_filter_threshold;
	uint16_t pause_filter_count;
	uint64_t iopm_base_pa;
	uint64_t msrpm_base_pa;
	uint64_t tsc_offset;

	union
	{
		uint64_t value58;
		struct
		{
			uint64_t guest_asid : 32;
			tlb_control_id tlb_control : 8;
			uint64_t reserved2 : 24;
		};
	};

	union
	{
		uint64_t value60;
		struct 
		{
			uint64_t v_tpr : 8;
			uint64_t v_irq : 1;
			uint64_t v_gif : 1;
			uint64_t v_nmi : 1;
			uint64_t v_nmi_mask : 1;
			uint64_t reserved3 : 3;
			uint64_t v_intr_prio : 4;
			uint64_t v_ign_tpr : 1;
			uint64_t reserved4 : 3;
			uint64_t v_intr_masking : 1;
			uint64_t v_gif_enable : 1;
			uint64_t v_nmi_enable : 1;
			uint64_t reserved5 : 3;
			uint64_t x2avic_enable : 1;
			uint64_t avic_enable : 1;
			uint64_t v_intr_vector : 8;
			uint64_t reserved6 : 24;
		};
	};

	union
	{
		uint64_t value68;
		struct
		{
			uint64_t interrupt_shadow : 1;
			uint64_t guest_interrupt_mask : 1;
			uint64_t reserved7 : 62;
		};
	};

	vmexit_code exit_code;
	exit_info1 exit_info1;
	uint64_t exit_info2;
	exit_int_info exit_int_info;

	union
	{
		uint64_t value90;
		struct
		{
			uint64_t np_enable : 1;
			uint64_t secure_vm : 1;
			uint64_t encrypted_vm : 1;
			uint64_t gme : 1;
			uint64_t ssschecken : 1;
			uint64_t vte : 1;
			uint64_t readonly_gpt : 1;
			uint64_t invlpgb_tlbsync : 1;
			uint64_t reserved8 : 56;
		};
	};

	union
	{
		uint64_t value98;
		struct
		{
			uint64_t avic_apic_bar : 52;
			uint64_t reserved9 : 12;
		};
	};

	uint64_t guest_pa_of_ghcb;
	event_injection event_injection;
	uint64_t ncr3;

	union
	{
		uint64_t valueB8;
		struct
		{
			uint64_t lbr_virtualization_enable : 1;
			uint64_t virtualized_vmsave_vmload : 1;
			uint64_t virtualized_instruction_based_sampling : 1;
			uint64_t reserved10 : 61;
		};
	};

	union
	{
		uint64_t valueC0;
		struct
		{
			uint64_t i : 1;
			uint64_t iopm : 1;
			uint64_t asid : 1;
			uint64_t tpr : 1;
			uint64_t np : 1;
			uint64_t crx : 1;
			uint64_t drx : 1;
			uint64_t dt : 1;
			uint64_t seg : 1;
			uint64_t cr2 : 1;
			uint64_t lbr : 1;
			uint64_t avic : 1;
			uint64_t cet : 1;
			uint64_t reserved11 : 19;
			uint64_t reserved12 : 32;
		};
	};

	uint64_t nrip;
	uint8_t number_of_bytes_fetched;
	uint8_t guest_instructions[15];

	union
	{
		uint64_t valueE0;
		struct
		{
			uint64_t avic_apic_backing_page_pointer : 52;
			uint64_t reserved13 : 12;
		};
	};

	uint64_t reserved14;

	union
	{
		uint64_t valueF0;
		struct
		{
			uint64_t reserved15 : 12;
			uint64_t avic_logical_table_pointer : 40;
			uint64_t reserved16 : 12;
		};
	};

	union
	{
		uint64_t valueF8;
		struct
		{
			uint64_t avic_physical_max_index : 8;
			uint64_t reserved17 : 4;
			uint64_t avic_physical_table_pointer : 40;
			uint64_t reserved18 : 12;
		};
	};

	uint64_t reserved19;

	union
	{
		uint64_t value108;
		struct
		{
			uint64_t reserved20 : 12;
			uint64_t vmsa_pointer : 40;
			uint64_t reserved21 : 12;
		};
	};

	uint64_t vmgexit_rax;
	uint64_t vmgexit_cpl;
	uint16_t bus_lock_threshold_counter;
	uint8_t reserved22[734];
};

struct state_save_area
{
	
	segment es;
	segment cs;
	segment ss;
	segment ds;
	segment fs;
	segment gs;
	segment gdtr;
	segment ldtr;
	segment idtr;
	segment tr;

	uint8_t reserved1[43];

	uint8_t cpl;

	uint32_t reserved2;

	efer efer;

	uint8_t reserved3[112];

	control_register cr4;
	control_register cr3;
	control_register cr0;
	uint64_t dr7;
	uint64_t dr6;
	rflags rflags;
	uint64_t rip;

	uint8_t reserved4[88];

	uint64_t rsp;
	scet scet;
	uint64_t ssp;
	uint64_t isst_addr;
	register_64 rax;
	star star;
	lstar lstar;
	cstar cstar;
	sfmask sfmask;
	uint64_t kernel_gs_base;
	uint64_t sysenter_cs;
	uint64_t sysenter_esp;
	uint64_t sysenter_eip;
	control_register cr2;

	uint8_t reserved5[32];

	uint64_t g_pat;
	uint64_t dbg_ctl;
	uint64_t br_from;
	uint64_t br_to;
	uint64_t last_excp_from;
	uint64_t last_excp_to;
	uint64_t debug_extn_ctl;

	uint8_t reserved6[72];

	uint64_t spec_ctrl;

	uint8_t reserved7[904];

	uint8_t lbr_stack_from[128];
	uint8_t lbr_stack_to[128];
	uint64_t lbr_select;
	uint64_t ibs_fetch_ctl;
	uint64_t ibs_fetch_linear_addr;
	uint64_t ibs_op_ctl;
	uint64_t ibs_op_rip;
	uint64_t ibs_op_data1;
	uint64_t ibs_op_data2;
	uint64_t ibs_op_data3;
	uint64_t ibs_dc_linear_addr;
	uint64_t bp_ibstgt_rip;
	uint64_t ic_ibs_extd_ctl;

	uint8_t reserved8[1072];
};

struct vmcb
{
	control_area control;
	state_save_area state_save;
};