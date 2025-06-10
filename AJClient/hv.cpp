#include <cstdarg>
#include <cstdio>
#include <Windows.h>
#include <vector>
#include "hv.h"
#include "paging.h"

extern "C"
{
    hypercall_status __vmmcall(uint64_t request, void* buffer);
}

void hv::hide()
{
    std::printf("[CLIENT] Attempting to hide Hypervisor - may result in system crash \n"); 

    Sleep(1000); 

    //Manually checked, if you add substantial amount of code/static data to AJVisor, change this. 
    constexpr uint64_t image_size = 0x8000; 

    leak_info leak = { };
    hypercall(hypercall_code::leak_image_base, &leak, sizeof(leak)); 

    if (!leak.hv_image_base)
    {
        std::printf("[CLIENT] Failed to find the HV image base, lucky your system didn't crash \n"); 
        return; 
    }

    std::printf("[CLIENT] HV image base %p \n", leak.hv_image_base);
    Sleep(1000); 

    //1AD000 is the system cr3
    hide_virtual_memory(0x1AD000, leak.hv_image_base, image_size);

	std::printf("[CLIENT] Sucessfully hidden the hypervisor \n");
}

uint64_t hv::get_eprocess(uint32_t process_id)
{
    get_eprocess_info process = { .id = process_id };

    hypercall(hypercall_code::get_eprocess, &process, sizeof(process));

    return process.eprocess;
}

uint64_t hv::get_cr3(uint64_t eprocess)
{
    get_cr3_info process = { .eprocess = eprocess };

    hypercall(hypercall_code::get_cr3, &process, sizeof(process));

    return process.cr3;
}

void hv::read_virtual_memory(uint64_t cr3, uint64_t destination, uint64_t source, uint64_t size)
{
    read_virtual_memory_info read = { .cr3 = cr3, .destination = destination, .source = source, .size = size };

    hypercall(hypercall_code::read_virtual_memory, &read, sizeof(read));
}

void hv::read_physical_memory(uint64_t destination, uint64_t source, uint64_t size)
{
    read_physical_memory_info read = { .destination = destination, .source = source, .size = size };

    hypercall(hypercall_code::read_physical_memory, &read, sizeof(read));
}

void hv::write_virtual_memory(uint64_t cr3, uint64_t destination, uint64_t source, uint64_t size)
{
    write_virtual_memory_info write = { .cr3 = cr3, .destination = destination, .source = source, .size = size };

    hypercall(hypercall_code::write_virtual_memory, &write, sizeof(write));
}

void hv::write_physical_memory(uint64_t destination, uint64_t source, uint64_t size)
{
    write_physical_memory_info write = { .destination = destination, .source = source, .size = size };

    hypercall(hypercall_code::write_physical_memory, &write, sizeof(write));
}

uint64_t hv::get_physical_address(uint64_t cr3, uint64_t address)
{
    get_physical_address_info translation = { .cr3 = cr3, .virtual_address = address };

    hypercall(hypercall_code::get_physical_address, &translation, sizeof(translation));

    return translation.physical_address;
}

void hv::hide_virtual_memory(uint64_t cr3, uint64_t address, uint64_t size)
{
	hide_virtual_memory_info hide = { .cr3 = cr3, .virtual_address = address, .size = size };

    hypercall(hypercall_code::hide_virtual_memory, &hide, sizeof(hide)); 
    
}

void hv::hide_physical_page(uint64_t address)
{
	hide_physical_page_info hide = { .physical_address = address };

	hypercall(hypercall_code::hide_physical_page, &hide, sizeof(hide));

}

void hv::unhide_virtual_memory(uint64_t cr3, uint64_t address, uint64_t size)
{
    unhide_virtual_memory_info unhide = { .cr3 = cr3, .virtual_address = address, .size = size };

    hypercall(hypercall_code::unhide_virtual_memory, &unhide, sizeof(unhide));

}

void hv::unhide_physical_page(uint64_t address)
{
	unhide_physical_page_info unhide = { .physical_address = address };

	hypercall(hypercall_code::unhide_physical_page, &unhide, sizeof(unhide));
}

uint64_t hv::get_section_base(uint64_t eprocess)
{
	get_section_base_info section_base = { .eprocess = eprocess };

	hypercall(hypercall_code::get_section_base, &section_base, sizeof(section_base));

    return section_base.address; 
}

void hv::flush_logs()
{
    std::vector<logger_message> logs(max_log_count); 

    flush_logs_info flushed_logs = { .buffer = logs.data(), .size = logs.size() * sizeof(logger_message)};

    hypercall(hypercall_code::flush_logs, &flushed_logs, sizeof(flushed_logs));

    for (auto& log : logs)
    {
        if (!log.tsc)
            continue; 

        std::printf("[LOG] Tsc: %llu %s \n", log.tsc, log.message);
    }
}

bool hv::is_running()
{
    ping_info ping = { };

	__try {
        hypercall(hypercall_code::ping, &ping, sizeof(ping));
        return ping.response == hypercall_code::magic;
	} __except (1) {}

    return false; 
}

void hv::unload()
{
    SYSTEM_INFO system_info = { };
    GetSystemInfo(&system_info);

    uint32_t core_count = system_info.dwNumberOfProcessors;

    uint64_t dummy = 0; 

    for (int i = 0; i < core_count; i++)
    {
        uint64_t original_affinity = SetThreadAffinityMask(GetCurrentThread(), 1ull << i);
        
        std::printf("[CLIENT] Devirtualizing Core %d \n", GetCurrentProcessorNumber()); 

        hypercall(hypercall_code::stop_hypervisor, &dummy, sizeof(dummy)); 

        SetThreadAffinityMask(GetCurrentThread(), original_affinity);

    }
}

bool verbose_debug = true;

hypercall_status hv::hypercall(hypercall_code code, void* buffer, uint64_t buffer_size)
{
    hypercall_request request = { .magic = hypercall_code::magic };
    request.code = code;

    void* user_buffer = buffer;

    virtual_address va = { .value = reinterpret_cast<uint64_t>(buffer) };

    // Check if the user buffer is split across a page, hypervisor won't handle split buffers
    bool over_page_boundary = (buffer_size + va.offset) >= 0x1000; 

    if (over_page_boundary)
    {
        // VirtualAlloc allocates on a page granularity
        user_buffer = VirtualAlloc(nullptr, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        memcpy(user_buffer, buffer, buffer_size);
    }

    hypercall_status status = __vmmcall(request.value, user_buffer);

    if (over_page_boundary)
    {
        memcpy(buffer, user_buffer, buffer_size);
        VirtualFree(user_buffer, 0, MEM_RELEASE);
    }

    if (verbose_debug && (status != hypercall_status::success))
        std::printf("[CLIENT] Failed Request %d Return Status %d \n", code, status);

    return status;
}