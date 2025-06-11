### AJVisor
AJVisor is a lightwieght hypervisor for AMD cpus. The hypervisor is aimed towards memory read/write capabilities and evasion of hypervisor detections (althought very minimal detections are present/implemented for AMD Hypervisors). 
The hypervisor has minimal features and lacks debugging/tracing capabilities. 

### Features
-Message logging <br />
-Virtual and physical read/write to any process <br />
-Memory hiding <br />
-Bruteforce cr3 for any processes with encrypted cr3 <br />
-Msr Interception, currently only HSAVE and EFER are handled alongside synthethic/invalid msr accesses. <br />
-Self hiding, uses a compiler inserted signature in the .text section to trace the hypervisor image base. <br />
-Unloading/Devirtualization <br />
-Guest buffer passthrough to Hypercalls <br />

### Hypercalls
```c++
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
```

```c++
void handle_hypercall(vcore* vcore)
{
    state_save_area& guest_state_save = vcore->guest_vmcb.state_save;
    context& guest_context = vcore->guest_context;

    hypercall_request request = { .value = guest_context.rcx.value };

    if (request.magic != hypercall_code::magic)
    {
        inject_exception(vcore, exception_vector::invalid_opcode, false, true);
        return;
    }

    hypercall_status status = hypercall_status::success;

    // The guest usermode address space is not accessible to the VMM (World Switch).
    // Translate the guest buffer so it is accessible. The guest buffer may also hold other buffers that need to be translated.
    void* user_buffer = guest_va_to_host_va(guest_state_save.cr3, guest_context.rdx.value);
    if (!user_buffer)
    {
        status = hypercall_status::failed_gva_translation;
        guest_state_save.rax.value = static_cast<uint64_t>(status);
        return;
    }

    switch (request.code)
    {
      //Omitted for simplicity
    }
    guest_state_save.rax.value = static_cast<uint64_t>(status);
}

```

### Logging
A printf style logger that automically logs the time stamp. 

Example of how to log messages in the hypervisor
```c++
logger::write("[HV] Failed to hide page 0x%llX - insufficient free pages", pte->page_frame_number << page_4kb_shift); 
logger::write("[HV] Failed to split a page directory pointer entry - insufficient free pages");
```

Example of how to extract the logs from client (usermode)
```c++
hv::flush_logs()
```

### Client Interface
The AJVisor comes with the AJClient that interacts with the hypervisor (makes hypercalls). The client has a hv namespace that holds the functions to make hypercalls and recieve information. 
The process class is designed to be a wrapper for the hv namespace. This is designed to allow the client to use the hypervisor for multiple processes - for whatever reason one may need this. 

```c++
namespace hv
{
	uint64_t get_eprocess(uint32_t process_id);
	uint64_t get_cr3(uint64_t eprocess);

	//Attempts to hide itself, only works if the PE Headers of the Hypervisor is intact or zeroed out
	void hide(); 

	void read_virtual_memory(uint64_t cr3, uint64_t destination, uint64_t source, uint64_t size);
	void read_physical_memory(uint64_t destination, uint64_t source, uint64_t size);

	void write_virtual_memory(uint64_t cr3, uint64_t destination, uint64_t source, uint64_t size);
	void write_physical_memory(uint64_t destination, uint64_t source, uint64_t size);
	
	void hide_virtual_memory(uint64_t cr3, uint64_t address, uint64_t size); 
	void hide_physical_page(uint64_t address);

	void unhide_virtual_memory(uint64_t cr3, uint64_t address, uint64_t size);
	void unhide_physical_page(uint64_t address);

	uint64_t get_section_base(uint64_t eprocess);

	void flush_logs(); 
	uint64_t get_physical_address(uint64_t cr3, uint64_t address);

	bool is_running(); 
	void unload(); 

	hypercall_status hypercall(hypercall_code code, void* buffer, uint64_t buffer_size);
}
```

```c++
enum class address_type : uint32_t
{
	virt = 0,
	phys,
};

struct process
{
	uint32_t id;
	uint64_t eprocess;
	uint64_t cr3;
	uint64_t base;
	uint64_t peb; 

  //Methods are ommited for simplicity (They also look ugly :c)
};
```

### Compabilitiy
-Tested on Windows 10 24h2 and Windows 11 24h2. <br />
-Designed to work with manual mappers such as KDMapper. <br />
-Designed to be compiled with MSVC <br />

### Problem/Concerns
-Potential insuffient amount of memory mapped for the NPTs since only 512GB of memory is mapped, if the IOMMU attempts to read a physical address over 512GB the system will freeze/crash. <br />
-The cr3 bruteforce requires two patterns for the MmPfnDatabase and PhysicalMemoryBlock from ntoskrnl, and these patterns may break in future Window updates. <br />

### To Do
-Map 2TB of memory for the NPTs. <br />
-Add debugging and tracing features. <br />
-Error code to description output <br />
