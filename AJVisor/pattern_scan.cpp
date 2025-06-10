#include "pattern_scan.h"
#include "utility.h"
#include "wrappers.h"

uint64_t pattern_scan(uint64_t image_base, const char* section_name, unsigned const char* pattern, const char* mask)
{
	PIMAGE_SECTION_HEADER section = get_image_section(image_base, section_name);

	uint8_t* data = allocate_pool<uint8_t*>(NonPagedPool, section->SizeOfRawData);

	memcpy(data, reinterpret_cast<void*>(image_base + section->VirtualAddress), section->SizeOfRawData);

	for (uint32_t i = 0; i < section->SizeOfRawData; i++)
	{
		if (data[i] == pattern[0])
		{
			bool found = true;

			for (uint32_t j = 0; j < strlen(mask); j++)
			{
				if (mask[j] == 'x' && data[i + j] != pattern[j])
				{
					found = false;
					break;
				}
			}

			if (found)
			{
				ExFreePool(data);
				return image_base + section->VirtualAddress + i;
			}
		}
	}
	ExFreePool(data);
	return 0;
}

uint64_t resolve_rva(uint64_t instruction, uint64_t offset, uint64_t instruction_size)
{
	int32_t rip_offset = *(int32_t*)(instruction + offset);
	return instruction + instruction_size + rip_offset;
}