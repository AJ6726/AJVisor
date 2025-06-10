#pragma once
#include <Windows.h>
#include <unordered_map>
#include <string>

struct pe_header
{
	IMAGE_DOS_HEADER dos = { };
	IMAGE_NT_HEADERS nt = { };
	IMAGE_FILE_HEADER file = { };
	IMAGE_OPTIONAL_HEADER optional = { };
	std::unordered_map<std::string, IMAGE_SECTION_HEADER> sections;
	IMAGE_DATA_DIRECTORY data_directory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] = { };

	pe_header() = default;
	pe_header(void* image_buffer);
};