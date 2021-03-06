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

// $Id: init-winnt.C,v 1.19 2006/07/07 00:01:12 jaw Exp $

#include "paradynd/src/internalMetrics.h"
#include "paradynd/src/init.h"
#include "paradynd/src/pd_process.h"
#include "common/h/timing.h"
#include "rtinst/h/RThwtimer-winnt.h"
#include "dyninstAPI/h/BPatch_process.h"

bool initOS() {
   //ccw 29 apr 2002 : SPLIT3 initialRequestsPARADYN is changed to
   // initialRequestsPARADYNPARADYN

   pdinstMapping *mapping = new pdinstMapping("execve", "DYNINSTexecFailed",
                                              FUNC_EXIT, BPatch_callAfter, BPatch_firstSnippet);
   initialRequestsPARADYN.push_back(mapping);
   
   return true;
};

// returns units of high-resolution perf counter (can be determined with
// QueryPerformanceFrequency).  The base is arbitrary.
rawTime64 getRawWallTime_hrtime() {
  LARGE_INTEGER time;
  assert(QueryPerformanceCounter(&time) != 0);
  rawTime64 now = static_cast<rawTime64>(time.QuadPart);
  return now;
}
#include "common/h/int64iostream.h"

bool dm_isTSCAvail() {
  return isTSCAvail() != 0;
}

rawTime64 dm_getTSC() {
  rawTime64 v;
  getTSC(v);
  return v;
}

// need to fix this up
void initWallTimeMgrPlt() {
  if(dm_isTSCAvail()) {
    timeStamp curTime = getCurrentTime();  // general util one
    timeLength hrtimeLength(dm_getTSC(), getCyclesPerSecond());
    timeStamp beghrtime = curTime - hrtimeLength;
    timeBase hrtimeBase(beghrtime);
    getWallTimeMgr().installLevel(wallTimeMgr_t::LEVEL_ONE, &dm_isTSCAvail,
				  getCyclesPerSecond(), hrtimeBase,
				  &dm_getTSC, "hwWallTimeFPtrInfo");
  }

  LARGE_INTEGER time;
  assert(QueryPerformanceFrequency(&time) != 0);
  int64_t freq = static_cast<int64_t>(time.QuadPart);
  double cpsTTHz = freq / 10000.0;
  // round it
  cpsTTHz = cpsTTHz + .5;
  int64_t tenThousHz = static_cast<int64_t>(cpsTTHz);
  timeUnit perfCtrFreq(fraction(100000, tenThousHz));

  timeStamp curTime = getCurrentTime();  // general util one
  timeLength hrtimeLength(getRawWallTime_hrtime(), perfCtrFreq);
  timeStamp beghrtime = curTime - hrtimeLength;
  timeBase hrtimeBase(beghrtime);

  getWallTimeMgr().installLevel(wallTimeMgr_t::LEVEL_TWO, yesFunc,
				perfCtrFreq, hrtimeBase,&getRawWallTime_hrtime,
				"swWallTimeFPtrInfo");
}

void pd_process::initOSPreLib()
{
}

/**
 * NT strips the path from the loaded library for
 * recognition purposed
 **/
pdstring formatLibParadynName(pdstring orig)
{
   char dllFilename[_MAX_FNAME];
   _splitpath(orig.c_str(), NULL, NULL, dllFilename, NULL);
   return pdstring(dllFilename);
}

