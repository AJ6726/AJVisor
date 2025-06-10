#include <intrin.h>
#include "svm.h"
#include "vmexit_handlers.h"
#include "vcore.h"
#include "vmexit_codes.h"
#include "hypercall.h"
#include "hyper_helpers.h"
#include "wrappers.h"
#include "cpuid.h"
#include "logger.h"
#include <Zydis/Zydis.h>

extern "C" bool handle_vmexit(vcore* vcore)
{
    vcore->storage.stop_nrip = false;

    control_area& guest_control = vcore->guest_vmcb.control;
    state_save_area& guest_state_save = vcore->guest_vmcb.state_save;

    switch (guest_control.exit_code)
    {

    case vmexit_code::vmcall:
        handle_hypercall(vcore);
        break;

    case vmexit_code::msr:
        handle_msr(vcore);
        break;

    case vmexit_code::vmrun:
        break; 

    case vmexit_code::invalid:
        vcore->storage.stop_hv = true;
        break;

    default:
        break;
    }

    if (!vcore->storage.stop_nrip)
        guest_state_save.rip = guest_control.nrip;

    if (vcore->storage.stop_hv)
        return handle_stop_hv(vcore);

    return true;
}

void handle_hypercall(vcore* vcore)
{
    state_save_area& guest_state_save = vcore->guest_vmcb.state_save;
    context& guest_context = vcore->guest_context;

    hypercall_request request = { .value = guest_context.rcx.value };

    if (request.magic != hypercall_code::magic)
    {
        inject_exception(vcore, exception_vector::invalid_opcode, false, true);
        return;
    }

    hypercall_status status = hypercall_status::success;

    // The guest usermode address space is not accessible to the VMM (World Switch).
    // Translate the guest buffer so it is accessible. The guest buffer may also hold other buffers that need to be translated.
    void* user_buffer = guest_va_to_host_va(guest_state_save.cr3, guest_context.rdx.value);
    if (!user_buffer)
    {
        status = hypercall_status::failed_gva_translation;
        guest_state_save.rax.value = static_cast<uint64_t>(status);
        return;
    }

    switch (request.code)
    {
    case hypercall_code::get_eprocess:
        status = get_eprocess(user_buffer);
        break;

    case hypercall_code::get_cr3:
        status = get_cr3(user_buffer);
        break;

    case hypercall_code::read_virtual_memory:
        status = read_virtual_memory(guest_state_save.cr3, user_buffer);
        break;

    case hypercall_code::write_virtual_memory:
        status = write_virtual_memory(guest_state_save.cr3, user_buffer);
        break;

    case hypercall_code::read_physical_memory:
        status = read_physical_memory(guest_state_save.cr3, user_buffer);
        break;

    case hypercall_code::write_physical_memory:
        status = write_physical_memory(guest_state_save.cr3, user_buffer);
        break;

    case hypercall_code::get_physical_address:
        status = get_physical_address(user_buffer);
        break;

    case hypercall_code::hide_virtual_memory: 
		status = hide_virtual_memory(vcore, user_buffer);
        break; 

    case hypercall_code::hide_physical_page: 
        status = hide_physical_page(vcore, user_buffer); 
        break;
         
    case hypercall_code::leak_image_base: 
        status = leak_image_base(user_buffer); 
        break; 

    case hypercall_code::get_section_base: 
		status = get_section_base(user_buffer);
        break;

    case hypercall_code::flush_logs: 
        status = flush_logs(vcore, user_buffer); 
        break; 

    case hypercall_code::ping: 
        static_cast<ping_info*>(user_buffer)->response = hypercall_code::magic; 
        break; 

    case hypercall_code::stop_hypervisor: 
        vcore->storage.stop_hv = true; 
        break; 

    default:
        break;
    }

    guest_state_save.rax.value = static_cast<uint64_t>(status);
}

bool handle_stop_hv(vcore* vcore)
{
    control_area& guest_control = vcore->guest_vmcb.control;
    state_save_area& guest_state_save = vcore->guest_vmcb.state_save;

    vcore->guest_context.rax.value = static_cast<uint64_t>(hypercall_status::success);

    machine_frame& machine_frame = vcore->machine_frame;
    machine_frame.rip = guest_control.nrip;
    machine_frame.cs = guest_state_save.cs.selector.value;
    machine_frame.eflags = static_cast<uint32_t>(guest_state_save.rflags.value);
    machine_frame.rsp = guest_state_save.rsp;
    machine_frame.ss = guest_state_save.ss.selector.value;

    __svm_vmload(vcore->guest_vmcb_pa);
    __writecr3(guest_state_save.cr3.value);

    _disable();
    __svm_stgi();

    efer& efer = guest_state_save.efer;
    efer.svme = 0;
    __writemsr(static_cast<uint32_t>(msr_number::efer), efer.value);

    __writemsr(static_cast<uint32_t>(msr_number::hsave_pa), 0); 

    return false;
}

void handle_msr(vcore* vcore)
{
    control_area& guest_control = vcore->guest_vmcb.control;
    state_save_area& guest_state_save = vcore->guest_vmcb.state_save;
    context& guest_context = vcore->guest_context;
    vcore_storage& storage = vcore->storage;

    uint32_t intercepted_msr = guest_context.rcx.low;
 
    if ((storage.in_vm && (intercepted_msr >= 0x40000000 && intercepted_msr <= 0x40000105)))
        return;

    if (!(((intercepted_msr > 0x00000000) && (intercepted_msr < 0x00001FFF)) || ((intercepted_msr > 0xC0000000) && (intercepted_msr < 0xC0001FFF)) || ((intercepted_msr > 0xC0010000) && (intercepted_msr < 0xC0011FFF))))
    {
        inject_exception(vcore, exception_vector::general_protection, true, true);
        return;
    }

    msr_intercept msr_intercept_type = static_cast<msr_intercept>(guest_control.exit_info1.msr.value);

    msr_number msr_num = static_cast<msr_number>(intercepted_msr); 

    if (msr_intercept_type == msr_intercept::rdmsr)
    {
        switch (msr_num)
        {

        case msr_number::efer:
        {
            register_64 shadow_efer = { .value = storage.shadow_efer.value };

            guest_state_save.rax = { .low = shadow_efer.low };
            guest_context.rdx = { .low = shadow_efer.high };

            break;
        }
        case msr_number::hsave_pa:
        {
            register_64 shadow_hsave_pa{ .value = storage.shadow_hsave_pa.value };

            guest_state_save.rax = { .low = shadow_hsave_pa.low };
            guest_context.rdx = { .low = shadow_hsave_pa.high };

            break;
        }
        default:
        {
            register_64 result = { .value = __readmsr(intercepted_msr) };

            guest_state_save.rax = { .low = result.low };
            guest_context.rdx = { .low = result.high };

            break;
        }
        }
    }

    // Only write to the shadow (fake/dummy) HSAVE as any changes to it will affect the host
    // EFER changes will be let through to the guest as the features exposed by EFER are necessary
    register_64 new_msr = { };
    new_msr.low = guest_state_save.rax.low;
    new_msr.high = guest_context.rdx.low;

    if (msr_intercept_type == msr_intercept::wrmsr)
    {
        switch (msr_num)
        {

        case msr_number::efer:
        {
            efer new_efer = { new_msr.value };

            // Deal with reserved mbz (must be zero) bits
            if (new_efer.reserved1_mbz || new_efer.reserved2_mbz)
            {
                inject_exception(vcore, exception_vector::general_protection, true, true);
                break;
            }

            // Deal with reserved raz (return always zero) bits
            new_efer.reserved0_raz = 0;
            storage.shadow_efer = new_efer;

            // Ensure svme is always on or guest won't run
            guest_state_save.efer = new_efer;
            guest_state_save.efer.svme = 1;

            break;
        }

        case msr_number::hsave_pa:
        {
            hsave_pa new_hsave_pa = { new_msr.value };

            if (new_hsave_pa.mbz)
            {
                inject_exception(vcore, exception_vector::general_protection, true, true);
                break;
            }

            storage.shadow_hsave_pa = new_hsave_pa;
            break;
        }

        default:
            __writemsr(intercepted_msr, new_msr.value);
            break;
   
        }
    }
}

void inject_exception(vcore* vcore, exception_vector vector, bool push_error, bool stop_nrip)
{
    vcore->storage.stop_nrip = stop_nrip;

    event_injection& event_injection = vcore->guest_vmcb.control.event_injection;
    event_injection.vector = vector;
    event_injection.type = interrupt_type::hardware;
    event_injection.valid = true;
    event_injection.error_code = 0;

    if (push_error)
        event_injection.error_code_valid = true;
}
