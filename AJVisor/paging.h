#pragma once
#include "types.h"

#pragma warning (disable : 4201)

enum page_shifts
{
    page_4kb_shift = 12,
    page_2mb_shift = 21,
    page_1gb_shift = 30,
};

union virtual_address
{
    uint64_t value;

    struct
    {
        uint64_t offset : 12;
        uint64_t pte_index : 9;
        uint64_t pde_index : 9;
        uint64_t pdpe_index : 9;
        uint64_t pml4e_index : 9;
        uint64_t unused : 16;
    };

    struct
    {
        uint64_t large_offset : 21;
    };

    struct
    {
        uint64_t huge_offset : 30;
    };
};

union page_map_level_4
{
    uint64_t value;

    struct
    {
        uint64_t present : 1;
        uint64_t write : 1;
        uint64_t usermode : 1;
        uint64_t page_write_through : 1;
        uint64_t page_cache_disable : 1;
        uint64_t accessed : 1;
        uint64_t ignored0 : 1;
        uint64_t reserved0 : 1;
        uint64_t ignored1 : 4;
        uint64_t page_frame_number : 40;
        uint64_t ignored2 : 11;
        uint64_t no_execute : 1;
    };
};

union page_directory_pointer
{
    uint64_t value;

    struct
    {
        uint64_t present : 1;
        uint64_t write : 1;
        uint64_t usermode : 1;
        uint64_t page_write_through : 1;
        uint64_t page_cache_disable : 1;
        uint64_t accessed : 1;
        uint64_t ignored0 : 1;
        uint64_t huge_page : 1;
        uint64_t ignored1 : 4;
        uint64_t page_frame_number : 40;
        uint64_t ignored2 : 11;
        uint64_t no_execute : 1;
    };

    struct
    {
        uint64_t present : 1;
        uint64_t write : 1;
        uint64_t usermode : 1;
        uint64_t page_write_through : 1;
        uint64_t page_cache_disable : 1;
        uint64_t accessed : 1;
        uint64_t dirty : 1;
        uint64_t huge_page : 1;
        uint64_t global : 1;
        uint64_t ignored0 : 3;
        uint64_t page_attribute_table : 1;
        uint64_t reserved0 : 17;
        uint64_t page_frame_number : 22;
        uint64_t ignored1 : 7;
        uint64_t protection_key : 4;
        uint64_t no_execute : 1;
    }huge;
};

union page_directory
{
    uint64_t value;

    struct
    {
        uint64_t present : 1;
        uint64_t write : 1;
        uint64_t usermode : 1;
        uint64_t page_write_through : 1;
        uint64_t page_cache_disable : 1;
        uint64_t accessed : 1;
        uint64_t ignored0 : 1;
        uint64_t large_page : 1;
        uint64_t ignored1 : 4;
        uint64_t page_frame_number : 38;
        uint64_t reserved0 : 2;
        uint64_t ignored2 : 11;
        uint64_t no_execute : 1;
    };

    struct
    {
        uint64_t present : 1;
        uint64_t write : 1;
        uint64_t usermode : 1;
        uint64_t page_write_through : 1;
        uint64_t page_cache_disable : 1;
        uint64_t accessed : 1;
        uint64_t dirty : 1;
        uint64_t large_page : 1;
        uint64_t global : 1;
        uint64_t ignored0 : 3;
        uint64_t page_attribute_table : 1;
        uint64_t reserved0 : 8;
        uint64_t page_frame_number : 31;
        //uint64_t reserved1 : 2;
        uint64_t ignored1 : 7;
        uint64_t protection_key : 4;
        uint64_t no_execute : 1;
    } large;
};

union page_table
{
    uint64_t value;

    struct
    {
        uint64_t present : 1;
        uint64_t write : 1;
        uint64_t usermode : 1;
        uint64_t page_write_through : 1;
        uint64_t page_cache_disable : 1;
        uint64_t accessed : 1;
        uint64_t dirty : 1;
        uint64_t page_attribute_table : 1;
        uint64_t global : 1;
        uint64_t ignored0 : 3;
        uint64_t page_frame_number : 38;
        uint64_t reserved0 : 2;
        uint64_t ignored1 : 7;
        uint64_t protection_key : 4;
        uint64_t no_execute : 1;
    };
};
