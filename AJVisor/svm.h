#pragma once
#include <ntifs.h>
#include <ntddk.h>
#include "nt.h"
#include "page_tables.h"
#include "vcore.h"

namespace svm
{
	inline vcore* vcores = nullptr; 

	inline host_page_table* hpt = nullptr; 
	inline nested_page_table* npt = nullptr; 

	inline control_register system_cr3 = { }; 

	inline PPHYSICAL_MEMORY_RUN* physical_memory_block = nullptr; 
	inline PMMPFN pfn_database = nullptr; 

}