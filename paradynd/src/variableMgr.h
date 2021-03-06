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

// $Id: variableMgr.h,v 1.9 2004/03/23 01:12:38 eli Exp $
// The variableMgr class is the top-level view of the actual
// shared, sampled counters and timers. The provides a logical way 
// to reference the counters and timers (called variables) for creation,
// deletion, and sampling.
//
// Organization: the variableMgr consists of several varTables. Each
// varTable is associated with a single type of variable (counter, procTimer,
// wallTimer). The variableMgr basically just routes creation/deletion
// and sampling requests to the appropriate varTable.

#ifndef _VARIABLE_MGR_H_
#define _VARIABLE_MGR_H_

#include "common/h/Vector.h"
#include "paradynd/src/variableMgrTypes.h"

class pd_process;
class pd_thread;
class shmMgr;
class baseVarTable;
class threadMetFocusNode_Val;
class HwEvent;

class variableMgr {
 private:
  unsigned maxNumberOfThreads; // Set at creation time and unchangeable

  // This is where we'd add another type to be sampled. 
  pdvector<baseVarTable *> varTables;

  // One variableMgr per process.
  pd_process *applicProcess;
  shmMgr &theShmMgr;

  variableMgr(const variableMgr &);             // disallow this
  variableMgr &operator=(const variableMgr &);  // disallow this
 public:
  variableMgr(pd_process *proc, shmMgr *shmMgr, 
	      unsigned i_currMaxNumberOfThreads);
  variableMgr(const variableMgr &parentVarMgr, pd_process *proc,
	      shmMgr *shmMgr_);
  
  ~variableMgr();

  // only used by a MT process
  unsigned getMaxNumberOfThreads() { return maxNumberOfThreads; }
  shmMgr &getShmMgr() { return theShmMgr; }
  pd_process *getApplicProcess() { return applicProcess; }
  
  inst_var_index allocateForInstVar(inst_var_type varType, HwEvent* hw);
  
  void markVarAsSampled(inst_var_type varType, inst_var_index varIndex,
		       unsigned thrPos, threadMetFocusNode_Val *thrNval) const;
  
  void markVarAsNotSampled(inst_var_type varType, inst_var_index varIndex,
			   unsigned thrPos) const;
  
  void free(inst_var_type varType, inst_var_index varIndex);
  
  void *shmVarDaemonAddr(inst_var_type varType, inst_var_index varIndex) const;
  
  void *shmVarApplicAddr(inst_var_type varType, inst_var_index varIndex) const;

  // does doMajorSample for three types (intCounter, wallTimer and procTimer)
  bool doMajorSample();
  
  // does doMinorSample for three types (intCounter, wallTimer and procTimer)
  bool doMinorSample();
  
  void handleExec();
  void forkHasCompleted();
  
  unsigned getCurrentNumberOfThreads();

  void deleteThread(pd_thread *thr);
  void initializeVarsAfterFork();
};

#endif
