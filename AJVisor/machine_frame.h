#pragma once

#include "types.h"

struct machine_frame
{
    uint64_t rip;
    uint16_t cs;
    uint16_t fill1[3];
    uint32_t eflags;
    uint32_t fill2;
    uint64_t rsp;
    uint16_t ss;
    uint16_t fill3[3];
};