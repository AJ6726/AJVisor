#include "process.h"
#include "utility.h"

process::process(std::wstring_view name)
{
	id = utility::get_process_id(name); 
	eprocess = hv::get_eprocess(id); 
	cr3 = hv::get_cr3(eprocess); 
	base = hv::get_section_base(eprocess); 
	peb = get_peb(eprocess); 
}

uint64_t process::get_peb(uint64_t eprocess)
{
	constexpr int32_t peb_offsets[] = { 0x2e0, 0x550 };

	for (int i = 0; i < std::size(peb_offsets); i++)
	{
		uint64_t potential_peb = read_memory<uint64_t>(eprocess + peb_offsets[i]);

		if (!potential_peb)
			continue;

		uint64_t image_base = read_memory<uint64_t>(potential_peb + 0x10);
		if (!image_base)
			continue;

		if (base == image_base)
			return potential_peb;
	}

	return 0;
}

uint64_t process::get_module_base(std::wstring_view module_name)
{
	uint64_t peb_ldr = read_memory<uint64_t>(peb + 0x18);

	PEB_LDR_DATA ldr_data = read_memory<PEB_LDR_DATA>(peb_ldr);

	for (PLIST_ENTRY entry = ldr_data.InLoadOrderModuleList.Flink; ;)
	{
		LDR_DATA_TABLE_ENTRY ldr_entry = read_memory<LDR_DATA_TABLE_ENTRY>(entry);

		std::wstring base_dll_name(ldr_entry.BaseDllName.Length, L'\0');

		read_memory(base_dll_name.data(), ldr_entry.BaseDllName.Buffer, ldr_entry.BaseDllName.Length);

		if (base_dll_name.contains(module_name))
			return reinterpret_cast<uint64_t>(ldr_entry.DllBase);

		if (!ldr_entry.InLoadOrderLinks.Flink || ldr_entry.InLoadOrderLinks.Flink == reinterpret_cast<PLIST_ENTRY>(peb_ldr + 0x10))
			break; 

		entry = ldr_entry.InLoadOrderLinks.Flink;
	}

	return 0; 

}
