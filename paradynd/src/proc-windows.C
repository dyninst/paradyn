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

// $Id: proc-windows.C,v


#include "common/h/timing.h"
#include "common/h/Types.h"
#include "paradynd/src/pd_process.h"
#include "paradynd/src/pd_thread.h"
#include "dyninstAPI/h/BPatch.h"
#include "dyninstAPI/h/BPatch_process.h"
#include "paradynd/src/debug.h"

rawTime64
FILETIME2rawTime64( FILETIME& ft )
{
    return (((rawTime64)(ft).dwHighDateTime<<32) | 
	    ((rawTime64)(ft).dwLowDateTime));
}

void pd_process::initCpuTimeMgrPlt() {
   cpuTimeMgr->installLevel(cpuTimeMgr_t::LEVEL_TWO, &pd_process::yesAvail, 
                            timeUnit(fraction(100)), timeBase::bNone(), 
                            &pd_process::getRawCpuTime_sw,"swCpuTimeFPtrInfo");
}

rawTime64 pd_process::getAllLwpRawCpuTime_hw() {
   rawTime64 total = 0;
   threadMgr::thrIter itr = thrMgr().begin();
   for(; itr != thrMgr().end(); itr++) {
      pd_thread *thr = *itr;
      total += thr->getRawCpuTime_hw();
   }
   return total;
}

extern void printSysError(unsigned errNo);

rawTime64 pd_process::getAllLwpRawCpuTime_sw() {
   rawTime64 total = 0;
   threadMgr::thrIter itr = thrMgr().begin();
   for(; itr != thrMgr().end(); itr++) {
      pd_thread *thr = *itr;
      total += thr->getRawCpuTime_sw();
   }
   return total;
} 

rawTime64 pd_thread::getRawCpuTime_hw() {
  fprintf(stderr, "dyn_lwp::getRawCpuTime_hw: not implemented\n");
  return 0;
}

/* return unit: nsecs */
rawTime64 pd_thread::getRawCpuTime_sw() 
{
  FILETIME kernelT, userT, creatT, exitT;
  rawTime64 now;
  if(GetThreadTimes( (HANDLE) dyninst_thread->os_handle(),
      &creatT, &exitT, &kernelT,&userT)==0) {
    return 0;
  }

  // GetProcessTimes returns values in 100-ns units
  now = (FILETIME2rawTime64(userT)+FILETIME2rawTime64(kernelT));

  // time shouldn't go backwards, but we'd better handle it if it does
//  printf(" %I64d %I64d \n", now, previous);
  if (now < sw_previous_) {
     pdlogLine("********* time going backwards in paradynd **********\n");
     now = sw_previous_;
  }
  else {
     sw_previous_ = now;
  }

  return now;
}

int getNumberOfCPUs() {
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwNumberOfProcessors;
}  
