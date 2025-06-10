#include <ntddk.h>
#include <windef.h>
#include <intrin.h>
#include "control_register.h"
#include "utility.h"
#include "wrappers.h"

#pragma warning (disable : 4996)

uint64_t get_empty_pml4_index()
{
	control_register cr3 = { .value = __readcr3() };
	page_map_level_4* pml4 = get_virtual_address<page_map_level_4*>(cr3.c3.pml4 << page_4kb_shift);
	
	for (int i = 0; i < 512; i++)
		if ((!pml4[i].present) && (!pml4[i].page_frame_number))
			return i; 

	return 255; 
}

uint64_t get_self_ref_index()
{
	control_register cr3 = { .value = __readcr3() };
	page_map_level_4* pml4 = get_virtual_address<page_map_level_4*>(cr3.c3.pml4 << page_4kb_shift);

	for (int i = 255; i < 512; i++)
		if (pml4[i].page_frame_number == cr3.c3.pml4)
			return i;

	return 0; 
}

page_table* get_pte_address(uint64_t address)
{
	static uint64_t self_ref_index = get_self_ref_index(); 

	virtual_address pte_va = { .unused = 0xFFFF };
	pte_va.pml4e_index = self_ref_index; 

	return (page_table*)(((address >> 9) & 0x7FFFFFFFF8ull) + pte_va.value);
}

extern "C" NTSTATUS WINAPI ZwQuerySystemInformation(_SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);

uint64_t get_kernel_module(const char* module_name)
{
	ULONG info_size = 0;
	ZwQuerySystemInformation(SystemModuleInformation, nullptr, info_size, &info_size);

	void* module_info = allocate_pool<void*>(NonPagedPool, info_size);
	ZwQuerySystemInformation(SystemModuleInformation, module_info, info_size, &info_size);

	uint64_t image_base = 0;

	PRTL_PROCESS_MODULES system = static_cast<PRTL_PROCESS_MODULES>(module_info);
	for (ULONG i = 0; i < system->NumberOfModules; i++)
	{
		RTL_PROCESS_MODULE_INFORMATION module = system->Modules[i];
		const char* name = reinterpret_cast<char*>(module.FullPathName + module.OffsetToFileName);

		if (!_stricmp(name, module_name))
		{
			image_base = reinterpret_cast<uint64_t>(module.ImageBase);
			break;
		}
	}

	ExFreePool(module_info);
	return image_base;
}

uint64_t get_module_export(const char* module_name, const char* function_name)
{
	return get_module_export(get_kernel_module(module_name), function_name);
}

uint64_t get_module_export(uint64_t module_base, const char* function_name)
{
	PIMAGE_DOS_HEADER dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(module_base);
	PIMAGE_NT_HEADERS nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(module_base + dos_header->e_lfanew);
	PIMAGE_EXPORT_DIRECTORY export_directory = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(module_base + nt_headers->OptionalHeader.DataDirectory[0].VirtualAddress);

	uint32_t* name_table = reinterpret_cast<uint32_t*>(module_base + export_directory->AddressOfNames);
	uint16_t* ordinal_table = reinterpret_cast<uint16_t*>(module_base + export_directory->AddressOfNameOrdinals);
	uint32_t* address_table = reinterpret_cast<uint32_t*>(module_base + export_directory->AddressOfFunctions); 

	for (uint32_t i = 0; i < export_directory->NumberOfNames; i++)
	{
		char* name = reinterpret_cast<char*>(module_base + name_table[i]); 

		if (!_stricmp(function_name, name))
			return module_base + address_table[ordinal_table[i]];
	}

	return 0;
}

PIMAGE_SECTION_HEADER get_image_section(uint64_t module_base, const char* section_name)
{
	PIMAGE_DOS_HEADER dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(module_base);
	PIMAGE_NT_HEADERS nt_header = reinterpret_cast<PIMAGE_NT_HEADERS>(module_base + dos_header->e_lfanew);

	PIMAGE_SECTION_HEADER section_header = reinterpret_cast<PIMAGE_SECTION_HEADER>(nt_header + 1);

	for (int i = 0; i < nt_header->FileHeader.NumberOfSections; i++, section_header++)
	{
		if (!_stricmp(reinterpret_cast<const char*>(section_header->Name), section_name))
			return section_header;
	}

	return nullptr;
};

PMDL lock_pages(uint64_t address, uint64_t size, LOCK_OPERATION operation)
{
	PMDL mdl = IoAllocateMdl(reinterpret_cast<void*>(address), static_cast<uint32_t>(size), false, false, nullptr);

	virtual_address va = { .value = address }; 
	MODE access_mode = va.unused == 0xFFFF ? KernelMode : UserMode; 

	MmProbeAndLockPages(mdl, static_cast<KPROCESSOR_MODE>(access_mode), operation); 

	return mdl; 
}
