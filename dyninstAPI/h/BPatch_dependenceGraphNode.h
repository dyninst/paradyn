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

#ifndef _BPatch_dependenceGraphNode_h_
#define _BPatch_dependenceGraphNode_h_

#include "BPatch_instruction.h"
#include "BPatch_Vector.h"
#include "BPatch_eventLock.h"
//#include "BPatch_dependenceGraphEdge.h"

class BPatch_dependenceGraphEdge;

#ifdef DYNINST_CLASS_NAME
#undef DYNINST_CLASS_NAME
#endif
#define DYNINST_CLASS_NAME BPatch_dependenceGraphNode

class BPATCH_DLL_EXPORT BPatch_dependenceGraphNode: public BPatch_eventLock {


  BPatch_instruction* bpinst;
  BPatch_Vector<BPatch_dependenceGraphEdge*>* incoming;
  BPatch_Vector<BPatch_dependenceGraphEdge*>* outgoing;

 public:
  BPatch_Vector<BPatch_dependenceGraphEdge*>* inter;

  // BPatch_dependenceGraphNode::BPatch_dependenceGraphNode
  //
  // constructor

  API_EXPORT_CTOR(Int,(inst),
  BPatch_dependenceGraphNode,(BPatch_instruction* inst));

  API_EXPORT_CTOR(Int,(inst,incList,outList),
  BPatch_dependenceGraphNode,(BPatch_instruction* inst,BPatch_Vector<BPatch_dependenceGraphEdge*>* incList,BPatch_Vector<BPatch_dependenceGraphEdge*>* outList));

  API_EXPORT_V(Int,(out),
  void, getOutgoingEdges,(BPatch_Vector<BPatch_dependenceGraphEdge*>& out))
  
  API_EXPORT_V(Int,(inc),
  void, getIncomingEdges,(BPatch_Vector<BPatch_dependenceGraphEdge*>& inc))

  API_EXPORT(Int,(other_node),
  bool,isImmediateSuccessor,(BPatch_instruction * other_node));

  API_EXPORT(Int,(other_node),
  bool,isImmediatePredecessor,(BPatch_instruction * other_node));

  API_EXPORT(Int,(),
  BPatch_instruction*,getBPInstruction,());

  bool addToIncoming(BPatch_dependenceGraphNode* other_instruction);

  bool addToOutgoing(BPatch_dependenceGraphNode* other_instruction);
};
#endif // _BPatch_dependenceGraphNode_h_

