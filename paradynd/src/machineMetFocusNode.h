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

#ifndef MACHINE_MET_FOCUS_NODE
#define MACHINE_MET_FOCUS_NODE

#include "paradynd/src/metric.h"
#include "common/h/Dictionary.h"


class processMetFocusNode_Val;
class threadMetFocusNode_Val;

class machineMetFocusNode : public metricDefinitionNode {
 private:
  bool partsNeedingInitializing;
  vector<processMetFocusNode*> procNodes;
  sampleAggregator aggregator;
  int id_;
  aggregateOp aggOp;
  timeStamp machStartTime;    // the time that this metric-focus started
                              // need this in updateWithDeltaValue()
  bool aggInfoInitialized;
  bool _sentInitialActualValue;        //  true when initial actual value has
                                       //  been sent to the front-end
  string met_;			     // what type of metric
  string flat_name_;
  vector< vector<string> > focus_;
  vector<process *> currentlyPausedProcs;
  bool enable;

  bool loadInstrIntoApp(pd_Function **func);
  bool hasAggInfoBeenInitialized() { return aggInfoInitialized; }
  void doCatchupInstrumentation();
  bool insertJumpsToTramps();

  static dictionary_hash<unsigned, machineMetFocusNode*> allMachNodes;

 public:
  static void getMachineNodes(vector<machineMetFocusNode*> *machNodes);
  static machineMetFocusNode *lookupMachNode(int mid) {
    machineMetFocusNode *foundMachNode;
    bool foundIt = allMachNodes.find(mid, foundMachNode);
    if(foundIt==true)  return foundMachNode;
    else               return NULL;
  }

  machineMetFocusNode(int metricID, const string& metric_name, 
		      const vector< vector<string> >& foc,
		      const string& flat_name,
		      vector<processMetFocusNode*>& parts,
		      aggregateOp agg_op, bool enable_);
  ~machineMetFocusNode();
  const string &getMetName() const { return met_; }
  const string &getFullName() const { return flat_name_; }
  const vector< vector<string> > &getFocus() const {return focus_;}
  vector<processMetFocusNode*> getProcNodes() { return procNodes; }
  static void updateAllAggInterval(timeLength width);
  void updateAggInterval(timeLength width);
  void mapSampledDRNs2ThrNodes();
  aggregateOp getAggOp() { return aggOp; }
  void forwardSimpleValue(timeStamp, timeStamp, pdSample);
  void sendInitialActualValue(pdSample s);
  bool sentInitialActualValue() {  return _sentInitialActualValue; }
  bool procsPaused() { return currentlyPausedProcs.size() > 0; }
  bool isEnabled() { return enable; }
  void pauseProcesses();
  void deleteProcNode(processMetFocusNode *procNode);
  bool insertInstrumentation();
  void initializeForSampling(timeStamp timeOfCont, pdSample initValue);
  void continueProcesses();
  bool hasDeferredInstr();
  bool instrLoaded();
  bool baseTrampsHookedUp();
  bool instrInserted() { return (instrLoaded() & baseTrampsHookedUp()); }
  void addPart(processMetFocusNode* thrNode);
  void print();
  int getMetricID() const { return id_; }
  timeLength cost() const;
  void endOfDataCollection();
  timeStamp getStartTime() { return machStartTime; }

  void updateWithDeltaValue(timeStamp startTime, timeStamp sampleTime, 
			    pdSample value);
  void tryAggregation();
};





#endif
