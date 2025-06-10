#pragma once
#include <ntddk.h>
#include "types.h"

#pragma warning (disable : 4996)

template <typename ...Arguments>
void print(const char* string, Arguments... args)
{
	DbgPrintEx(0, 0, string, args...);
}

template <typename T, typename J>
T  get_physical_address(J virtual_address)
{
	return (T)MmGetPhysicalAddress((void*)virtual_address).QuadPart;
}

template <typename T, typename J>
T get_virtual_address(J physical_address)
{
	return (T)MmGetVirtualForPhysical({ .QuadPart = (int64_t)physical_address });
}

template <typename T>
T allocate_pool(POOL_TYPE pool_type, uint64_t pool_size, bool zero_allocation = true)
{
	T pool = (T)ExAllocatePool(pool_type, pool_size);

	if (zero_allocation)
		memset((void*)pool, 0, pool_size); 

	return pool;
}


