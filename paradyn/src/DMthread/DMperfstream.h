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

// $Id: DMperfstream.h,v 1.30 2005/12/20 00:19:33 pack Exp $

#ifndef dmperfstream_H
#define dmperfstream_H

#include "dataManager.thread.h"
#include "dataManager.thread.SRVR.h"
#include <string.h>
#include "paradyn/src/UIthread/Status.h"
#include <stdlib.h>
#include "common/h/Vector.h"
#include "common/h/Dictionary.h"
#include "common/h/Time.h"
#include "paradyn/src/DMthread/BufferPool.h"
#include "DMphase.h"
#include "DMinclude.h"
#include "paradyn/src/DMthread/DVbufferpool.h"


class metricInstance;
class metric;
class resourceList;

struct pred_Cost_Type {
    metricHandle m_handle;
    resourceListHandle rl_handle;
    float max;
    int howmany;
    u_int requestId;
};

typedef struct pred_Cost_Type predCostType;

//
// A consumer of performance data.
//
class performanceStream {
   friend class paradynDaemon;
   friend class dynRPCUser;

 public:
   typedef dictionary_hash<perfStreamHandle,performanceStream*>::iterator
      psIter_t;

   performanceStream(dataType t, dataCallback dc, controlCallback cc,
                     int tid); 
   ~performanceStream();
   void callSampleFunc(metricInstanceHandle mi, pdSample *buckets,
                       int count, int first, phaseType type);
   void callResourceFunc(resourceHandle parent, resourceHandle child, 
                         const char *name, const char *abstr);
   void callUpdateResourceFunc(resourceHandle theResource, 
                               const char * displayname, const char *abstr);
   void callResourceRetireFunc(resourceHandle uniqueID, const char *name);

   void callResourceBatchFunc(batchMode mode);
   void callFoldFunc(timeLength width, phaseType phase_type);
   void callInitialActualValueFunc(metricHandle mi, pdSample initActVal,
                                   phaseType phase_type);
   void callStateFunc(appState state);
   void callPhaseFunc(phaseInfo& phase,bool with_new_pc,bool with_visis);
   void callPredictedCostFuc(metricHandle,resourceListHandle,float,u_int);
   void callDataEnableFunc(pdvector<metricInstInfo> *response,
                           u_int request_Id, u_int last_cb_for_request);
   perfStreamHandle Handle(){return(handle);}
   void flushBuffer();   // send data to client thread
   void signalToFlush();
   void predictedDataCostCallback(u_int,float,u_int);
   static void notifyAllChange(appState state);
   static void ResourceBatchMode(batchMode mode);
   static void foldAll(timeLength width, phaseType phase_type); 
   static performanceStream *find(perfStreamHandle psh);

   // these routines change the size of my_buffer 
   static void addCurrentUser(perfStreamHandle psh);
   static void addGlobalUser(perfStreamHandle psh);
   static void removeCurrentUser(perfStreamHandle psh);
   static void removeGlobalUser(perfStreamHandle psh);
   static void removeAllCurrUsers();
   static bool addPredCostRequest(perfStreamHandle,u_int&,
                                  metricHandle,resourceListHandle,u_int);
   // trace data streams
   void callTraceFunc(metricInstanceHandle, const void *, int);
   void flushTraceBuffer();
   controlCallback getControlCallbackData() const { return controlFunc; }
   int getThreadID() const { return threadId; }
   static void addTraceUser(perfStreamHandle psh);
   static void removeTraceUser(perfStreamHandle psh);

   static psIter_t getAllStreamsIter() {
      return allStreams.begin();
   }

   // send data to client thread
   // static flushBuffer(perfStreamHandle psh);

   static unsigned pshash(const perfStreamHandle &val) {
      return((unsigned)val);
   }

 private:
   dataType           type;   // Trace or Sample
   dataCallback       dataFunc;
   controlCallback    controlFunc;
   int 		      threadId;
   perfStreamHandle   handle;
   u_int              num_global_mis;   // num MI's for global phase
   u_int              num_curr_mis;     // num MI's for curr phase
   u_int              my_buffer_size;   // total number of MI's enabled
   u_int              next_buffer_loc;  // next buffer loc. to fill
   u_int              nextpredCostReqestId;    
   pdvector<dataValueType>   *my_buffer;	// buffer of dataValues
   pdvector<predCostType*>   pred_Cost_buff; // outstanding predCost events
   static u_int       next_id;
   // trace data streams
   u_int              num_trace_mis;   // num MI's for trace
   u_int              my_traceBuffer_size; // fixed; 10
   u_int              next_traceBuffer_loc; // next buffer loc. to fill
   pdvector<traceDataValueType>   *my_traceBuffer; // buffer of trace data

   bool reallocTraceBuffer();  
   // dictionary rather than vector since perfStreams can be destroyed
   static dictionary_hash<perfStreamHandle,performanceStream*> allStreams;
   bool	reallocBuffer();
};
#endif
