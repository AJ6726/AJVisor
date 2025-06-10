#pragma once
#include "types.h"

uint64_t pattern_scan(uint64_t image_base, const char* section_name, unsigned const char* pattern, const char* mask);
uint64_t resolve_rva(uint64_t instruction, uint64_t offset, uint64_t instruction_size);

template <typename T>
T pattern_scan(uint64_t image_base, const char* section_name, const char* pattern, const char* mask)
{
	return (T)pattern_scan(image_base, section_name, (unsigned const char*)pattern, mask);
}

template <typename T>
T resolve_rva(uint64_t instruction, int32_t offset, int32_t instruction_size)
{
	return (T)resolve_rva(instruction, offset, instruction_size);
}
