#pragma once
#include "types.h"

enum class vmexit_code : uint64_t
{
    cr0_read = 0x0,
    cr1_read = 0x1,
    cr2_read = 0x2,
    cr3_read = 0x3,
    cr4_read = 0x4,
    cr5_read = 0x5,
    cr6_read = 0x6,
    cr7_read = 0x7,
    cr8_read = 0x8,
    cr9_read = 0x9,
    cr10_read = 0xA,
    cr11_read = 0xB,
    cr12_read = 0xC,
    cr13_read = 0xD,
    cr14_read = 0xE,
    cr15_read = 0xF,

    cr_write = 0x10,
    cr0_write = 0x10,
    cr1_write = 0x11,
    cr2_write = 0x12,
    cr3_write = 0x13,
    cr4_write = 0x14,
    cr5_write = 0x15,
    cr6_write = 0x16,
    cr7_write = 0x17,
    cr8_write = 0x18,
    cr9_write = 0x19,
    cr10_write = 0x1A,
    cr11_write = 0x1B,
    cr12_write = 0x1C,
    cr13_write = 0x1D,
    cr14_write = 0x1E,
    cr15_write = 0x1F,

    dr0_read = 0x20,
    dr1_read = 0x21,
    dr2_read = 0x22,
    dr3_read = 0x23,
    dr4_read = 0x24,
    dr5_read = 0x25,
    dr6_read = 0x26,
    dr7_read = 0x27,
    dr8_read = 0x28,
    dr9_read = 0x29,
    dr10_read = 0x2A,
    dr11_read = 0x2B,
    dr12_read = 0x2C,
    dr13_read = 0x2D,
    dr14_read = 0x2E,
    dr15_read = 0x2F,

    dr0_write = 0x30,
    dr1_write = 0x31,
    dr2_write = 0x32,
    dr3_write = 0x33,
    dr4_write = 0x34,
    dr5_write = 0x35,
    dr6_write = 0x36,
    dr7_write = 0x37,
    dr8_write = 0x38,
    dr9_write = 0x39,
    dr10_write = 0x3A,
    dr11_write = 0x3B,
    dr12_write = 0x3C,
    dr13_write = 0x3D,
    dr14_write = 0x3E,
    dr15_write = 0x3F,

    de = 0x40, // divide by zero
    db = 0x41, // debug exception

    exception_2 = 0x42,
    // nmi = 0x42, interception of this vector isnt supported

    bp = 0x43, // breakpoint
    of = 0x44, // overflow
    br = 0x45, // bound range exceeded
    ud = 0x46, // invalid opcode
    nm = 0x47, // device not available
    df = 0x48, // double fault
    cso = 0x49, // coprocessor segment overrun
    ts = 0x4A, // invalid tss
    np = 0x4B, // segment not present
    ss = 0x4C, // stack exception
    gp = 0x4D, // general protection
    pf = 0x4E, // page fault

    exception_15 = 0x4F,

    mf = 0x50, // x87 fpu floating-point error
    ac = 0x51, // alignment check
    mc = 0x52, // machine check
    xf = 0x53, // simd floating-point exception

    exception_20 = 0x54,

    cp = 0x55, // control protection exception

    exception_22 = 0x56,
    exception_23 = 0x57,
    exception_24 = 0x58,
    exception_25 = 0x59,
    exception_26 = 0x5A,
    exception_27 = 0x5B,

    hv = 0x5C, // event injection exception
    vc = 0x5D, // vmm communication exception
    sx = 0x5E, // security exception

    exception_31 = 0x5F,

    // physical interrupts
    intr = 0x60, // physical intr (maskable interrupt).
    nmi = 0x61, // physical nmi.
    smi = 0x62, // physical smi (additional info in exitinfo1).
    init = 0x63, // physical init.
    vintr = 0x64, // virtual intr.

    // control register operations
    cr0_sel_write = 0x65, // write of cr0 that changed any bits other than cr0.ts or cr0.mp.

    // register reads
    idtr_read = 0x66, // read of idtr.
    gdtr_read = 0x67, // read of gdtr.
    ldtr_read = 0x68, // read of ldtr.
    tr_read = 0x69, // read of tr.

    // register writes
    idtr_write = 0x6A, // write of idtr.
    gdtr_write = 0x6B, // write of gdtr.
    ldtr_write = 0x6C, // write of ldtr.
    tr_write = 0x6D, // write of tr.

    // other instructions
    rdtsc = 0x6E, // rdtsc instruction.
    rdpmc = 0x6F, // rdpmc instruction.
    pushf = 0x70, // pushf instruction.
    popf = 0x71, // popf instruction.
    cpuid = 0x72, // cpuid instruction.
    rsm = 0x73, // rsm instruction.
    iret = 0x74, // iret instruction.
    swint = 0x75, // software interrupt (intn instructions).
    invd = 0x76, // invd instruction.
    pause = 0x77, // pause instruction.
    hlt = 0x78, // hlt instruction.
    invlpg = 0x79, // invlpg instruction.
    invlpg_a = 0x7A, // invlpg_a instruction.
    ioio = 0x7B, // in or out accessing protected port.
    msr = 0x7C, // rdmsr or wrmsr access to protected msr.
    task_switch = 0x7D, // task switch.
    ferr_freeze = 0x7E, // fp legacy handling enabled, and processor is frozen in an x87/mmx instruction waiting for an interrupt.
    shutdown = 0x7F, // shutdown

    vmrun = 0x80, // vmrun instruction.
    vmcall = 0x81, // vmcall instruction.
    vmload = 0x82, // vmload instruction.
    vmsave = 0x83, // vmsave instruction.
    stgi = 0x84, // stgi instruction.
    clgi = 0x85, // clgi instruction.
    skinit = 0x86, // skinit instruction.

    rdtscp = 0x87, // rdtscp instruction.
    icebp = 0x88, // icebp instruction.
    wbinvd = 0x89, // wbinvd instruction.
    monitor = 0x8A, // monitor instruction.
    mwait = 0x8B, // mwait instruction.
    mwait_conditional = 0x8C, // mwait instruction with the events hint set in rcx.
    rdpru = 0x8E, // rdpru instruction.
    xsetbv = 0x8D, // xsetbv instruction.

    efer_write_trap = 0x8F, // write of efer msr (occurs after guest instruction finishes)
    cr0_write_trap = 0x90, // write of cr0-15, respectively (occurs after guest instruction finishes)
    cr1_write_trap = 0x91,
    cr2_write_trap = 0x92,
    cr3_write_trap = 0x93,
    cr4_write_trap = 0x94,
    cr5_write_trap = 0x95,
    cr6_write_trap = 0x96,
    cr7_write_trap = 0x97,
    cr8_write_trap = 0x98,
    cr9_write_trap = 0x99,
    cr10_write_trap = 0x9A,
    cr11_write_trap = 0x9B,
    cr12_write_trap = 0x9C,
    cr13_write_trap = 0x9D,
    cr14_write_trap = 0x9E,
    cr15_write_trap = 0x9F,

    invlpgb = 0xA0, // invlpg instruction.
    invlpgb_illegal = 0xA1, // invlpg instruction with an illegal operand.
    invpcid = 0xA2, // invpcid instruction.
    mcommit = 0xA3, // mcommit instruction.
    tlbsync = 0xA4, // tlbsync instruction.
    buslock = 0xA5, // bus lock while bus lock threshold counter value is 0

    npf = 0x400, // nested paging: host-level page fault occurred (exitinfo1 contains fault error code; exitinfo2 contains the guest physical address causing the fault)
    avic_incomplete_ipi = 0x401, // avic—virtual ipi delivery not completed.
    avic_noaccel = 0x402, // avic—attempted access by guest to vapic register not handled by avic hardware
    vmgexit = 0x403, // vmgexit instruction

    invalid = -1, // invalid guest state in vmcb
    busy = -2, // busy bit was set in the encrypted vmsa
    idle_required = -3, // the sibling thread is not in an idle state
    invalid_pmc = -4,
};
