#pragma once
#include <ntifs.h>
#include <ntddk.h>
#include "hyper_helpers.h"
#include "nt.h"
#include "paging.h"
#include "svm.h"
#include "utility.h"

extern "C" uint64_t __fastcall PsGetProcessSectionBaseAddress(uint64_t a1); 

bool MiCheckPhysicalAddressRange(uint64_t physical_address, uint64_t size)
{
    unsigned __int64 v2;

    static uint64_t KiMtrrMaskBase = 0; 

    if (!KiMtrrMaskBase)
    {
        cpuid cpuid = { }; 
        __cpuid(cpuid.regs, 0x80000008);

        uint64_t KiMtrrMaxRangeShift = cpuid.registers.eax; 
        KiMtrrMaskBase = ((1i64 << KiMtrrMaxRangeShift) - 1) & 0xFFFFFFFFFFFFF000;
    }

    if (physical_address + size <= physical_address)
        return false; 

    v2 = physical_address + size - 1;

    if (KiMtrrMaskBase)
    {
        if ((v2 & 0xFFFFFFFFFFFFF000ui64) > KiMtrrMaskBase)
            return false; 
    }
    
	uint64_t pcrb = __readgsqword(0x20);
    uint8_t cpu_vendor = *reinterpret_cast<uint8_t*>(pcrb + 0x8d); 

    if (cpu_vendor == 1 && v2 >= 0xFD00000000 && physical_address <= 0xFFFFFFFFFF)
        return false; 

    return true; 
}

uint64_t guest_va_to_guest_pa(control_register guest_cr3, uint64_t guest_va)
{
    uint64_t temp = 0;
    return guest_va_to_guest_pa(guest_cr3, guest_va, temp);
}

uint64_t guest_va_to_guest_pa(control_register guest_cr3, uint64_t guest_va, uint64_t& offset_to_next_page)
{
    constexpr uint64_t huge_page_size = 0x40000000; // 1 GB
    constexpr uint64_t large_page_size = 0x200000;   // 2 MB

    uint64_t cr3_pa = guest_cr3.c3.pml4 << page_4kb_shift;

    if (!MiCheckPhysicalAddressRange(cr3_pa, PAGE_SIZE))
        return 0;

    virtual_address va = { .value = guest_va };
    uint64_t base = svm::hpt->identity_mapped_va;

    page_map_level_4* pml4e_table = reinterpret_cast<page_map_level_4*>(base + cr3_pa);
    page_map_level_4& pml4e = pml4e_table[va.pml4e_index];

    if (!pml4e.present)
        return 0;

    uint64_t pdpe_pa = pml4e.page_frame_number << page_4kb_shift;
    if (!MiCheckPhysicalAddressRange(pdpe_pa, PAGE_SIZE))
        return 0;

    page_directory_pointer* pdpe_table = reinterpret_cast<page_directory_pointer*>(base + pdpe_pa);
    page_directory_pointer& pdpe = pdpe_table[va.pdpe_index];

    if (!pdpe.present)
        return 0;

    if (pdpe.huge_page)
    {
        uint64_t final_pa = pdpe.huge.page_frame_number << page_1gb_shift;
        if (!MiCheckPhysicalAddressRange(final_pa, huge_page_size))
            return 0;

        offset_to_next_page = huge_page_size - va.huge_offset;
        return final_pa + va.huge_offset;
    }

    uint64_t pde_pa = pdpe.page_frame_number << page_4kb_shift;
    if (!MiCheckPhysicalAddressRange(pde_pa, PAGE_SIZE))
        return 0;

    page_directory* pde_table = reinterpret_cast<page_directory*>(base + pde_pa);
    page_directory& pde = pde_table[va.pde_index];

    if (!pde.present)
        return 0;

    if (pde.large_page)
    {
        uint64_t final_pa = pde.large.page_frame_number << page_2mb_shift;
        if (!MiCheckPhysicalAddressRange(final_pa, large_page_size))
            return 0;

        offset_to_next_page = large_page_size - va.large_offset;
        return final_pa + va.large_offset;
    }

    uint64_t pte_pa = pde.page_frame_number << page_4kb_shift;
    if (!MiCheckPhysicalAddressRange(pte_pa, PAGE_SIZE))
        return 0;

    page_table* pte_table = reinterpret_cast<page_table*>(base + pte_pa);
    page_table& pte = pte_table[va.pte_index];

    if (!pte.present)
        return 0;

    uint64_t final_pa = pte.page_frame_number << page_4kb_shift;
    if (!MiCheckPhysicalAddressRange(final_pa, PAGE_SIZE))
        return 0;

    offset_to_next_page = PAGE_SIZE - va.offset;
    return final_pa + va.offset;
}

void* guest_va_to_host_va(control_register guest_cr3, uint64_t guest_va)
{
    uint64_t guest_pa = guest_va_to_guest_pa(guest_cr3, guest_va);
    if (!guest_pa)
        return 0;

    return reinterpret_cast<void*>(svm::hpt->identity_mapped_va + guest_pa);
}

void* guest_va_to_host_va(control_register guest_cr3, uint64_t guest_va, uint64_t& offset_to_next_page)
{
    uint64_t guest_pa = guest_va_to_guest_pa(guest_cr3, guest_va, offset_to_next_page);
    if (!guest_pa)
        return 0;

    return reinterpret_cast<void*>(svm::hpt->identity_mapped_va + guest_pa);
}

uint64_t bruteforce_cr3(uint64_t eprocess)
{
    //Make a copy
    PPHYSICAL_MEMORY_RUN physical_range = *svm::physical_memory_block;

    static uint64_t self_ref_index = get_self_ref_index(); 

    virtual_address cr3_va = { .unused = 0xFFFF }; //Canonical address moment
    cr3_va.pml4e_index = self_ref_index;
    cr3_va.pdpe_index = self_ref_index;
    cr3_va.pde_index = self_ref_index;
    cr3_va.pte_index = self_ref_index;
    cr3_va.offset = self_ref_index * 8;

    for (; ; physical_range++)
    {
        uint64_t pfn = physical_range->BasePage; 
        uint64_t page_count = physical_range->PageCount;

        if (!pfn && !page_count)
            break;

        for (uint64_t i = 0; i < page_count; i++, pfn++)
        {
            MMPFN mmpfn_entry = static_cast<PMMPFN>(svm::pfn_database)[pfn];

            if (mmpfn_entry.flags == 1 || mmpfn_entry.pte_address != cr3_va.value)
                continue;

            uint64_t decrypted_eprocess = ((mmpfn_entry.flags | 0xF000000000000000) >> 0xd) | 0xFFFF000000000000;

            if (decrypted_eprocess == eprocess)
                return pfn << page_4kb_shift;
        }
    }

    return 0;
}


hypercall_status get_eprocess(void* user_buffer)
{
    auto* process = static_cast<get_eprocess_info*>(user_buffer);

    if (!process->id)
        return hypercall_status::invalid_userbuffer_info;

    uint64_t system_process = reinterpret_cast<uint64_t>(PsInitialSystemProcess);
    PLIST_ENTRY active_process_link = reinterpret_cast<PLIST_ENTRY>(system_process + 0x448);

    for (PLIST_ENTRY entry = active_process_link; entry->Flink != active_process_link; entry = entry->Flink)
    {
        uint64_t eprocess = reinterpret_cast<uint64_t>(entry) - 0x448;

        HANDLE id = PsGetProcessId(reinterpret_cast<PEPROCESS>(eprocess)); 

        uint64_t unique_process_id = reinterpret_cast<uint64_t>(id);
        if (unique_process_id == process->id)
        {
            process->eprocess = eprocess;
            return hypercall_status::success;
        }
    }

    return hypercall_status::failed_eprocess_search;
}

hypercall_status get_cr3(void* user_buffer)
{
    auto* process = static_cast<get_cr3_info*>(user_buffer);

    if (!process->eprocess)
        return hypercall_status::invalid_userbuffer_info;

    uint64_t cr3 = bruteforce_cr3(process->eprocess);

    if (!cr3)
        return hypercall_status::failed_cr3_bruteforce;

    process->cr3 = cr3;

    return hypercall_status::success;
}

hypercall_status get_section_base(void* user_buffer)
{
	auto* section_base = static_cast<get_section_base_info*>(user_buffer);

    if (!section_base->eprocess)
        return hypercall_status::invalid_userbuffer_info;

	section_base->address = PsGetProcessSectionBaseAddress(section_base->eprocess);

    return hypercall_status::success; 
}

hypercall_status read_virtual_memory(control_register& guest_cr3, void* user_buffer)
{
    auto* read = static_cast<read_virtual_memory_info*>(user_buffer);

    if (!read->cr3 || !read->destination || !read->source || !read->size)
        return hypercall_status::invalid_userbuffer_info;

    uint64_t bytes_read = 0;
    while (bytes_read < read->size)
    {
        uint64_t source_remaining = 0; uint64_t destination_remaining = 0;

        void* source_hva = guest_va_to_host_va({ .value = read->cr3 }, read->source + bytes_read, source_remaining);
        if (!source_hva)
            return hypercall_status::failed_gva_translation;

        void* destination_hva = guest_va_to_host_va(guest_cr3, read->destination + bytes_read, destination_remaining);
        if (!destination_hva)
            return hypercall_status::failed_gva_translation;

        uint64_t bytes_to_read = min(min(source_remaining, destination_remaining), read->size - bytes_read);
        memcpy(destination_hva, source_hva, bytes_to_read);
        bytes_read += bytes_to_read;
    }

    return hypercall_status::success;
}

hypercall_status write_virtual_memory(control_register& guest_cr3, void* user_buffer)
{
    auto* write = static_cast<write_virtual_memory_info*>(user_buffer);

    if (!write->cr3 || !write->destination || !write->source || !write->size)
        return hypercall_status::invalid_userbuffer_info;

    uint64_t bytes_written = 0;
    while (bytes_written < write->size)
    {
        uint64_t source_remaining = 0, destination_remaining = 0;

        void* source_hva = guest_va_to_host_va(guest_cr3, write->source + bytes_written, source_remaining);
        if (!source_hva)
            return hypercall_status::failed_gva_translation;

        void* destination_hva = guest_va_to_host_va({ .value = write->cr3 }, write->destination + bytes_written, destination_remaining);
        if (!destination_hva)
            return hypercall_status::failed_gva_translation;

        uint64_t bytes_to_write = min(min(source_remaining, destination_remaining), write->size - bytes_written);
        memcpy(destination_hva, source_hva, bytes_to_write);

        bytes_written += bytes_to_write;
    }

    return hypercall_status::success;
}

hypercall_status read_physical_memory(control_register& guest_cr3, void* user_buffer)
{
    auto* read = static_cast<read_physical_memory_info*>(user_buffer);

    uint64_t bytes_read = 0;
    while (bytes_read < read->size)
    {
        uint64_t destination_remaining = 0;

        uint64_t source_hva = svm::hpt->identity_mapped_va + read->source + bytes_read;

        void* destination_hva = guest_va_to_host_va(guest_cr3, read->destination + bytes_read, destination_remaining);
        if (!destination_hva)
            return hypercall_status::failed_gva_translation;

        uint64_t bytes_to_read = min(destination_remaining, read->size - bytes_read);
        memcpy(destination_hva, reinterpret_cast<void*>(source_hva), bytes_to_read);
        bytes_read += bytes_to_read;
    }

    return hypercall_status::success;
}

hypercall_status write_physical_memory(control_register& guest_cr3, void* user_buffer)
{
    auto* write = static_cast<write_physical_memory_info*>(user_buffer);

    uint64_t bytes_written = 0;
    while (bytes_written < write->size)
    {
        uint64_t source_remaining = 0;

        void* source_hva = guest_va_to_host_va(guest_cr3, write->source + bytes_written, source_remaining);
        if (!source_hva)
            return hypercall_status::failed_gva_translation;

        uint64_t destination_hva = svm::hpt->identity_mapped_va + write->destination + bytes_written;

        uint64_t bytes_to_write = min(source_remaining, write->size - bytes_written);
        memcpy(reinterpret_cast<void*>(destination_hva), source_hva, bytes_to_write);

        bytes_written += bytes_to_write;
    }

    return hypercall_status::success;
}

hypercall_status get_physical_address(void* user_buffer)
{
    auto* translation = static_cast<get_physical_address_info*>(user_buffer);

    if (!translation->cr3 || !translation->virtual_address)
        return hypercall_status::invalid_userbuffer_info;

    uint64_t physical_address = guest_va_to_guest_pa({ .value = translation->cr3 }, translation->virtual_address);
    if (!physical_address)
        return hypercall_status::failed_gva_translation;

    translation->physical_address = physical_address;

    return hypercall_status::success;
}

hypercall_status hide_virtual_memory(vcore* vcore, void* user_buffer)
{
    auto* hide = static_cast<hide_virtual_memory_info*>(user_buffer);

	if (!hide->cr3 || !hide->virtual_address || !hide->size)
		return hypercall_status::invalid_userbuffer_info;

    uint64_t page_count = (hide->size + PAGE_SIZE - 1) / PAGE_SIZE;

    for (uint64_t i = 0; i < page_count; i++)
    {
		uint64_t physical_address = guest_va_to_guest_pa({ .value = hide->cr3 }, hide->virtual_address + (i * PAGE_SIZE));

        if (!physical_address)
            return hypercall_status::failed_gva_translation; 

		page_table* pte = svm::npt->get_pte(svm::hpt->identity_mapped_va, physical_address);

        if (!pte)
            return hypercall_status::failed_npt_translation; 

		if (!svm::npt->hide_page(pte))
			return hypercall_status::failed_hide_page;

    }

    vcore->guest_vmcb.control.tlb_control = tlb_control_id::flush_guest_tlb; 

    return hypercall_status::success; 
}

hypercall_status hide_physical_page(vcore* vcore, void* user_buffer)
{
	auto* hide = static_cast<hide_physical_page_info*>(user_buffer);
    if(!hide->physical_address)
        return hypercall_status::invalid_userbuffer_info;

    page_table* pte = svm::npt->get_pte(svm::hpt->identity_mapped_va, hide->physical_address);

    if (!pte)
        return hypercall_status::failed_npt_translation;

    vcore->guest_vmcb.control.tlb_control = tlb_control_id::flush_guest_tlb;

	return svm::npt->hide_page(pte) ? hypercall_status::success : hypercall_status::failed_hide_page;
}

hypercall_status leak_image_base(void* user_buffer)
{
    leak_info* leak = reinterpret_cast<leak_info*>(user_buffer);

    uint64_t return_address = reinterpret_cast<uint64_t>(_ReturnAddress());

    virtual_address va = { .value = return_address }; 

	return_address -= va.offset; //Page align it back to the start of the page

    //Search the previous 2MB of a random .text address in hopes to find the ajvisor_signature at the start of the .text section

    int32_t page_count = 0x200000 / PAGE_SIZE; 

    for (int i = 0; i < page_count; i++)
    {
        uint64_t address = return_address - (i * 0x1000); 
        
		int signature = *reinterpret_cast<int*>(address);

		if (signature == 0x6969) //AJVisor signature
		{
			leak->hv_image_base = address - 0x1000; //0x1000 to account for the PE Header, remove if mapper/loader does not allocate memory for PE Headers
			return hypercall_status::success;
		}
    }

    return hypercall_status::failed_to_leak; 
}

hypercall_status flush_logs(vcore* vcore, void* user_buffer)
{
    flush_logs_info* output_logs = static_cast<flush_logs_info*>(user_buffer); 

    if (!output_logs->buffer || !output_logs->size)
        return hypercall_status::invalid_userbuffer_info; 

    uint8_t* buffer = static_cast<uint8_t*>(logger::flush()); 
 
    uint64_t bytes_copied = 0; 
    while (bytes_copied < output_logs->size)
    {
        uint64_t bytes_remaining = 0; 

        void* buffer_hva = guest_va_to_host_va(vcore->guest_vmcb.state_save.cr3, reinterpret_cast<uint64_t>(output_logs->buffer) + bytes_copied, bytes_remaining); 

        uint64_t bytes_to_copy = min(bytes_remaining, output_logs->size - bytes_copied); 
        memcpy(buffer_hva, buffer + bytes_copied, bytes_to_copy);

        bytes_copied += bytes_to_copy;
    }

    return hypercall_status::success; 
}