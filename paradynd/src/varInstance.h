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

#ifndef _VAR_INSTANCE_H_
#define _VAR_INSTANCE_H_


#include "common/h/Types.h"
#include "common/h/Vector.h"
#include "paradynd/src/variableMgrTypes.h"
#include "rtinst/h/rtinst.h"

class threadMetFocusNode_Val;
class Frame;

class baseVarInstance {
 public:
  baseVarInstance() { }
  baseVarInstance(const baseVarInstance &) { }
  virtual ~baseVarInstance() { }
  virtual void *getBaseAddressInDaemon() = 0;
  virtual void *getBaseAddressInApplic() = 0;
  virtual void *elementAddressInDaemon(unsigned thrPos) = 0;
  virtual void *elementAddressInApplic(unsigned thrPos) = 0;
  virtual bool doMajorSample() = 0;
  virtual bool doMinorSample() = 0;
  virtual void markVarAsSampled(unsigned thrPos, 
				threadMetFocusNode_Val *thrNval) = 0;
  virtual void markVarAsNotSampled(unsigned thrPos) = 0;
  virtual void deleteThread(unsigned thrPos) = 0;
  virtual void initializeVarsAfterFork(rawTime64 curRawTime) = 0;
};

class variableMgr;
class pd_process;
class shmMgr;
class HwEvent;

template <class HK>
class varInstance : public baseVarInstance {
 public:
  typedef typename HK::rawType RAWTYPE;
 private:
  unsigned numElems;
  void *baseAddrInDaemon;
  void *baseAddrInApplic;
  pdvector<HK*> hkBuf;
  bool elementsToBeSampled;  // true if currentSamplingSet > 0
  pd_process *proc;
  //RAWTYPE  initValue;
  shmMgr &theShmMgr;
  HwEvent* hwEvent;

  pdvector<unsigned> permanentSamplingSet;
  pdvector<unsigned> currentSamplingSet;

  void createHKifNotPresent(unsigned thrPos);
  bool removeFromSamplingSet(pdvector<unsigned> *set, unsigned thrPosToRemove);

  // --- disallow these -------------------
  varInstance(const varInstance<HK> &par) : baseVarInstance(par),
    theShmMgr(par.theShmMgr) { }
  varInstance &operator=(const varInstance<HK> &) { return *this; }
  // --------------------------------------

 public:
  varInstance(variableMgr &varMgr, 
              //RAWTYPE (*initFunc)(unsigned),
              HwEvent* hwEvent);
  varInstance(const varInstance<HK> &par, shmMgr &sMgr, pd_process *p);

  ~varInstance();
  void *elementAddressInDaemon(unsigned thrPos) {
    RAWTYPE *baseAddr = static_cast<RAWTYPE *>(baseAddrInDaemon);
    return static_cast<void*>(baseAddr + thrPos);  // ptr arith
  }
  void *elementAddressInApplic(unsigned thrPos) {
    RAWTYPE *baseAddr = static_cast<RAWTYPE *>(baseAddrInApplic);
    return static_cast<void*>(baseAddr + thrPos);  // ptr arith
  }
  void *getBaseAddressInDaemon() {
    return baseAddrInDaemon;
  }
  void *getBaseAddressInApplic() {
    return baseAddrInApplic;
  }
  bool doMajorSample() {
    if(permanentSamplingSet.size() == 0) return true;
    else elementsToBeSampled = true;

    currentSamplingSet = permanentSamplingSet;
    return doMinorSample();
  }
  bool doMinorSample();
  void markVarAsSampled(unsigned thrPos, threadMetFocusNode_Val *thrNval);
  void markVarAsNotSampled(unsigned thrPos);
  void deleteThread(unsigned thrPos);

  void initializeVarsAfterFork(rawTime64 curRawTime);
};



#endif



