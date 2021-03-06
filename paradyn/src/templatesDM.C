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

//
// $Id: templatesDM.C,v 1.42 2007/03/22 19:56:32 legendre Exp $
// templates for DMthread, excluding igen templates
//

#pragma implementation "List.h"
#include "common/h/List.h"
#include "common/h/String.h"

#pragma implementation "Vector.h"
#include "common/h/Vector.h"

#pragma implementation "BufferPool.h"
#include "paradyn/src/DMthread/BufferPool.h"

#pragma implementation "Dictionary.h"
#include "common/src/Dictionary.C"

#pragma implementation "dyninstRPC.mrnet.h"
#include "dyninstRPC.mrnet.h"
#pragma implementation "visi.xdr.h"
#include "visi.xdr.h"

#include "paradyn/src/DMthread/DMabstractions.h"

/*
 * Handy ready-to-use templates for the templates


  ****** dictionary_hash (no iter) *********
template class pdpair<>;
template class dictionary<>;
template class pdvector< pdpair<> >;
template class pdvector< dictionary_hash<>::hash_pair>;
template class pdvector< pdvector< dictionary_hash<>::hash_pair> >;
template class dictionary_hash<>;

  ****** dictionary_hash (iter) ************
template class dictionary<>;
template class pdpair<>;
template class pdvector< pdpair<> >; 
template class dictionary_hash<>;
template class dictionary_hash_iter<>;
template class pdvector< dictionary_hash<> :: hash_pair >;
template class pdvector< pdvector< dictionary_hash<> :: hash_pair > >;

  ******* priority queue ********************
template class PriorityQueue<>; 
template ostream &operator<<(ostream &, PriorityQueue<> &);
template class pdvector<PriorityQueue<>::pdpair>;

*/

template class pdvector<bool>;
template class pdvector< pdvector<pdstring> >;
class paradynDaemon;
template class dictionary_hash<unsigned, paradynDaemon*>;
template class pdvector < dictionary_hash<unsigned, paradynDaemon*> ::entry>;

/* *********************************   
 * DMthread stuff
 */

#include "dataManager.thread.h"
#include "dyninstRPC.mrnet.CLNT.h"
class cpContext;
#include "paradyn/src/DMthread/DMdaemon.h"
#include "paradyn/src/DMthread/DMmetric.h"
#include "paradyn/src/DMthread/DMresource.h"
#include "paradyn/src/DMthread/DMperfstream.h"
#include "paradyn/src/DMthread/DMinclude.h"
#include "paradyn/src/DMthread/DVbufferpool.h"

template class pdvector<phaseInfo *>;
template class pdvector<daemonEntry*>;
template class pdvector<paradynDaemon*>;
template class pdvector<executable*>;
template class pdvector<component*>;
template class pdvector<aggComponent*>;
template class pdvector<processMetFocusNode *>;
template class pdvector<metric_focus_pair>;
template class pdvector<metricInstInfo>;
template class pdvector<met_name_id>;
template class pdvector<metric*>;
template class pdvector<resource*>;
template class pdvector<const resource*>;
template class pdvector<resourceList*>;
template class pdvector<metricInstance*>;
template class pdvector<ArchiveType *>;
template class pdvector<rlNameIdType>;

template class dictionary_hash<pdstring,metric*>;
template class pdvector<dictionary_hash<pdstring,metric*>::entry>;

template class dictionary_hash<pdstring,abstraction*>;
template class pdvector<dictionary_hash<pdstring,abstraction*>::entry>;

template class pdvector<dataValueType>;
template class BufferPool<dataValueType>;

template class pdvector<predCostType*>;
template class pdvector<metricRLType>;

// trace data streams
template class pdvector<traceDataValueType>;
template class BufferPool<traceDataValueType>;

template class dictionary_hash<pdstring, resource*>;
template class pdvector<dictionary_hash<pdstring, resource*>::entry>;

template class dictionary_hash<unsigned, resource*>;
template class pdvector<dictionary_hash<unsigned, resource*>::entry>;
template class dictionary_hash_iter<unsigned, resource*>;

template class dictionary_hash<unsigned, resourceHandle>;
template class pdvector<dictionary_hash<unsigned, resourceHandle>::entry>;

template class dictionary_hash<unsigned int, cpContext*>;
template class pdvector<dictionary_hash<unsigned int, cpContext*>::entry>;
template class pdvector<cpContext *>;

template class dictionary_hash<pdstring, resourceList*>;
template class pdvector<dictionary_hash<pdstring, resourceList*>::entry>;

template class dictionary_hash<perfStreamHandle,performanceStream*>;
template class pdvector<dictionary_hash<perfStreamHandle,performanceStream*>::entry>;

template class dictionary_hash<metricInstanceHandle,metricInstance*>;
template class pdvector<dictionary_hash<metricInstanceHandle,metricInstance*>::entry>;

template class pdvector<perfStreamEntry>;

//Blizzard
template class dictionary_hash<pdstring,unsigned>;
template class pdvector<dictionary_hash<pdstring,unsigned>::entry>;

#include <common/src/List.C>
template class List<aggComponent*>;
template class ListBase<aggComponent*, void*>;

class CallGraph;
template class dictionary_hash<int,CallGraph*>;
template class pdvector<CallGraph*>;
template class pdvector<dictionary_hash<int,CallGraph*>::entry>;

template class dictionary_hash<resource*,pdvector<resource*> >;
template class pdvector<pdvector<resource*> >;
template class pdvector<dictionary_hash<resource*,pdvector<resource*> >::entry>;
template class dictionary_hash<const resource*,pdvector<const resource*> >;
template class pdvector<dictionary_hash<const resource*,pdvector<const resource*> >::entry>;

template class dictionary_hash<resource*, int>;
template class pdvector<dictionary_hash<resource*, int>::entry>;
template class dictionary_hash<const resource*, int>;
template class pdvector<dictionary_hash<const resource*, int>::entry>;

#include "paradyn/src/DMthread/DMmetricFocusReqBundle.h"
template class pdvector<metricFocusReqBundle *>;
template class dictionary_hash<unsigned, metricFocusReq *>;
template class pdvector<dictionary_hash<unsigned, metricFocusReq *>::entry>;

#include "paradyn/src/DMthread/DMmetricFocusReq.h"
template class pdvector<metricFocusReq *>;

template class dictionary_hash<unsigned, metricFocusReq_Val*>;
template class pdvector<dictionary_hash<unsigned, metricFocusReq_Val*>::entry>;
template class pdvector<metricFocusReq_Val*>;

template class dictionary_hash<unsigned, inst_insert_result_t>;
template class pdvector<dictionary_hash<unsigned,inst_insert_result_t>::entry>;

template class pdvector<paradynDaemon::MPICHWrapperInfo>;
template class dictionary_hash<pdstring, pdvector<paradynDaemon*> >;
template class pdvector<dictionary_hash<pdstring,
                    pdvector<paradynDaemon*> >::entry>;

template class pdvector<int>;

template class dictionary_hash<unsigned, pdvector<unsigned> >;
template class dictionary_hash<unsigned, pdvector<T_dyninstRPC::focusStruct> >;
template class pdvector<dictionary_hash<unsigned, pdvector<T_dyninstRPC::focusStruct> >::entry >;
template class pdvector<dictionary_hash<unsigned, pdvector<unsigned> >::entry >;
template class pdvector<dictionary_hash<unsigned, pdvector<pdstring> >::entry>;
template class dictionary_hash<unsigned, pdvector<pdstring> >;


