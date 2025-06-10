#pragma once
#include "hv.h"
#include "nt.h"

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

	process(std::wstring_view name);

	uint64_t get_peb(uint64_t eprocess); 
	uint64_t get_module_base(std::wstring_view module_name); 

	template <typename T, typename J>
	T read_memory(J source, address_type type = address_type::virt)
	{
		T value = { };
		(type == address_type::virt) ? hv::read_virtual_memory(cr3, (uint64_t)&value, (uint64_t)source, sizeof(T)) : hv::read_physical_memory((uint64_t)&value, (uint64_t)source, sizeof(T));
		return value;
	}

	template <typename T, typename J>
	void read_memory(T destination, J source, uint64_t size, address_type type = address_type::virt)
	{
		(type == address_type::virt) ? hv::read_virtual_memory(cr3, (uint64_t)destination, (uint64_t)source, size) : hv::read_physical_memory((uint64_t)destination, (uint64_t)source, size);
	}

	template <typename T, typename J>
	void write_memory(T destination, J value, address_type type = address_type::virt)
	{
		(type == address_type::virt) ? hv::write_virtual_memory(cr3, (uint64_t)destination, (uint64_t)&value, sizeof(J)) : hv::write_physical_memory((uint64_t)destination, (uint64_t)&value, sizeof(J));
	}

	template <typename T, typename J>
	void hide_memory(T address, J size, address_type type = address_type::virt)
	{
		(type == address_type::virt) ? hv::hide_virtual_memory(cr3, (uint64_t)address, size) : hv::hide_physical_page((uint64_t)address);
	}

	template<typename T>
	uint64_t get_physical_address(T address)
{
		return hv::get_physical_address(cr3, (uint64_t)address);
	}
};