#include <ntifs.h>
#include <intrin.h>
#include "cpuid.h"
#include "hypercall.h"
#include "logger.h"
#include "pattern_scan.h"
#include "svm.h"
#include "utility.h"
#include "hv.h"
#include "wrappers.h"

extern "C" void __vmlaunch(uint64_t* guest_vmcb_pa);
extern "C" uint64_t __vmmcall(uint64_t request, void* buffer);

uint64_t hypercall(hypercall_code code, void* buffer)
{
    hypercall_request request = {};
    request.code = code;
    request.magic = hypercall_code::magic;

    return __vmmcall(request.value, buffer);
}

bool check_svm_support()
{
    cpuid cpuid = {};
    __cpuid(cpuid.regs, static_cast<int32_t>(function_id::vendor));

    if (!((cpuid.vendor.v1 == 'htuA') && (cpuid.vendor.v2 == 'DMAc') && (cpuid.vendor.v3 == 'itne')))
        return false;

    memset(cpuid.regs, 0, sizeof(int32_t) * 4);
    __cpuid(cpuid.regs, static_cast<int32_t>(function_id::identifiers));

    if (!cpuid.identifiers.feature.svm)
    {
        print("[HV] AMD-V/SVM not enabled\n");
        return false;
    }

    memset(cpuid.regs, 0, sizeof(int32_t) * 4);
    __cpuid(cpuid.regs, static_cast<int32_t>(function_id::svm_features));

    if (!cpuid.svm.feature.np)
    {
        print("[HV] Nested Paging not supported\n");
        return false;
    }

    if (!cpuid.svm.feature.nrips)
    {
        print("Next Rip not supported\n");
        return false;
    }

    vmcr vmcr = { .value = __readmsr(static_cast<uint64_t>(msr_number::vmcr))};

    if (vmcr.lock && vmcr.svm_dis)
    {
        print("[HV] Svm Locked and Disabled\n");
        return false;
    }
    
    return true;
}

void setup_resources()
{
    uint32_t core_count = KeQueryActiveProcessorCount(nullptr);

    svm::vcores = allocate_pool<vcore*>(NonPagedPool, sizeof(vcore) * core_count);   
    svm::hpt = allocate_pool<host_page_table*>(NonPagedPool, sizeof(host_page_table)); 
    svm::npt = allocate_pool<nested_page_table*>(NonPagedPool, sizeof(nested_page_table)); 

    uint64_t ntoskrnl = get_kernel_module("ntoskrnl.exe"); 

    uint64_t instruction = pattern_scan<uint64_t>(ntoskrnl, ".text", "\xE8\x00\x00\x00\x00\x4D\x8B\x57\x10", "x????xxxx"); 
    instruction = resolve_rva<uint64_t>(instruction, 1, 5);
	svm::physical_memory_block = resolve_rva<PPHYSICAL_MEMORY_RUN*>(instruction + 0x7B, 3, 7);

    instruction = pattern_scan<uint64_t>(ntoskrnl, ".text", "\xB9\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x89\x43\x18", "x????xxx????xxxx");
    svm::pfn_database = *resolve_rva<PMMPFN*>(instruction + 5, 3, 7);

    logger::lock.initialize(); 
    logger::logs = allocate_pool<logger_message*>(NonPagedPool, max_logs_size);
    logger::flush_buffer = allocate_pool<void*>(NonPagedPool, max_logs_size);
}

void virtualize()
{
    setup_resources();
    setup_host_page_table(svm::hpt);
    setup_nested_page_table(svm::npt);

    call_on_all_cores(virtualize_core);
}

void unvirtualize()
{
    call_on_all_cores(unvirtualize_core);

    ExFreePool(svm::vcores);
    ExFreePool(svm::npt);
    ExFreePool(logger::logs); 
}

void virtualize_core(uint32_t core)
{
    volatile bool virtualized = false;

    print("[HV] Virtualizing Core %d \n", core);

    vcore& vcore = svm::vcores[core];
    vcore.self = &vcore;
    vcore.host_vmcb_pa = get_physical_address<uint64_t>(&vcore.host_vmcb);
    vcore.guest_vmcb_pa = get_physical_address<uint64_t>(&vcore.guest_vmcb);

    efer efer = { .value = __readmsr(static_cast<uint32_t>(msr_number::efer))};
    efer.svme = 1;
    __writemsr(static_cast<uint32_t>(msr_number::efer), efer.value);

    efer.svme = 0;
    vcore.storage.shadow_efer = efer;

    cpuid cpuid = {};
    __cpuid(cpuid.regs, static_cast<uint32_t>(function_id::hv_interface_id));

    vcore.storage.in_vm = (cpuid.hv_interface.signature == '1#vH') ? 1 : 0;

    print("[HV] In Virtual Machine %d \n", vcore.storage.in_vm); 

    CONTEXT start_context = { .ContextFlags = CONTEXT_ALL };
    RtlCaptureContext(&start_context);

    if (virtualized)
        return;
   
    setup_guest(vcore, start_context);
    setup_host(vcore, virtualized);

    print("[HV] Calling VmLaunch\n", start_context.Rip); 

    __vmlaunch(&vcore.guest_vmcb_pa);
}

void unvirtualize_core(uint32_t core)
{
    UNREFERENCED_PARAMETER(core);
    
    hypercall(hypercall_code::stop_hypervisor, (void*)1); 
}

void setup_guest(vcore& vcore, CONTEXT& start_context)
{
    control_area& control = vcore.guest_vmcb.control;

    control.vmmcall = 1;
    control.vmrun = 1; 
 
    control.guest_asid = 1;

    setup_msr_pm(vcore);
    control.msr_prot = 1;
    control.msrpm_base_pa = get_physical_address<uint64_t>(vcore.msr_pm.bits);

    control.np_enable = 1;
    control.ncr3 = get_physical_address<uint64_t>(svm::npt->pml4);

    state_save_area& state_save = vcore.guest_vmcb.state_save;

    state_save.rip = start_context.Rip;
    state_save.rsp = start_context.Rsp;
    state_save.rflags.value = start_context.EFlags;
    
    state_save.cr0.value = __readcr0();
    state_save.cr2.value = __readcr2();
    state_save.cr3.value = __readcr3();
    state_save.cr4.value = __readcr4();
    state_save.efer.value = __readmsr(static_cast<uint32_t>(msr_number::efer));
    state_save.g_pat = __readmsr(static_cast<uint32_t>(msr_number::pat));

    descriptor_table_register idtr{}, gdtr{};
    __sidt(&idtr);
    _sgdt(&gdtr);

    state_save.idtr.base = idtr.base;
    state_save.idtr.limit = idtr.limit;
    state_save.gdtr.base = gdtr.base;
    state_save.gdtr.limit = gdtr.limit;

    state_save.cs.selector.value = start_context.SegCs;
    state_save.es.selector.value = start_context.SegEs;
    state_save.ds.selector.value = start_context.SegDs; 
    state_save.ss.selector.value = start_context.SegSs;

    state_save.cs.get_attributes(gdtr);
    state_save.es.get_attributes(gdtr);
    state_save.ds.get_attributes(gdtr);
    state_save.ss.get_attributes(gdtr);

    state_save.cs.limit = __segmentlimit(start_context.SegCs);
    state_save.es.limit = __segmentlimit(start_context.SegEs);
    state_save.ds.limit = __segmentlimit(start_context.SegDs);
    state_save.ss.limit = __segmentlimit(start_context.SegSs);

    __svm_vmsave(vcore.guest_vmcb_pa);

    __writemsr(static_cast<uint32_t>(msr_number::hsave_pa), vcore.host_vmcb_pa);

    __svm_vmsave(vcore.host_vmcb_pa);
}

void setup_host(vcore& vcore, volatile bool& virtualized)
{
    control_register& host_cr3 = vcore.host_vmcb.state_save.cr3;

    host_cr3.value = 0; 
    host_cr3.c3.pml4 = get_physical_address<uint64_t>(svm::hpt->pml4) >> page_4kb_shift;

    __writecr3(host_cr3.value);

    virtualized = true;
}

void setup_host_page_table(host_page_table* hpt)
{
    uint64_t identity_pml4_index = get_empty_pml4_index();

    virtual_address identity_mapped_va = { .unused = (identity_pml4_index > 255) ? 0xFFFFull : 0 }; 
    identity_mapped_va.pml4e_index = identity_pml4_index;

    hpt->identity_pml4_index = identity_pml4_index;
    hpt->identity_mapped_va = identity_mapped_va.value;

    print("[HV] Identity PML4 Index: %llu\n", identity_pml4_index);
	print("[HV] Identity Mapped VA: 0x%llX\n", identity_mapped_va.value);

    page_map_level_4& pml4e = hpt->pml4[identity_pml4_index];
    pml4e.present = 1;
    pml4e.write = 1;
    pml4e.page_frame_number = get_physical_address<uint64_t>(hpt->pdp) >> page_4kb_shift;

	//If you have more than 128GB of RAM, I hate you.
    for (int i = 0; i < 128; i++)
    {
        page_directory_pointer& pdpe = hpt->pdp[i];
        pdpe.huge_page = 1; 
        pdpe.huge.present = 1;
        pdpe.huge.write = 1;
        pdpe.huge.page_frame_number = i;
    }

    uint64_t system_process = reinterpret_cast<uint64_t>(PsInitialSystemProcess);
    control_register system_cr3 = *reinterpret_cast<control_register*>(system_process + 0x28);
    uint64_t* system_pml4 = get_virtual_address<uint64_t*>(system_cr3.c3.pml4 << page_4kb_shift);

    page_map_level_4* host_pml4 = hpt->pml4;
    memcpy(host_pml4 + 256, system_pml4 + 256, 8 * 256);

    //Fixup the self referencing index
    uint64_t self_ref_index = get_self_ref_index(); 
    host_pml4[self_ref_index].page_frame_number = get_physical_address<uint64_t>(host_pml4) >> page_4kb_shift;

    svm::system_cr3 = system_cr3;
}

void setup_nested_page_table(nested_page_table* npt)
{
    for(int i = 0; i < npt->max_free_page_count; i++)
		npt->free_page_pa[i] = get_physical_address<uint64_t>(npt->free_pages[i]);

    page_map_level_4& pml4e = npt->pml4[0];
    pml4e.present = 1;
    pml4e.write = 1;
    pml4e.usermode = 1;
    pml4e.page_frame_number = get_physical_address<uint64_t>(npt->pdp) >> page_4kb_shift;

    for (int i = 0; i < 512; i++)
    {
        page_directory_pointer& pdpe = npt->pdp[i];
        pdpe.huge_page = 1;
        pdpe.huge.present = 1;
        pdpe.huge.write = 1;
        pdpe.huge.usermode = 1; 
        pdpe.huge.page_frame_number = i;
    }
}

void setup_msr_pm(vcore& vcore)
{
    msr_pm& msr_pm = vcore.msr_pm;
    msr_pm.set(msr_number::efer, msr_access::read);
    msr_pm.set(msr_number::efer, msr_access::write);
    msr_pm.set(msr_number::hsave_pa, msr_access::read);
    msr_pm.set(msr_number::hsave_pa, msr_access::write);
}