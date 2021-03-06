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

#include "paradynd/src/threadMetFocusNode.h"
#include "paradynd/src/processMetFocusNode.h"
#include "pdutil/h/pdDebugOstream.h"
#include "paradynd/src/pd_thread.h"
#include "paradynd/src/pd_process.h"
#include "paradynd/src/debug.h"

dictionary_hash<pdstring, threadMetFocusNode_Val*> 
             threadMetFocusNode::allThrMetFocusNodeVals(::Dyninst::stringhash);
             //threadMetFocusNode::allThrMetFocusNodeVals(pdstring::hash);


threadMetFocusNode *threadMetFocusNode::newThreadMetFocusNode(
		   const pdstring &metric_name, const Focus &f, pd_thread *pdthr)
{
  threadMetFocusNode_Val *nodeVal;
  pdstring key_name = threadMetFocusNode_Val::construct_key_name(metric_name, 
							       f.getName());
  bool foundIt = 
    threadMetFocusNode::allThrMetFocusNodeVals.find(key_name, nodeVal);

  if(! foundIt) {
    nodeVal = new threadMetFocusNode_Val(metric_name, f, pdthr);
    allThrMetFocusNodeVals[key_name] = nodeVal;
  }
  nodeVal->incrementRefCount();
  threadMetFocusNode *thrNode = new threadMetFocusNode(nodeVal);

  /*
  cerr << "newThreadMetFocusNode" << key_name << " (" << (void*)thrNode << ")";
  if(foundIt)
    cerr << ", found one ( " << (void*)nodeVal << "), using it\n";
  else
    cerr << ", not found, creating one (" << (void*)nodeVal << ")\n";
  */

  return thrNode;
}

threadMetFocusNode *threadMetFocusNode::copyThreadMetFocusNode(
		    const threadMetFocusNode &par, pd_process *childProc,
		    pd_thread *pdThr)
{
  // we'll use the "create new threadMetFocusNode" method (we'll start out
  // this new threadNode without aggregation and insert it later).  This will
  // do threadNode sharing when it's applicable.
  Focus adjustedFocus = adjustFocusForPid(par.getFocus(), childProc->getPid());
  threadMetFocusNode *newThreadNode = 
    newThreadMetFocusNode(par.getMetricName(), adjustedFocus, pdThr);

  // new threads in the forked process need to have their cumulative
  // value start at zero because the total cpu time starts back at zero
  newThreadNode->getValuePtr()->setCumulativeValue(pdSample::Zero());
  return newThreadNode;
}

threadMetFocusNode_Val::threadMetFocusNode_Val(
                         const threadMetFocusNode_Val &par, BPatch_process *childProc,
			 pd_thread *pdthr)
  : metric_name(par.metric_name), 
    focus(adjustFocusForPid(par.focus, childProc->getPid())),
    cumulativeValue(pdSample::Zero()), allAggInfoInitialized(false),
    pdThr(pdthr), referenceCount(1)
{
}

threadMetFocusNode_Val::~threadMetFocusNode_Val() {
  // /* DEBUG */ fprintf( stderr, "%s[%d]: ~threadMetFocusNode_Val(): reference count %d, this = %p\n", __FILE__, __LINE__, referenceCount, this );
  
  if( referenceCount == 0 ) {
    for( unsigned i = 0; i < parentsBuf.size(); i++ ) {
      processMetFocusNode * parent = parentsBuf[i].parent;
      if( parent != NULL ) {
	    parent->stopSamplingThr( this );
        removeParent( parent );
      }
    } /* end iteration over parents */
  } /* end if reference count is 0. */
} /* end destructor */

threadMetFocusNode::~threadMetFocusNode() {
  // /* DEBUG */ fprintf( stderr, "%s[%d]: ~threadMetFocusNode(), this = %p, & V = %p.\n", __FILE__, __LINE__, this, & V );

  V.decrementRefCount();
  if( V.getRefCount() == 0 ) {
  	// /* DEBUG */ fprintf( stderr, "%s[%d]: ~threadMetFocusNode(), this = %p, & V = %p, reference count == 0\n", __FILE__, __LINE__, this, & V );
    allThrMetFocusNodeVals.undef( V.getKeyName() );

	/* I know this looks horrible, but the constructor initializes it from
	   the dereference of a pointer.  Really, this class should just use a
	   pointer, like varInstance (generic), but whatever, at least before
	   a release. */
	delete & V;
  } else {
  	// /* DEBUG */ fprintf( stderr, "%s[%d]: ~threadMetFocusNode(), this = %p, & V = %p, reference count != 0\n", __FILE__, __LINE__, this, & V );
    V.removeParent( parent );
  }
}

void threadMetFocusNode::recordAsParent(processMetFocusNode *procNode, 
					aggComponent *childAggInfo) {
  assert(parent == NULL);
  parent = procNode;
  V.recordAsParent(procNode, childAggInfo);
}

void threadMetFocusNode_Val::updateAllAggInfoInitialized() {
  bool allInitialized = true;
  for(unsigned i=0; i<parentsBuf.size(); i++) {
    aggComponent *curAggInfo = parentsBuf[i].childNodeAggInfo;
    if(! curAggInfo->isReadyToReceiveSamples())
      allInitialized = false;
  }
  allAggInfoInitialized = allInitialized;
}

void threadMetFocusNode::print() {
   cerr << "T:" << (void*)this << "\n";
}

bool threadMetFocusNode_Val::instrInserted() {
  bool allParentsInserted = true;
  for(unsigned i=0; i<parentsBuf.size(); i++) {
    processMetFocusNode *parent = parentsBuf[i].parent;
    if(parent->instrInserted() == false)
      allParentsInserted = false;
  }
  return allParentsInserted;
}

unsigned threadMetFocusNode_Val::getThreadID() const { 
  return pdThr->get_tid(); 
}
unsigned threadMetFocusNode_Val::getThreadIndex() const { 
  return pdThr->get_index(); 
}

bool threadMetFocusNode_Val::isReadyForUpdates() {
  if(! instrInserted()) {
    sample_cerr << "mi " << this << " hasn't been inserted\n";
    return false;
  }
  if(! hasAggInfoBeenInitialized()) {
    sample_cerr << "mi " << this 
		   << " hasn't had it's agg info initialized yet\n";
    return false;
  }
  return true;
}

// takes an actual value as input, as opindexed to a change in sample value
void threadMetFocusNode_Val::updateValue(timeStamp sampleTime, pdSample value)
{
  assert(value >= pdSample::Zero());
  sample_cerr << "thrNode::updateValue() - mdn: " << (void*)this
		 << ", value: " << value << ", cumVal: " << cumulativeValue 
		 << "\n";

  if (value < cumulativeValue) {
  // only use delta from last sample.
    value = cumulativeValue;
  }

  value -= cumulativeValue;
  cumulativeValue += value;
  timeStamp uninitializedTime;  // we don't know the start of the interval here
  updateWithDeltaValue(uninitializedTime, sampleTime, value);
}

extern bool adjustSampleIfNewMdn(pdSample *sampleVal, timeStamp startTime,
				 timeStamp sampleTime, timeStamp MFstartTime);

void threadMetFocusNode_Val::updateWithDeltaValue(timeStamp startTime,
			timeStamp sampleTime, pdSample value) {
  for(unsigned i=0; i<parentsBuf.size(); i++) {
    processMetFocusNode *curParent = parentsBuf[i].parent;
    aggComponent * thrNodeAggInfo = parentsBuf[i].childNodeAggInfo;
    assert( thrNodeAggInfo != NULL );

    pdSample valToPass = value;
    bool shouldAddSample = adjustSampleIfNewMdn(&valToPass, startTime, 
				  sampleTime, curParent->getStartTime());
    if(!shouldAddSample)   continue;
    sample_cerr << "thrNode::updateWithDeltaVal- (" << i+1 
		   << ") addSamplePt, tm: " << sampleTime << ", val: " 
		   << valToPass << "\n";
    thrNodeAggInfo->addSamplePt(sampleTime, valToPass);
    curParent->tryAggregation();
  }
}

pd_process *threadMetFocusNode_Val::proc() {
  if(parentsBuf.size() > 0) {
    return parentsBuf[0].parent->proc();
  } else
    return NULL;
}

void threadMetFocusNode_Val::recordAsParent(processMetFocusNode *procNode,
					    aggComponent *childAggInfo) {
  // make sure that any added parent is of the same process
  if(parentsBuf.size() > 0) {
    processMetFocusNode *aParent = parentsBuf[0].parent;
    assert(aParent->proc() == procNode->proc());
  }
  parentDataRec<processMetFocusNode> curRec;
  curRec.parent = procNode;
  curRec.childNodeAggInfo = childAggInfo;
  parentsBuf.push_back(curRec);
  if(! childAggInfo->isReadyToReceiveSamples())
    allAggInfoInitialized = false;
}

void threadMetFocusNode_Val::removeParent(processMetFocusNode *procNode) {
   pdvector< parentDataRec<processMetFocusNode> >::iterator parentItr = 
      parentsBuf.end();
   while(parentItr != parentsBuf.begin()) {
      parentItr--;
      if((*parentItr).parent == procNode) {
         (*parentItr).childNodeAggInfo->markAsFinished();
         parentsBuf.erase(parentItr);
      }
   }

   // it's indexsible that one of the parents caused allAggInfoInitialized
   // to be turned off, now this parent could have been deleted and so
   // it's possible allAggInfoInitialized should be changed to true
   // or 
   // assuming we removed one or more parents, we may have just changed
   // things so that the all remaining parents' childAggInfos are 
   // ready to receive samples  
   // Here's one example:
   // a metric-focus with a whole program focus is sharing a threadNode
   // with a metric-focus with a process focus where this process is the
   // child process of a recently forked process.  When we added this child
   // procNode as a parent to the shared thrNodeVal, the allAggInfoInitialized
   // got turned off (in recordAsParent) since the agg info for the child
   // process wasn't initialized yet.  Now in the case that this child
   // procMetFocusNode has it's instrumentation deleted (ie. "unforked")
   // we delete this procMetFocusNode.  And then since this procMetFocusNode
   // is no longer a parent of this thrMetFocusNode_Val we can turn the
   // allAggInfoInitialized back to true.
   updateAllAggInfoInitialized();
}

pdstring threadMetFocusNode_Val::getKeyName() {
  return construct_key_name(metric_name, focus.getName());
}

pd_process *threadMetFocusNode::proc() {
  assert(parent != NULL);
  return parent->proc();
}

// initializes agg info objects between this THR node and parent PROC NODE
void threadMetFocusNode::initAggInfoObjects(timeStamp startTime, 
					    pdSample initValue)
{
   for(unsigned i=0; i<V.parentsBuf.size(); i++) {  
      processMetFocusNode *curParent  = V.parentsBuf[i].parent;    
      if(parent == curParent) {
	 aggComponent * thrNodeAggInfo = V.parentsBuf[i].childNodeAggInfo;
	 assert( thrNodeAggInfo != NULL );
	 if(! thrNodeAggInfo->isInitialized()) {
	    thrNodeAggInfo->setInitialStartTime(startTime);
	    thrNodeAggInfo->setInitialActualValue(initValue);
	 }
      }
   }
   updateAllAggInfoInitialized();
}

void threadMetFocusNode::initializeForSampling(timeStamp startTime, 
					       pdSample initValue)
{
  initAggInfoObjects(startTime, initValue);
  parent->prepareForSampling(this);
}

