#pragma once
#include "types.h"

union exit_info1
{
    uint64_t value;

    struct
    {
        uint64_t gpr_number : 4;
        uint64_t reserved0 : 59;
        uint64_t mov_crx : 1;
    } movcr;

    struct
    {
        uint64_t gpr_number : 4;
        uint64_t reserved0 : 60;
    } movdr;

    struct
    {
        uint64_t software_interrupt_number : 8;
        uint64_t reserved0 : 56;
    } intn;

    struct
    {
        uint64_t virtual_address;
    } invlpg;

    struct
    {
        uint64_t access_type : 1;
        uint64_t reserved0 : 1;
        uint64_t string_based_port_access : 1;
        uint64_t repeated_port_access : 1;
        uint64_t size8 : 1;
        uint64_t size16 : 1;
        uint64_t size32 : 1;
        uint64_t address16 : 1;
        uint64_t address32 : 1;
        uint64_t address64 : 1;
        uint64_t effective_segment_number : 3;
        uint64_t reserved1 : 3;
        uint64_t port_number : 16;
        uint64_t reserved2 : 32;
    } in_out;

    struct
    {
        uint64_t value;
    } msr;

    struct
    {
        uint64_t segment_selector : 16;
        uint64_t reserved9 : 48;
    } ts;

    struct
    {
        uint64_t error_code : 64;
    } np;

    struct
    {
        uint64_t error_code : 64;
    } ss;

    struct
    {
        uint64_t error_code : 64;
    } gp;

    struct
    {
        uint64_t error_code : 64;
    } pf;
};

union exit_int_info
{
    uint64_t value;
    struct
    {
        uint64_t vector : 8;
        uint64_t type : 3;
        uint64_t error_code_valid : 1;
        uint64_t reserved0 : 19;
        uint64_t valid : 1;
        uint64_t error_code : 32;
    };
};
