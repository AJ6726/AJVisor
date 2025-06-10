#include <ntddk.h>
#include "hv.h"
#include "wrappers.h"

#pragma section (".AJVisor", read)
#pragma comment(linker, "/merge:.AJVisor=.text")
extern "C" __declspec(allocate(".AJVisor")) int ajvisor_signature = 0x6969; 

void driver_unload(PDRIVER_OBJECT);

NTSTATUS driver_entry(PDRIVER_OBJECT driver_object = nullptr, PUNICODE_STRING registry_path = nullptr)
{

	UNREFERENCED_PARAMETER(registry_path);

	if(driver_object)
		driver_object->DriverUnload = driver_unload;

	if (!check_svm_support())
		return STATUS_UNSUCCESSFUL;

	virtualize();

	return STATUS_SUCCESS;
}

void driver_unload(PDRIVER_OBJECT)
{
	print("Driver Unload \n");
	unvirtualize();
}
