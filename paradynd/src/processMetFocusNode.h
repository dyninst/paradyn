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

#ifndef PROC_MET_FOCUS_NODE
#define PROC_MET_FOCUS_NODE

#include "paradynd/src/metricFocusNode.h"
#include "common/h/Dictionary.h"
#include "dyninstAPI/src/frame.h"
#include "paradynd/src/focus.h"
#include "paradynd/src/pd_thread.h"
#include "common/h/Vector.h"

class machineMetFocusNode;
class instrCodeNode;
class threadMetFocusNode;
class threadMetFocusNode_Val;
class instrDataNode;
class catchupReq;
class AstNode;
class dyn_thread;
class pd_thread;
class instReqNode;
class dyn_lwp;

struct catchup_t {
  AstNode *ast;
  dyn_thread *thread;
  dyn_lwp *lwp;
};

struct sideEffect_t {
  Frame frame;
  instReqNode *reqNode;
};
class processMetFocusNode : public metricFocusNode {
 private:
  instrCodeNode *metricVarCodeNode;
  vector<instrCodeNode *> constraintCodeNodes;
  vector<threadMetFocusNode *> thrNodes;
  sampleAggregator aggregator;
  machineMetFocusNode *parentNode;
  aggComponent *aggInfo;  // for machNode <-> procNode link
  pd_process *proc_;

  aggregateOp aggOp;
  timeStamp procStartTime;    // the time that this metric started
                              // need this in updateWithDeltaValue()
  const string metric_name;
  const Focus focus;

  bool dontInsertData_;
  bool currentlyPaused;
  vector<catchup_t >   catchupASTList;
  vector<sideEffect_t> sideEffectFrameList;

  dictionary_hash<unsigned, int> thridsCatchuped;

  // remember the threads that need catchup separately than the threads in
  // the process because pd_process could gain more threads independently
  // of procMetFocusNode being informed
  vector<pd_thread *> threadsNeedingCatchup;
  bool hasBeenCatchuped_;

  // don't have a hash indexed by pid because can have multiple procNodes
  // with same pid
  static vector<processMetFocusNode*> allProcNodes;

  bool isBeingDeleted_;

  processMetFocusNode(pd_process *p, const string &metname,
		      const Focus &component_foc, aggregateOp agg_op, 
		      bool arg_dontInsertData);

  void manuallyTrigger(int mid);
  bool catchupInstrNeeded() const;
  void prepareCatchupInstr(pd_thread *thr);  // do catchup on given thread

  // returns true if any instrumentation was added for codeNodes
  bool updateCodeNodes(pd_thread *thr);
  void markAsCatchuped(const pd_thread *thr) {
     thridsCatchuped[thr->get_tid()] = 1;
  }


 public:
  bool isBeingDeleted() {return isBeingDeleted_; };
  static void getProcNodes(vector<processMetFocusNode*> *procNodes);
  static void getProcNodes(vector<processMetFocusNode*> *procNodes, int pid);

  // use this function to create a new processMetFocusNode in the general case
  static processMetFocusNode *newProcessMetFocusNode(pd_process *p, 
				 const string &metname, const Focus &focus_,
				 aggregateOp agg_op, bool arg_dontInsertData);

  // a copy constructor variant, used when handling fork
  processMetFocusNode(const processMetFocusNode &par, pd_process *childProc);

  ~processMetFocusNode();

  const string getFullName() const {
     return (metric_name + focus.getName());
  }
  const Focus getFocus() const { 
    return focus;
  }
  void setMetricVarCodeNode(instrCodeNode* part);
  void addConstraintCodeNode(instrCodeNode* part);
  void propagateToNewThread(pd_thread *thr);
  void deleteThread(dyn_thread *thr);
  processMetFocusNode* handleExec();
  timeLength cost() const;
  instrCodeNode *getMetricVarCodeNode() {
    return metricVarCodeNode;
  }
  int getMetricID();
  unsigned numThrNodes() { return thrNodes.size(); }
  machineMetFocusNode *getParent() { return parentNode; }
  void recordAsParent(machineMetFocusNode *machNode,
		      aggComponent *childAggInfo);

  void getAllCodeNodes(vector<instrCodeNode *> *vecPtr) {
    (*vecPtr).push_back(metricVarCodeNode);
    for(unsigned i=0; i<constraintCodeNodes.size(); i++)
      (*vecPtr).push_back(constraintCodeNodes[i]);
  }
  void getAllCodeNodes(vector<const instrCodeNode *> *vecPtr) const {
    (*vecPtr).push_back(metricVarCodeNode);
    for(unsigned i=0; i<constraintCodeNodes.size(); i++)
      (*vecPtr).push_back(constraintCodeNodes[i]);
  }
  aggregateOp getAggOp() { return aggOp; }
  void addPart(threadMetFocusNode* thrNode);
  void updateAggInterval(timeLength width) {
    aggregator.changeAggIntervalWidth(width);
  }
  void updateWithDeltaValue(timeStamp startTime, timeStamp sampleTime, 
			    pdSample value);
  void tryAggregation();
  pd_process *proc() { return proc_; }
  bool dontInsertData() { return dontInsertData_; }
  void print();
  void initializeForSampling(timeStamp timeOfCont, pdSample initValue);
  void initAggInfoObjects(timeStamp timeOfCont, pdSample initValue);
  bool hasDeferredInstr();
  bool insertJumpsToTramps();
  bool trampsHookedUp();
  bool instrLoaded();
  bool hasBeenCatchuped() { return hasBeenCatchuped_; }

  // considers the passed thread
  bool hasBeenCatchuped(const pd_thread *thr) {
     return thridsCatchuped.defines(thr->get_tid());
  }

  bool instrInserted() { return (instrLoaded() && trampsHookedUp()
				 && hasBeenCatchuped()); }
  instr_insert_result_t loadInstrIntoApp();
  void doCatchupInstrumentation();
  instr_insert_result_t insertInstrumentation();
  
  vector<const instrDataNode *> getFlagDataNodes() const;
  void prepareForSampling();
  void prepareForSampling(threadMetFocusNode *thrNode);
  void stopSamplingThr(threadMetFocusNode_Val *thrNodeVal);
  bool needToWalkStack() ;  // const;  , make this const in the future

  timeStamp getStartTime() { return procStartTime; }

  void prepareCatchupInstr();  // on all threads
  void postCatchupRPCs();

  void pauseProcess();
  void continueProcess();
  void unFork();
};



#endif
