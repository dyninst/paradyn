/* -*- Mode: C; indent-tabs-mode: true -*- */

#ifndef LINUX_IA64_HDR
#define LINUX_IA64_HDR

#include "arch-ia64.h"
#include "inst-ia64.h"
#include <linux/ptrace.h>

IA64_bundle generateTrapBundle();

struct dyn_saved_regs {
    struct pt_regs pt;
    struct switch_stack ss;
};

/* Removes a #ifdef in linux.C to include this here. */
#include <asm/ptrace_offsets.h>

#endif
