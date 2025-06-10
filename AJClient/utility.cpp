#include <Windows.h>
#include <TlHelp32.h>
#include "utility.h"

uint32_t utility::get_process_id(std::wstring_view name)
{
	HANDLE snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	uint32_t process_id = 0; 

	PROCESSENTRY32W process = { };
	process.dwSize = sizeof(PROCESSENTRY32W);

	while (Process32NextW(snapshot_handle, &process))
	{
		if (!name.compare(process.szExeFile))
		{
			process_id = process.th32ProcessID;
			break; 
		}
	}

	CloseHandle(snapshot_handle);
	return process_id;
}