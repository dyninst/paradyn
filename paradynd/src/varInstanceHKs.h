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

// $Id: varInstanceHKs.h,v 1.12 2006/07/07 00:01:15 jaw Exp $
// contains houseKeeping (HK) classes used as the first template input type
// to fastInferiorHeap (see fastInferiorHeap.h and .C)

#ifndef _VAR_INSTANCE_HKS_H
#define _VAR_INSTANCE_HKS_H

#include "rtinst/h/rtinst.h" // intCounter, tTimer
#include "common/h/Types.h"    // Address
#include "common/h/Time.h"
#include "paradynd/src/papiMgr.h" // HwEvent

class threadMetFocusNode_Val;
class pd_process;

// The following should provide a useful building-block for intCounterHK,
// wallTimerHK, processTimerHK, etc.
class genericHK {
 protected:
   threadMetFocusNode_Val *thrNodeVal;

   HwEvent* hwEvent;
   
   // new vector class wants copy-ctor defined:
   genericHK(const genericHK &src) {
      thrNodeVal = src.thrNodeVal;
   }

 public:
   genericHK() : thrNodeVal(NULL) {
      // trampsUsingMe is initialized to empty array
   }
   virtual ~genericHK();
   genericHK &operator=(const genericHK &src);

   // Given a list of inferior process PC-registers representing a stack
   // trace (i.e. the current inferior process PC, the PC of its caller, then
   // its caller, etc etc.).  GC will succeed iff none of these PC values
   // fall within any "trampsUsingMe", previously passed to
   // makePendingFree().

   void setThrClient(threadMetFocusNode_Val *thrclient);
   threadMetFocusNode_Val* getThrNodeVal() { return thrNodeVal; }

   void setHwEvent(HwEvent* hw) {
     hwEvent = hw;
   }
};

/* ************************************************************************* */

// Note that in the future, intCounter (the RAW type) can be trimmed down to
// a single integer; the current "id" bit-field can become part of
// intCounterHK.

class intCounterHK : public genericHK {
 public:
  typedef intCounter rawType;
  static intCounter initValue(unsigned index);

  intCounterHK() : genericHK() {
  }

  ~intCounterHK() { };
  intCounterHK &operator=(const intCounterHK &src);

  void assertWellDefined() const {
    assert(thrNodeVal != NULL);
  }

  static void initializeAfterFork(rawType *, rawTime64) { }

  bool perform(const intCounter *dataValue, pd_process *);
  // we used to pass the wall time, to be used as the sample time.  But it
  // seems that unless the sample time is taken at the same time as the
  // sampling of the value, then incorrect values are reported, visible as
  // jagged spikes in the histogram.  This is too bad; it would be nice to
  // take the time once per sample instead of once per counter per sample.
};

class wallTimerHK : public genericHK {
 private:
  timeLength lastTimeValueUsed;
  // to check for rollbacks; formerly stored in inferior heap
  
 public:
  typedef tTimer rawType;
  static tTimer initValue(unsigned index);

  wallTimerHK() : genericHK(), lastTimeValueUsed(timeLength::Zero()) {
  }
  
  // new vector class wants copy-ctor to be defined:
  wallTimerHK(const wallTimerHK &src) : genericHK(src), 
    lastTimeValueUsed(timeLength::Zero()) {
  }
  
  ~wallTimerHK() { };
  wallTimerHK &operator=(const wallTimerHK &src);
  
  void assertWellDefined() const {
    assert(thrNodeVal != NULL);
  }
  
  bool perform(const tTimer *theTimer, pd_process *);
  // We used to take in a wall time to use as the time-of-sample.  But we've
  // found that the wall time (when used as fudge factor when sampling an
  // active process timer) must be taken at the same time the sample is taken
  // to avoid incorrectly scaled fudge factors, leading to jagged spikes in
  // the histogram (i.e.  incorrect samples).  This is too bad; it would be
  // nice to read the wall time just once per paradynd sample, instead of
  // once per timer per paradynd sample.
  static void initializeAfterFork(rawType *curElem, rawTime64 curRawTime);
};

class processTimerHK : public genericHK {
 private:
  timeLength lastTimeValueUsed;
  // to check for rollbacks; formerly stored in inferior heap
  
  // The per-thread virtual timer, so that we do not have to
  // recalulate its address everytime
  tTimer* vTimer; 

 public:
  typedef tTimer rawType;
  static tTimer initValue(unsigned index);

  /*
  { // class cons
    memset(&initValue, 0, sizeof(initValue));
  }
  */

  processTimerHK() : genericHK(), lastTimeValueUsed(timeLength::Zero()),
    vTimer(NULL) {
  }
  
  // new vector class wants copy-ctor defined
  processTimerHK(const processTimerHK &src) : genericHK(src), 
    lastTimeValueUsed(timeLength::Zero()),
    vTimer(src.vTimer) // is this right 
  {
  }
  
  ~processTimerHK() { };
  processTimerHK &operator=(const processTimerHK &src);

  static void initializeAfterFork(rawType *curElem, rawTime64 curRawTime);
  
  tTimer* vtimer() const { return vTimer;} 
  void assertWellDefined() const {
    assert(thrNodeVal != NULL);
  }
  
  bool perform(const tTimer *theTimer, pd_process *);
  // We used to take in a wall time to use as the time-of-sample, and a
  // process time to use as the current-process-time for use in fudge factor
  // when sampling an active process timer.  But we've found that both values
  // must be taken at the same time the sample is taken to avoid incorrect
  // values, leading to jagged spikes in the histogram (i.e. incorrect
  // samples).  This is too bad; it would be nice to read these times just
  // once per paradynd sample, instead of once per timer per paradynd sample.
};

#ifdef PAPI

class hwTimerHK : public genericHK {
 private:
  timeLength lastTimeValueUsed;
  // to check for rollbacks; formerly stored in inferior heap
  
  // The per-thread virtual timer, so that we do not have to
  // recalulate its address everytime
  tHwTimer* vTimer; 

 public:
  typedef tHwTimer rawType;
  static tHwTimer initValue(unsigned index);

  hwTimerHK() : genericHK(), lastTimeValueUsed(timeLength::Zero()),
    vTimer(NULL) {
  }
  
  // new vector class wants copy-ctor defined
  hwTimerHK(const hwTimerHK&src) : genericHK(src), 
    lastTimeValueUsed(src.lastTimeValueUsed),
    vTimer(src.vTimer) // is this right 
  {
  }
  
  ~hwTimerHK() { };
 hwTimerHK &operator=(const hwTimerHK &src);

  static void initializeAfterFork(rawType *curElem, rawTime64 curRawTime);
  
  tHwTimer* vtimer() const { return vTimer;} 
  void assertWellDefined() const {
    assert(thrNodeVal != NULL);
  }

  bool perform(const tHwTimer *theTimer, pd_process *);

};

class hwCounterHK : public genericHK {
 public:
  typedef tHwCounter rawType;
  static tHwCounter initValue(unsigned index);

  hwCounterHK() : genericHK() {
  }

  ~hwCounterHK() { };
  hwCounterHK &operator=(const hwCounterHK &src);

  static void initializeAfterFork(rawType *curElem, rawTime64 curRawTime) { }

  void assertWellDefined() const {
    assert(thrNodeVal != NULL);
  }

  bool perform(const tHwCounter *dataValue, pd_process *);
};

#endif

#endif
