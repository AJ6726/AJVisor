#pragma once
#include <intrin.h>
#include "paging.h"
#include "wrappers.h"
#include "utility.h"
#include "logger.h"

struct host_page_table
{
    alignas(0x1000) page_map_level_4 pml4[512];
    alignas(0x1000) page_directory_pointer pdp[512];

	//For the identity map
	uint64_t identity_pml4_index;
	uint64_t identity_mapped_va;
};

struct nested_page_table
{
    static constexpr uint64_t max_free_page_count = 100;

    alignas(0x1000) page_map_level_4 pml4[512];
    alignas(0x1000) page_directory_pointer pdp[512];

    alignas(0x1000) uint64_t free_pages[max_free_page_count][512];

    uint64_t free_page_pa[max_free_page_count];
 
    volatile int64_t free_pages_used;

    page_map_level_4* get_pml4e(uint64_t physical_address)
    {
        virtual_address va = { .value = physical_address };

        return pml4 + va.pml4e_index;
    }

    page_directory_pointer* get_pdpe(uint64_t physical_address)
    {
        virtual_address va = { .value = physical_address };

        return pdp + va.pdpe_index;
    }

    bool split_pdpe(page_directory_pointer* pdpe)
    {
        if (free_pages_used > max_free_page_count)
        {
            logger::write("[HV] Failed to split a page directory pointer entry - insufficient free pages");
            return false;
        }

        page_directory* pde_table = reinterpret_cast<page_directory*>(free_pages[free_pages_used]);
        uint64_t pde_table_pa = free_page_pa[free_pages_used];

        _InterlockedIncrement64(&free_pages_used);

		//Split the huge page (1GB) into 512 2mb pages (512 Large PDE Entries)

        for (int i = 0; i < 512; i++)
        {
            auto& pde = pde_table[i];
            pde.large_page = 1;
            pde.large.present = pdpe->huge.present;
            pde.large.write = pdpe->huge.write;
            pde.large.global = pdpe->huge.global;
            pde.large.no_execute = pdpe->huge.no_execute;
            pde.large.dirty = pdpe->huge.dirty;
            pde.large.accessed = pdpe->huge.accessed;
            pde.large.protection_key = pdpe->huge.protection_key;
            pde.large.page_attribute_table = pdpe->huge.page_attribute_table;
            pde.large.page_cache_disable = pdpe->huge.page_cache_disable;
            pde.large.usermode = pdpe->huge.usermode;
            pde.large.page_frame_number = (pdpe->huge.page_frame_number << 9) + i;
        }

        pdpe->page_frame_number = pde_table_pa >> page_4kb_shift;
        pdpe->huge_page = 0;

        return true; 
    }

    page_directory* get_pde(uint64_t identity_mapped_va, uint64_t physical_address)
    {
        virtual_address va = { .value = physical_address };

		page_directory_pointer* pdpe = get_pdpe(physical_address);

        if (pdpe->huge_page)
            if (!split_pdpe(pdpe))
                return nullptr; 

        page_directory* pde_table = reinterpret_cast<page_directory*>(identity_mapped_va + (pdpe->page_frame_number << page_4kb_shift));

        return pde_table + va.pde_index;
    }

    page_table* get_pte(uint64_t identity_mapped_va, uint64_t physical_address)
    {
        virtual_address va = { .value = physical_address };
        page_directory* pde = get_pde(identity_mapped_va, physical_address);

        if (!pde)
            return nullptr; 

        if (pde->large_page)
            if (!split_pde(pde))
                return nullptr; 

        page_table* pte_table = reinterpret_cast<page_table*>(identity_mapped_va + (pde->page_frame_number << page_4kb_shift));

        return pte_table + va.pte_index;
    }

    bool split_pde(page_directory* pde)
    {
        if (free_pages_used >= max_free_page_count)
        {
            logger::write("[HV] Failed to split a page directory entry - insufficient free pages"); 
            return false;
        }

        page_table* pte_table = reinterpret_cast<page_table*>(free_pages[free_pages_used]);
        uint64_t pte_table_pa = free_page_pa[free_pages_used];

        _InterlockedIncrement64(&free_pages_used);

		//Split the large page (2MB) into 512 4kb pages (512 PTE Entries)
        for (int i = 0; i < 512; i++)
        {
            auto& pte = pte_table[i];
            pte.present = pde->large.present;
            pte.write = pde->large.write;
            pte.global = pde->large.global;
            pte.no_execute = pde->large.no_execute;
            pte.dirty = pde->large.dirty;
            pte.accessed = pde->large.accessed;
            pte.protection_key = pde->large.protection_key;
            pte.page_attribute_table = pde->large.page_attribute_table;
            pte.page_cache_disable = pde->large.page_cache_disable;
            pte.usermode = pde->large.usermode;
            pte.page_frame_number = (pde->large.page_frame_number << 9) + i;
        }

        pde->page_frame_number = pte_table_pa >> page_4kb_shift;
        pde->large_page = 0;

        return true;
    }

    bool hide_page(page_table* pte)
    {
        if (free_pages_used >= max_free_page_count)
        {
            logger::write("[HV] Failed to hide page 0x%llX - insufficient free pages", pte->page_frame_number << page_4kb_shift); 
            return false;
        }

        uint64_t physical_address = pte->page_frame_number << page_4kb_shift; 

		//Check if the page is already hidden
        for (uint64_t i = 0; i < max_free_page_count; i++)
            if (physical_address == free_page_pa[i])
                return true;

        uint64_t dummy_pa = free_page_pa[free_pages_used];

        pte->page_frame_number = dummy_pa >> page_4kb_shift; 

        _InterlockedIncrement64(&free_pages_used);

        return true; 
    }
}; 