#pragma once
#include "hypercall.h"
#include "vcore.h"

uint64_t guest_va_to_guest_pa(control_register guest_cr3, uint64_t guest_va);
uint64_t guest_va_to_guest_pa(control_register guest_cr3, uint64_t guest_va, uint64_t& offset_to_next_page);
void* guest_va_to_host_va(control_register guest_cr3, uint64_t guest_va);
void* guest_va_to_host_va(control_register guest_cr3, uint64_t guest_va, uint64_t& offset_to_next_page);

uint64_t bruteforce_cr3(uint64_t eprocess);

hypercall_status get_eprocess(void* user_buffer);
hypercall_status get_cr3(void* user_buffer);
hypercall_status read_virtual_memory(control_register& guest_cr3, void* user_buffer);
hypercall_status write_virtual_memory(control_register& guest_cr3, void* user_buffer);
hypercall_status read_physical_memory(control_register& guest_cr3, void* user_buffer);
hypercall_status write_physical_memory(control_register& guest_cr3, void* user_buffer);
hypercall_status get_physical_address(void* user_buffer);
hypercall_status hide_virtual_memory(vcore* vcore, void* user_buffer);
hypercall_status hide_physical_page(vcore* vcore, void* user_buffer);
hypercall_status leak_image_base(void* user_buffer); 
hypercall_status get_section_base(void* user_buffer); 
hypercall_status flush_logs(vcore* vcore, void* user_buffer); 
