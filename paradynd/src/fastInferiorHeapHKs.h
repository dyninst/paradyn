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

// fastInferiorHeapHKs.h
// Ari Tamches
// contains houseKeeping (HK) classes used as the first template input type
// to fastInferiorHeap (see fastInferiorHeap.h and .C)

#ifndef _FAST_INFERIOR_HEAP_HKS_H_
#define _FAST_INFERIOR_HEAP_HKS_H_

#include "rtinst/h/rtinst.h" // intCounter, tTimer
#include "metric.h" // metricDefinitionNode
#include "dyninstAPI/src/process.h" 

// The following should provide a useful building-block for intCounterHK,
// wallTimerHK, processTimerHK, etc.
class genericHK {
 protected:
   metricDefinitionNode *mi;
   
   // Needed for GC use:
   struct trampRange {
      unsigned startAddr, endAddr;
   };
   vector<trampRange> trampsUsingMe;

   // since we don't define this, make sure it isn't used:
   genericHK(const genericHK &src);

 public:
   genericHK() {
      // no-param ctor required since the actual HK types need one
      mi = NULL;
      // trampsUsingMe is initialized to empty array
   }
   genericHK(metricDefinitionNode *iMi) {
      mi = iMi;
      // trampsUsingMe is initialized to empty array
   }
  ~genericHK() {}
   genericHK &operator=(const genericHK &src);

   void makePendingFree(const vector<Address> &iTrampsUsing);

   bool tryGarbageCollect(const vector<unsigned> &PCs);
      // Given a list of inferior process PC-registers representing a stack trace
      // (i.e. the current inferior process PC, the PC of its caller, then its
      // caller, etc etc.).  GC will succeed iff none of these PC values fall within
      // any "trampsUsingMe", previously passed to makePendingFree().
};

/* ******************************************************************************* */

// Note that in the future, intCounter (the RAW type) can be trimmed down
// to a single integer; the current "id" bit-field can become part of intCounterHK.

class intCounterHK : public genericHK {
 // assumes intCounter type (rtinst/h/rtinst.h) is the raw type
 private:
   unsigned lowLevelId;
      // can be made obsolete in the future; we keep it for now to aid assert checking

   // Since we don't define this, making it private makes sure it isn't used:
   intCounterHK(const intCounterHK &src);

 public:
   intCounterHK() : genericHK() {
      // no-param ctor is required by fastInferiorHeap
      lowLevelId = 0; // wouldn't UINT_MAX be better?
   }
   intCounterHK(int iCounterId,
                metricDefinitionNode *iMi) : genericHK(iMi) {
      lowLevelId = iCounterId;
   }

  ~intCounterHK() {}
   intCounterHK &operator=(const intCounterHK &src);

   void assertWellDefined() const {
      assert(mi != NULL);
   }

   bool perform(const intCounter &dataValue, process *);
      // we used to pass the wall time, to be used as the sample time.
      // But it seems that unless the sample time is taken at the same
      // time as the sampling of the value, then incorrect values are
      // reported, visible as jagged spikes in the histogram.  This is
      // too bad; it would be nice to take the time once per sample instead
      // of once per counter per sample.
};

class wallTimerHK : public genericHK {
 private:
   unsigned lowLevelId;
      // can be made obsolete in the future; we keep it for now to aid assert checking

   unsigned long long lastTimeValueUsed;
      // to check for rollbacks; formerly stored in inferior heap

   // Since we don't define this, making it private makes sure it isn't used:
   wallTimerHK(const wallTimerHK &src);

   static unsigned normalize;
      // currently always 1000000; in theory, platform-dependent (e.g., for some
      // platform, the timer routines could return cycles instead of usecs)

 public:
   wallTimerHK() : genericHK() {
      // no-param ctor is required by fastInferiorHeap
      lowLevelId = 0;
      lastTimeValueUsed = 0;
   }
   wallTimerHK(int iCounterId,
	       metricDefinitionNode *iMi,
	       unsigned long long iLastTimeValueUsed) : genericHK(iMi) {
      lowLevelId = iCounterId;
      lastTimeValueUsed = iLastTimeValueUsed;
   }
  ~wallTimerHK() {}
   wallTimerHK &operator=(const wallTimerHK &src);

   void assertWellDefined() const {
      assert(mi != NULL);
   }

   bool perform(const tTimer &theTimer, process *);
      // We used to take in a wall time to use as the time-of-sample.  But we've found
      // that the wall time (when used as fudge factor when sampling an active process
      // timer) must be taken at the same time the sample is taken to avoid incorrectly
      // scaled fudge factors, leading to jagged spikes in the histogram (i.e.
      // incorrect samples).  This is too bad; it would be nice to read the wall time
      // just once per paradynd sample, instead of once per timer per paradynd sample.
};

class processTimerHK : public genericHK {
 private:
   unsigned lowLevelId;
      // can be made obsolete in the future; we keep it for now to aid assert checking

   unsigned long long lastTimeValueUsed;
      // to check for rollbacks; formerly stored in inferior heap

   // Since we don't define this, making it private makes sure it isn't used:
   processTimerHK(const processTimerHK &src);

   static unsigned normalize;
      // currently always 1000000; in theory, platform-dependent (e.g., for some
      // platform, the timer routines could return cycles instead of usecs)

 public:
   processTimerHK() : genericHK() {
      // no-param ctor is required by fastInferiorHeap
      lowLevelId = 0;
      lastTimeValueUsed = 0;
   }
   processTimerHK(int iCounterId,
	          metricDefinitionNode *iMi,
	          unsigned long long iLastTimeValueUsed) : genericHK(iMi) {
      lowLevelId = iCounterId;
      lastTimeValueUsed = iLastTimeValueUsed;
   }
  ~processTimerHK() {}
   processTimerHK &operator=(const processTimerHK &src);

   void assertWellDefined() const {
      assert(mi != NULL);
   }

   bool perform(const tTimer &theTimer, process *);
      // We used to take in a wall time to use as the time-of-sample, and a process
      // time to use as the current-process-time for use in fudge factor when sampling
      // an active process timer.  But we've found that both values must be taken at
      // the same time the sample is taken to avoid incorrect values, leading to
      // jagged spikes in the histogram (i.e. incorrect samples).  This is too bad; it
      // would be nice to read these times just once per paradynd sample, instead of
      // once per timer per paradynd sample.
};

#endif
