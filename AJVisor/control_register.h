#pragma once
#include "types.h"

union control_register
{
	uint64_t value;

	struct
	{
		uint64_t pe : 1;
		uint64_t mp : 1;
		uint64_t em : 1;
		uint64_t ts : 1;
		uint64_t et : 1;
		uint64_t ne : 1;
		uint64_t reserved0 : 10;
		uint64_t wp : 1;
		uint64_t reserved1 : 1;
		uint64_t am : 1;
		uint64_t reserved2 : 10;
		uint64_t nw : 1;
		uint64_t cd : 1;
		uint64_t pg : 1;
		uint64_t reserved3 : 32;
	}c0;

	struct
	{
		uint64_t pfla;
	}c2;

	struct
	{
		uint64_t reserved0 : 3;
		uint64_t pwt : 1;
		uint64_t pcd : 1;
		uint64_t reserved1 : 7;
		uint64_t pml4 : 40;
		uint64_t reserved2 : 12;
	}c3;

	struct
	{
		uint64_t pci : 12;
		uint64_t pml4 : 40;
		uint64_t reserved52 : 12;
	}c3_pcid_on;

	struct
	{
		uint64_t vme : 1;
		uint64_t pvi : 1;
		uint64_t tsd : 1;
		uint64_t de : 1;
		uint64_t pse : 1;
		uint64_t pae : 1;
		uint64_t mce : 1;
		uint64_t pge : 1;
		uint64_t pce : 1;
		uint64_t os_fxsr : 1;
		uint64_t os_xmm_excpt : 1;
		uint64_t umip : 1;
		uint64_t la57 : 1;
		uint64_t reserved0 : 3;
		uint64_t fs_gs_base : 1;
		uint64_t pcide : 1;
		uint64_t os_xsave : 1;
		uint64_t reserved1 : 1;
		uint64_t smep : 1;
		uint64_t smap : 1;
		uint64_t pke : 1;
		uint64_t cet : 1;
		uint64_t reserved2 : 40;
	}c4;

	struct
	{
		uint64_t tpr : 4;
		uint64_t reserved0 : 60;
	}c8;
};