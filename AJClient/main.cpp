#include <Windows.h>
#include <iostream>
#include "process.h"
#include "utility.h"

int press_to_exit(); 

int main()
{
	
	if (!hv::is_running())
	{
		std::printf("[CLIENT] Hypervisor is not running, please start the hypervisor first \n");
		return press_to_exit(); 
	}

	hv::hide(); 

	std::wstring_view test_process = utility::get_process_id(L"notepad.exe") ? L"notepad.exe" : L"Notepad.exe"; 

	process notepad = process(test_process);

	std::wprintf(L"[CLIENT] %s Id %d | eprocess %p | cr3 %p | base %p | peb %p \n", test_process.data(), notepad.id, notepad.eprocess, notepad.cr3, notepad.base, notepad.peb);

	while (true)
	{
		if(GetAsyncKeyState(VK_INSERT) & 1)
		{
			hv::unload(); 
			return press_to_exit(); 
		}
	}
}

int press_to_exit()
{
	std::printf("Press enter to exit...\n");
	std::cin.get();
	return 0;
}