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

// templatesPCV.C
// templates for perf consultant and visis

#pragma implementation "List.h"
#include "common/h/List.h"
#include "common/h/String.h"

#pragma implementation "Vector.h"
#include "common/h/Vector.h"

#pragma implementation "BufferPool.h"
#include "paradyn/src/DMthread/BufferPool.h"

#pragma implementation "Dictionary.C"
#include "common/src/Dictionary.C"

#pragma implementation "dyninstRPC.mrnet.h"
#include "dyninstRPC.mrnet.h"
#pragma implementation "visi.xdr.h"
#include "visi.xdr.h"

/* ********************************
 * PCthread stuff
 */

#include "paradyn/src/PCthread/PCintern.h"

#include "paradyn/src/PCthread/PCsearch.h"
#include "paradyn/src/PCthread/PCfilter.h"
#include "paradyn/src/PCthread/PCmetric.h"
#include "paradyn/src/PCthread/PCmetricInst.h"
#include "paradyn/src/PCthread/PCwhy.h"
#include "paradyn/src/PCthread/PCshg.h"
#include "paradyn/src/PCthread/PCdata.h"
#include "paradyn/src/PCthread/PCcostServer.h"
#include "UI.thread.h"

template class pdvector<pdstring*>;
template class pdvector<costServerRec>;
template class pdvector<uiSHGrequest>;
template class pdvector<dataSubscriber*>;
template class pdvector<PCsearch*>;
template class pdvector<filter*>;
template class pdvector<PCMetInfo*>;
template class pdvector<PCmetric*>;
template class pdvector<PCMRec>;
template class pdvector<inPort>;
template class pdvector<searchHistoryNode*>;
template class pdvector<hypothesis*>;
template class pdvector<PCmetricInst*>;

template class dictionary_hash<unsigned, PCsearch*>;
template class pdvector<dictionary_hash<unsigned, PCsearch*>::entry>;

#include "pdutil/src/PriorityQueue.C"
template class PriorityQueue<unsigned, searchHistoryNode*>; 
template ostream &operator<<(ostream &, PriorityQueue<unsigned, searchHistoryNode*> &);
template class pdvector<PriorityQueue<unsigned, searchHistoryNode*>::pair>;

template class dictionary_hash<unsigned,filter*>;
template class pdvector<dictionary_hash<unsigned,filter*>::entry>;
 
template class dictionary_hash<unsigned, searchHistoryNode*>;
template class pdvector<dictionary_hash<unsigned, searchHistoryNode*>::entry>;

template class dictionary_hash<pdstring,PCmetric*>;
template class pdvector<dictionary_hash<pdstring,PCmetric*>::entry>;

template class dictionary_hash<unsigned, PCmetricInst*>;
template class pdvector<dictionary_hash<unsigned, PCmetricInst*>::entry>;

template class dictionary_hash<pdstring, hypothesis*>;
template class pdvector<dictionary_hash<pdstring, hypothesis*>::entry>;

template class dictionary_hash<focus, pdvector<searchHistoryNode*>*>;
template class pdvector<dictionary_hash<focus, pdvector<searchHistoryNode*>*>::entry>;
template class pdvector< pdvector<searchHistoryNode*> *>;

#include "pdutil/src/CircularBuffer.C"
#include "paradyn/src/PCthread/PCintern.h"
template class circularBuffer<PCInterval, PCdataQSize>;

/* ************************************
 * VMthread stuff
 */
#include "paradyn/src/VMthread/VMtypes.h"
#include "VM.thread.h"

template class pdvector<VMactiveStruct *>;
template class pdvector<VMvisisStruct *>;
template class pdvector<VM_visiInfo>;

/* ******************************
 * VISIthread stuff
  */
template class pdvector<metricInstInfo *>;
