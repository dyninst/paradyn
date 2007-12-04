/*
 * Copyright (c) 1996-2004 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as "Paradyn") on an AS IS basis, and do not warrant its
 * validity or performance.  We reserve the right to update, modify,
 * or discontinue this software at any time.  We shall have no
 * obligation to supply such updates or modifications or any other
 * form of support to you.
 * 
 * This license is for research uses.  For such uses, there is no
 * charge. We define "research use" to mean you may freely use it
 * inside your organization for whatever purposes you see fit. But you
 * may not re-distribute Paradyn or parts of Paradyn, in any form
 * source or binary (including derivatives), electronic or otherwise,
 * to any other organization or entity without our permission.
 * 
 * (for other uses, please contact us at paradyn@cs.wisc.edu)
 * 
 * All warranties, including without limitation, any warranty of
 * merchantability or fitness for a particular purpose, are hereby
 * excluded.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * Even if advised of the possibility of such damages, under no
 * circumstances shall we (or any other person or entity with
 * proprietary rights in the software licensed hereunder) be liable
 * to you or any third party for direct, indirect, or consequential
 * damages of any character regardless of type of action, including,
 * without limitation, loss of profits, loss of use, loss of good
 * will, or computer failure or malfunction.  You agree to indemnify
 * us (and any other person or entity with proprietary rights in the
 * software licensed hereunder) for any and all liability it may
 * incur to third parties resulting from your use of Paradyn.
 */

/*
 * inst-power.h - Common definitions to the POWER specific instrumentation code.
 * $Id: inst-power.h,v 1.34 2007/12/04 17:58:02 bernat Exp $
 */

#ifndef INST_POWER_H
#define INST_POWER_H


/* "pseudo" instructions that are placed in the tramp code for the inst funcs
 *   to patch up.   This must be invalid instructions (any instruction with
 *   its top 10 bits as 0 is invalid (technically UNIMP).
 *
 */

#define DEAD_REG              0
#define LIVE_REG              1
#define LIVE_UNCLOBBERED_REG  2
#define LIVE_CLOBBERED_REG    3

#define GPRSIZE_32            4
#define GPRSIZE_64            8
#define FPRSIZE               8

#define REG_SP		      1		
#define REG_TOC               2   /* TOC anchor                            */
// REG_GUARD_OFFSET and REG_GUARD_VALUE could overlap.
#define REG_GUARD_ADDR        5   /* Arbitrary                             */
#define REG_GUARD_VALUE       6
#define REG_GUARD_OFFSET      6

#define REG_COST_ADDR         5
#define REG_COST_VALUE        6

#define REG_SCRATCH          10

#define REG_MT_POS           12   /* Register to reserve for MT implementation */
#define NUM_INSN_MT_PREAMBLE 26   /* number of instructions required for   */
                                  /* the MT preamble.                      */ 

// The stack grows down from high addresses toward low addresses.
// There is a maximum number of bytes on the stack below the current
// value of the stack frame pointer that a function can use without
// first establishing a new stack frame.  When our instrumentation
// needs to use the stack, we make sure not to write into this
// potentially used area.  AIX documentation stated 220 bytes as
// the maximum size of this area.  64-bit PowerPC ELF ABI Supplement,
// Version 1.9, 2004-10-23, used by Linux, stated 288 bytes for this
// area.  We skip the larger number of bytes (288) to be safe on both
// AIX and Linux, 32-bit and 64-bit.
#define STACKSKIP          288

// Both 32-bit and 64-bit PowerPC ELF ABI documents for Linux state
// that the stack frame pointer value must always be 16-byte (quadword)
// aligned.  Use the following macro on all quantities used to
// increment or decrement the stack frame pointer.
#define ALIGN_QUADWORD(x)  ( ((x) + 0xf) & ~0xf )  //x is positive or unsigned

#define GPRSAVE_32 (14*4)
#define GPRSAVE_64 (14*8)
#define FPRSAVE    (14*8)
#define SPRSAVE_32 (6*4+8)
#define SPRSAVE_64 (6*8+8)
#define PDYNSAVE   (8)
#define FUNCSAVE   (14*4)
#define FUNCARGS   32
#define LINKAREA   24

#if defined(os_aix)
#define PARAM_OFFSET(mutatee_address_width) (14*4)  //FIXME for AIX 64-bit
#elif defined(os_linux)
#define PARAM_OFFSET(mutatee_address_width)                         \
        (                                                           \
            ((mutatee_address_width) == sizeof(uint64_t))           \
            ? (   /* 64-bit ELF PowerPC Linux                   */  \
                  sizeof(uint64_t) +  /* TOC save               */  \
                  sizeof(uint64_t) +  /* link editor doubleword */  \
                  sizeof(uint64_t) +  /* compiler doubleword    */  \
                  sizeof(uint64_t) +  /* LR save                */  \
                  sizeof(uint64_t) +  /* CR save                */  \
                  sizeof(uint64_t)    /* Stack frame back chain */  \
              )                                                     \
            : (   /* 32-bit ELF PowerPC Linux                   */  \
                  sizeof(uint32_t) +  /* LR save                */  \
                  sizeof(uint32_t)    /* Stack frame back chain */  \
              )                                                     \
        )
#else
#error "Unknown operating system in inst-power.h"
#endif

// The number of registers (general and floating point) that aren't
// used in the base tramp explicitly 
#define NUM_LO_REGISTERS 18


// Okay, now that we have those defined, let's define the offsets upwards
#define TRAMP_FRAME_SIZE_32 ALIGN_QUADWORD(STACKSKIP + GPRSAVE_32 + FPRSAVE \
                                           + SPRSAVE_32 + PDYNSAVE \
                                           + FUNCSAVE + FUNCARGS + LINKAREA)
#define TRAMP_FRAME_SIZE_64 ALIGN_QUADWORD(STACKSKIP + GPRSAVE_64 + FPRSAVE \
                                           + SPRSAVE_64 + PDYNSAVE \
                                           + FUNCSAVE + FUNCARGS + LINKAREA)
#define PDYN_RESERVED (LINKAREA + FUNCARGS + FUNCSAVE)
#define TRAMP_SPR_OFFSET (PDYN_RESERVED + PDYNSAVE) /* 4 for LR */
#define STK_GUARD (PDYN_RESERVED)
#define STK_LR       (              0)
#define STK_CR_32    (STK_LR      + 4)
#define STK_CTR_32   (STK_CR_32   + 4)
#define STK_XER_32   (STK_CTR_32  + 4)
#define STK_SPR0_32  (STK_XER_32  + 4)
#define STK_FP_CR_32 (STK_SPR0_32 + 4)

#define STK_CR_64    (STK_LR      + 8)
#define STK_CTR_64   (STK_CR_64   + 8)
#define STK_XER_64   (STK_CTR_64  + 8)
#define STK_SPR0_64  (STK_XER_64  + 8)
#define STK_FP_CR_64 (STK_SPR0_64 + 8)

#define TRAMP_FPR_OFFSET_32 (TRAMP_SPR_OFFSET + SPRSAVE_32)
#define TRAMP_GPR_OFFSET_32 (TRAMP_FPR_OFFSET_32 + FPRSAVE)

#define TRAMP_FPR_OFFSET_64 (TRAMP_SPR_OFFSET + SPRSAVE_64)
#define TRAMP_GPR_OFFSET_64 (TRAMP_FPR_OFFSET_64 + FPRSAVE)

#define FUNC_CALL_SAVE (LINKAREA + FUNCARGS)
               

///////////////////////////// Multi-instruction sequences
class codeGen;

void saveSPR(codeGen &gen,
             Register scratchReg,
             int sprnum,
             int stkOffset);
void restoreSPR(codeGen &gen,
                Register scratchReg,
                int sprnum,
                int stkOffset);
void saveLR(codeGen &gen,
            Register scratchReg,
            int stkOffset);
void restoreLR(codeGen &gen,
               Register scratchReg,
               int stkOffset);
void setBRL(codeGen &gen,
            Register scratchReg,
            long val,
            unsigned ti); // We're lazy and hand in the next insn
void saveCR(codeGen &gen,
            Register scratchReg,
            int stkOffset);
void restoreCR(codeGen &gen,
               Register scratchReg,
               int stkOffset);
void saveFPSCR(codeGen &gen,
               Register scratchReg,
               int stkOffset);
void restoreFPSCR(codeGen &gen,
                  Register scratchReg,
                  int stkOffset);
void saveRegister(codeGen &gen,
                  Register reg,
                  int save_off);
// We may want to restore a _logical_ register N
// (that is, the save slot for N) into a different reg.
// This avoids using a temporary
void restoreRegister(codeGen &gen,
                     Register source,
                     Register dest,
                     int save_off);
// Much more common case
void restoreRegister(codeGen &gen,
                     Register reg,
                     int save_off);
void saveFPRegister(codeGen &gen,
                    Register reg,
                    int save_off);
// See above...
void restoreFPRegister(codeGen &gen,
                       Register source,
                       Register dest,
                       int save_off);
void restoreFPRegister(codeGen &gen,
                       Register reg,
                       int save_off);
void pushStack(codeGen &gen);
void popStack(codeGen &gen);
unsigned saveGPRegisters(codeGen &gen, 
                         registerSpace *theRegSpace,
                         int save_off);
unsigned restoreGPRegisters(codeGen &gen, 
                            registerSpace *theRegSpace,
                            int save_off);
unsigned saveFPRegisters(codeGen &gen, 
                         registerSpace *theRegSpace,
                         int save_off);
unsigned restoreFPRegisters(codeGen &gen,
                            registerSpace *theRegSpace,
                            int save_off);
unsigned saveSPRegisters(codeGen &gen, 
                         int save_off);
unsigned restoreSPRegisters(codeGen &gen, 
                            int save_off);

#endif
