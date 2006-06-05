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

// Put Paradyn specific thread code in this class, not in
// dyninstAPI/src/dyn_thread.[hC]

#ifndef __PD_THREAD__
#define __PD_THREAD__

#include "dyninstAPI/h/BPatch_thread.h"
#include "paradynd/src/pd_process.h"
#include "rtinst/h/rtinst.h" // virtualTimer, must include or conflict
#include "common/h/Vector.h"

class resource;
class dyn_thread;
class pd_process;
class Frame;
class int_function;

class pd_thread {
   BPatch_thread *dyninst_thread;
   pd_process *pd_proc;
   resource *rid;
   pdstring initial_func_name;

   rawTime64 hw_previous_;
   rawTime64 sw_previous_;

 public:
   pd_thread(BPatch_thread *t, pd_process *p);

   BPatch_thread *get_dyn_thread() { return dyninst_thread; }

   unsigned get_tid() const { return dyninst_thread->getTid(); }
   unsigned get_index() const { return dyninst_thread->getBPatchID(); }
   int get_lwp() const;
   BPatch_function* get_start_func() {return dyninst_thread->getInitialFunc();}

   bool walkStack(BPatch_Vector<BPatch_frame> &stackWalk);
   // Used for catchup
   bool walkStack_ll(pdvector<Frame> &stackWalk);

   bool saveStack(const pdvector<Frame> &stackToSave);
   bool clearSavedStack();

   void update_rid(resource *rid_) { rid = rid_; } 
   resource* get_rid() { return rid; }

   rawTime64 getRawCpuTime_hw();
   rawTime64 getRawCpuTime_sw();

   rawTime64  getInferiorVtime(virtualTimer*, bool&);
   bool       resetInferiorVtime(virtualTimer*);
   pdstring get_initial_func_name() { return initial_func_name; }


 private:
   // Saved stack walk (for catchup)
   pdvector<Frame> savedStack_;
   int savedStackRefCount_;

   // Cross-platform... if there's an OS fd/handle that you ping to get
   // thread time, this is the one to use.
   handleT time_handle_;

   // And the "current" LWP. If we ever go migration, check this before
   // using the handle above to make sure it's the same.
   int currentLWP_;
};



#endif

