#pragma once
#include <cstdint>

#pragma warning (disable : 4201)

enum class hypercall_status : uint64_t
{
    success = 0,
    invalid_userbuffer_info = 1, // Information in the usermode buffer is invalid.
    failed_eprocess_search = 2, // Failed to get an eprocess.
    failed_gva_translation = 3, // Failed to translate a guest virtual address to a physical address or host virtual address.
    failed_cr3_bruteforce = 4, // Failed to brute-force the cr3 of a process.
    failed_npt_translation = 5, // Faled to get an npt entry from a physical address
    failed_hide_page = 6, //Failed to hide a page
    failed_to_leak = 7,
};

enum class hypercall_code : uint32_t
{
    magic = 0x672615,
    get_eprocess,
    get_cr3,
    read_virtual_memory,
    write_virtual_memory,
    read_physical_memory,
    write_physical_memory,
    get_physical_address,
    hide_virtual_memory,
    hide_physical_page,
    unhide_virtual_memory,
    unhide_physical_page,
    leak_image_base,
    get_section_base,
    flush_logs,
    ping,
    stop_hypervisor,
};

union hypercall_request
{
    uint64_t value;
    struct
    {
        hypercall_code code;
        hypercall_code magic;
    };
};

struct get_eprocess_info
{
    uint32_t id;
    uint64_t eprocess;
};

struct get_cr3_info
{
    uint64_t eprocess;
    uint64_t cr3;
};

struct read_virtual_memory_info
{
    uint64_t cr3;
    uint64_t destination;
    uint64_t source;
    uint64_t size;
};

struct write_virtual_memory_info
{
    uint64_t cr3;
    uint64_t destination;
    uint64_t source;
    uint64_t size;
};

struct read_physical_memory_info
{
    uint64_t destination;
    uint64_t source;
    uint64_t size;
};

struct write_physical_memory_info
{
    uint64_t destination;
    uint64_t source;
    uint64_t size;
};

struct get_physical_address_info
{
    uint64_t cr3;
    uint64_t virtual_address;
    uint64_t physical_address;
};

struct hide_virtual_memory_info
{
    uint64_t cr3;
    uint64_t virtual_address;
    uint64_t size;
};

struct hide_physical_page_info
{
    uint64_t physical_address;
};

struct unhide_virtual_memory_info
{
    uint64_t cr3;
    uint64_t virtual_address;
    uint64_t size;
};

struct unhide_physical_page_info
{
    uint64_t physical_address;
};

struct leak_info
{
    uint64_t hv_image_base;
};

struct get_section_base_info
{
    uint64_t eprocess;
    uint64_t address;
};

struct flush_logs_info
{
    void* buffer;
    uint64_t size;
};

struct ping_info
{
    hypercall_code response;
};