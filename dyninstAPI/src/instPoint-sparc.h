// instPoint-sparc.h
// sparc-specific definition of class instPoint

/*
 * Copyright (c) 1996 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as Paradyn") on an AS IS basis, and do not warrant its
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

#ifndef _INST_POINT_SPARC_H_
#define _INST_POINT_SPARC_H_

#include "util/h/Types.h" // for "Address" (typedef'd to unsigned)
class pd_Function;
class image;

#include "arch-sparc.h" // for union type "instruction"

typedef enum {
    noneType,
    functionEntry,
    functionExit,
    callSite
} instPointType;

class instPoint {
public:
  instPoint(pd_Function *f, const instruction &instr, const image *owner,
	    Address &adr, const bool delayOK, bool isLeaf,
	    instPointType ipt);
  instPoint(pd_Function *f, const instruction &instr, const image *owner,
            Address &adr, const bool delayOK, instPointType ipt,   
	    Address &oldAddr);
  ~instPoint() {  /* TODO */ }

  // can't set this in the constructor because call points can't be classified
  // until all functions have been seen -- this might be cleaned up
  void set_callee(pd_Function *to) { callee = to; }

  const instruction &insnAtPoint() const { return originalInstruction;}

  const function_base *iPgetFunction() const { return func;      }
  const function_base *iPgetCallee()   const { return callee;    }
  const image         *iPgetOwner()    const { return image_ptr; }
        Address        iPgetAddress()  const { return addr;      }

  Address getTargetAddress() {
      if(!isCallInsn(originalInstruction)) return 0;
       if (!isInsnType(originalInstruction, CALLmask, CALLmatch)) {
	       return 0;  // can't get target from indirect call instructions
       }

      Address ta = (originalInstruction.call.disp30 << 2); 
      return ( addr+ta ); 
  }

  // These are the instructions corresponding to different instrumentation
  // points (the instr point is always the "originalInstruction" instruction,
  // "addr" has the value of the address of "originalInstruction")
  // 
  //  entry point	   entry point leaf	  call site 
  //  -----------	   ----------------       ---------
  //  saveInstr		   originalInstruction    originalInstruction	
  //  originalInstruction  otherInstruction	  delaySlotInsn
  //  delaySlotInsn 	   delaySlotInsn	  aggregateInsn
  //  isDelayedInsn 	   isDelayedInsn
  //  aggregateInsn 
  // 
  //  return leaf		return non-leaf
  //  ----------		---------------
  //  originalInstruction  	originalInstruction
  //  otherInstruction		delaySlotInsn
  //  delaySlotInsn
  //  inDelaySlotInsn
  //  extraInsn
  // 

// TODO: These should all be private
  Address addr;                       // address of inst point
  instruction originalInstruction;    // original instruction
  instruction delaySlotInsn;          // original instruction
  instruction aggregateInsn;          // aggregate insn
  instruction otherInstruction;       
  instruction isDelayedInsn;  
  instruction inDelaySlotInsn;
  instruction extraInsn;   	// if 1st instr is conditional branch this is
			        // previous instruction 
  instruction saveInsn;         // valid only with nonLeaf function entry 

  bool inDelaySlot;             // Is the instruction in a delay slot
  bool isDelayed;		// is the instruction a delayed instruction
  bool callIndirect;		// is it a call whose target is rt computed ?
  bool callAggregate;		// calling a func that returns an aggregate
				// we need to reolcate three insns in this case
  pd_Function *callee;		// what function is called
  pd_Function *func;		// what function we are inst
  bool isBranchOut;             // true if this point is a conditional branch, 
				// that may branch out of the function
  int branchTarget;             // the original target of the branch
  bool leaf;                    // true if the procedure is a leaf
  instPointType ipType;
  int instId;                   // id of inst in this function
  int size;                     // size of multiple instruction sequences
  const image *image_ptr;	// for finding correct image in process
  bool firstIsConditional;      // 1st instruction is conditional branch
  bool relocated_;	        // true if instPoint is from a relocated func

  bool isLongJump;              // true if it turned out the branch from this 
                                // point to baseTramp needs long jump.   
};


#endif
