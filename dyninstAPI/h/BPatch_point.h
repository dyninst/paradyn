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

#ifndef _BPatch_point_h_
#define _BPatch_point_h_

class process;
class instPoint;
class BPatch_thread;
class BPatch_image;
class BPatch_function;


/*
 * Used with BPatch_function::findPoint to specify which of the possible
 * instrumentation points within a procedure should be returned.
 */
typedef enum {
    BPatch_entry,
    BPatch_exit,
    BPatch_subroutine,
    BPatch_longJump,
    BPatch_allLocations,
    BPatch_instruction
} BPatch_procedureLocation;


class BPatch_point {
    friend class BPatch_thread;
    friend class BPatch_image;
    friend class BPatch_function;
    friend class process;
    friend BPatch_point* createInstructionInstPoint(process*proc,void*address);

    process	*proc;
    const BPatch_function	*func;
    instPoint	*point;
    BPatch_procedureLocation pointType;

    BPatch_point(process *_proc, BPatch_function *_func, instPoint *_point,
		 BPatch_procedureLocation _pointType) :
	proc(_proc), func(_func), point(_point), pointType(_pointType) {};
public:
    const BPatch_procedureLocation getPointType() { return pointType; }
    const BPatch_function *getFunction() { return func; }
    BPatch_function *getCalledFunction();
    void            *getAddress();
    int             getDisplacedInstructions(int maxSize, void *insns);

    bool	usesTrap_NP();
};

#endif /* _BPatch_point_h_ */
