#include "pe_header.h"

pe_header::pe_header(void* image_buffer)
{
	uint64_t base = reinterpret_cast<uint64_t>(image_buffer);

	dos = *reinterpret_cast<PIMAGE_DOS_HEADER>(base);
	nt = *reinterpret_cast<PIMAGE_NT_HEADERS>(base + dos.e_lfanew);
	file = nt.FileHeader;
	optional = nt.OptionalHeader;

	PIMAGE_SECTION_HEADER section = reinterpret_cast<PIMAGE_SECTION_HEADER>(base + dos.e_lfanew + sizeof(IMAGE_NT_HEADERS));
	for (int i = 0; i < file.NumberOfSections; i++, section++)
		sections[reinterpret_cast<const char*>(section->Name)] = *section;

	for (int i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
		data_directory[i] = optional.DataDirectory[i];

}

