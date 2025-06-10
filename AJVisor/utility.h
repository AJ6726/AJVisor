#pragma once
#include <ntddk.h>
#include "types.h"
#include "pe.h"
#include "nt.h"
#include "paging.h"

uint64_t get_self_ref_index(); 
page_table* get_pte_address(uint64_t address); 
uint64_t get_empty_pml4_index(); 
uint64_t get_kernel_module(const char* module_name);
uint64_t get_module_export(const char* module_name, const char* function_name); 
uint64_t get_module_export(uint64_t module_base, const char* function_name);
PIMAGE_SECTION_HEADER get_image_section(uint64_t image_base, const char* section_name);

template <typename Fn, typename... Arguments>
void call_on_all_cores(Fn callback, Arguments... arguments)
{
    uint32_t core_count = KeQueryActiveProcessorCount(nullptr);
    for (uint32_t i = 0; i < core_count; i++)
    {
        KAFFINITY original_affinity = KeSetSystemAffinityThreadEx(1ll << i); 

        callback(i, arguments...); 

        KeRevertToUserAffinityThreadEx(original_affinity);
    }
}