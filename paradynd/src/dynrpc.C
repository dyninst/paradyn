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

/* $Id: dynrpc.C,v 1.130 2007/05/11 21:47:34 legendre Exp $ */

#include "paradynd/src/metricFocusNode.h"
#include "paradynd/src/machineMetFocusNode.h"
#include "paradynd/src/processMetFocusNode.h"
#include "paradynd/src/internalMetrics.h"
#include "dyninstRPC.mrnet.SRVR.h"

#include "common/h/Timer.h"

#include "paradynd/src/resource.h"
#include "paradynd/src/debug.h"
#include "paradynd/src/mdld.h"
#include "paradynd/src/init.h"
#include "paradynd/src/costmetrics.h"
#include "paradynd/src/context.h"
#include "common/h/debugOstream.h"
#include "pdutil/h/hist.h"
#include "paradynd/src/pd_process.h"
#include "paradynd/src/processMgr.h"
#include "pdutil/h/mdlParse.h"
#include "paradynd/src/mdld_data.h"
#include "mrnet/MRNet.h"

int StartOrAttach( void );
extern bool startOnReportSelfDone;
extern pdstring pd_flavor;

timer totalInstTime;

timeLength *imetricSamplingRate = NULL;
timeLength *currSamplingRate = NULL;

const timeLength &getIMetricSamplingRate() {
  if(imetricSamplingRate == NULL) {
    // default to once a second.
    imetricSamplingRate = new timeLength(timeLength::sec());
  }
  return *imetricSamplingRate;
}

void setCurrSamplingRate(timeLength tl) {
  if(currSamplingRate == NULL) {
    currSamplingRate = new timeLength(BASEBUCKETWIDTH_SECS, timeUnit::sec());
  }
  *currSamplingRate = tl;
}

const timeLength &getCurrSamplingRate() {
  if(currSamplingRate == NULL) {
    currSamplingRate = new timeLength(BASEBUCKETWIDTH_SECS, timeUnit::sec());
  }
  return *currSamplingRate;
}

void dynRPC::printStats(MRN::Stream * /* stream */)
{
   fprintf(stderr, "%s[%d]:  If we had 'em, we'd print some stats here\n", FILE__, __LINE__);
}


void dynRPC::coreProcess(MRN::Stream * /* stream */ ,int id)
{
   pd_process *proc = getProcMgr().find_pd_process(id);
   if (proc)
      proc->dumpCore("core.out");
}

pdstring dynRPC::getStatus(MRN::Stream * /* stream */,int id)
{
   pd_process *proc = getProcMgr().find_pd_process(id);
   if (!proc) {
      pdstring ret = pdstring("PID: ") + pdstring(itos(id));
      ret += pdstring(" not found for getStatus\n");
      return (P_strdup(ret.c_str()));
   } 

   if (proc->isTerminated()) return pdstring("exited");
   if (proc->isDetached()) return pdstring("detached"); // might just want "running" here
   if (proc->isStopped()) return pdstring("stopped"); // maybe example stopSignal here?
   return pdstring("running");
}

pdvector<T_dyninstRPC::metricInfo> dynRPC::getAvailableMetrics(MRN::Stream * /* stream */) {
  pdvector<T_dyninstRPC::metricInfo> metInfo;
  unsigned size = internalMetric::allInternalMetrics.size();
  for (unsigned u=0; u<size; u++)
    metInfo += internalMetric::allInternalMetrics[u]->getInfo();
  for (unsigned u2=0; u2< costMetric::allCostMetrics.size(); u2++)
    metInfo += costMetric::allCostMetrics[u2]->getInfo();
  mdl_get_info(metInfo);
  return(metInfo);
}

extern MRN::Stream * defaultStream;
void dynRPC::getPredictedDataCost(MRN::Stream * /* stream */,u_int id, u_int req_id, pdvector<u_int> focus, 
				  pdstring metName, u_int clientID)
{

   if (!metName.length())
     {
       getPredictedDataCostCallback(defaultStream, id, req_id, 0.0,clientID);
     }
   else
     {
       timeLength cost = guessCost(metName, focus);
       // note: returns 0.0 in a variety of situations (if metric cannot be
       //       enabled, etc.)  Would we rather have a more explicit error
       //       return value?
       getPredictedDataCostCallback(defaultStream, id, req_id, 
				    static_cast<float>(cost.getD(timeUnit::sec())), clientID);
     }
}

extern pdvector<int> deferredMetricIDs;

pdvector<int> metricFocusesRequestedForDelete;

void dynRPC::disableDataCollection(MRN::Stream * /* stream */,int mid)
{
  metricFocusesRequestedForDelete.push_back(mid);
}


void deleteMetricFocus(machineMetFocusNode *mi) {
#if defined(sparc_sun_solaris2_4) && defined(TIMINGDEBUG)
    begin_timing(1);
#endif

    if(mi == NULL) {
        // Already deleted
        return;
    }
    
    timeLength cost = mi->cost();
    
    if(cost > currentPredictedCost)
        setCurrentPredictedCost(timeLength::Zero());
    else 
        subCurrentPredictedCost(cost);

    // If this MID is on the deferred list, rip it out
    
    pdvector<int>::iterator itr = deferredMetricIDs.end();
    while (itr != deferredMetricIDs.begin()) {
        itr--;
        int defMID = *itr;
        if (defMID == mi->getMetricID()) {
            deferredMetricIDs.erase(itr);
        }
    }

    // This is a good idea -- let the frontend know that instrumentation was
    // removed and all that. But the frontend logic doesn't understand yet.

#ifdef BROKEN
    // Let the frontend know this instrumentation "failed"
    metFocInstResponse *cbi = mi->getMetricFocusResponse();
    if (cbi) {
        cbi->updateResponse(mi->getMetricID(), inst_insert_failure,
                            "Instrumentation cancelled");
        cbi->makeCallback();
    }
#endif
    delete mi;

#if defined(sparc_sun_solaris2_4) && defined(TIMINGDEBUG)
    end_timing(1,"disable");
#endif
}

void processInstrDeletionRequests() {
   pdvector<pd_process *> procsToCont;
   for(unsigned i=0; i<metricFocusesRequestedForDelete.size(); i++) {
      int curmid = metricFocusesRequestedForDelete[i];
      machineMetFocusNode *mi = machineMetFocusNode::lookupMachNode(curmid);

      if (!mi) {
	// Case: the backend has already deleted the node, and now the
	// frontend is duplicating our work.
	continue;
      }
      pdvector<processMetFocusNode*> procnodes = mi->getProcNodes();
      for(unsigned j=0; j<procnodes.size(); j++) {
         processMetFocusNode *cur_procnode = procnodes[j];
         pd_process *proc = cur_procnode->proc();
         proc->pauseProc();
         procsToCont.push_back(proc);
      }
      
      deleteMetricFocus(mi);
   }
   metricFocusesRequestedForDelete.clear();

   for(unsigned k=0; k<procsToCont.size(); k++) {
      pd_process *proc = procsToCont[k];
      proc->continueProc();
   }
}

bool dynRPC::setTracking(MRN::Stream * /* stream */,unsigned target, bool /* mode */)
{
    resource *res = resource::findResource(target);
    if (res) {
	if (res->isResourceDescendent(moduleRoot)) {
	    res->suppress(true);
	    return(true);
	} else {
	    // un-supported resource hierarchy.
	    return(false);
	}
    } else {
      // cout << "Set tracking target " << target << " not found\n";
      return(false);
    }
}

void dynRPC::resourceInfoResponse(MRN::Stream * /* stream */,pdvector<u_int> temporaryIds, 
																	pdvector<u_int> resourceIds)
{
	assert(temporaryIds.size() == resourceIds.size());
	
	for (unsigned u = 0; u < temporaryIds.size(); u++)
		{
			resource *res = resource::findResource(temporaryIds[u]);
			assert(res);
			res->set_id(resourceIds[u]);
		}
}

void dynRPC::enableDataCollection(MRN::Stream * /* stream */,
                                  pdvector<T_dyninstRPC::focusStruct> focusVector, 
                                  pdvector<pdstring> metric,
                                  pdvector<u_int> mi_ids, 
                                  u_int daemon_id, u_int request_id)
{
    extern u_int sdm_id;

	if(sdm_id != daemon_id)
		return;

    processInstrDeletionRequests();
	
    assert(focusVector.size() == metric.size());
    totalInstTime.start();
	
	metFocInstResponse* cbi = new metFocInstResponse( request_id, daemon_id );
  
    bool responce = false;
  
    for (unsigned j=0; j < metric.size(); j++) {
        responce = startCollecting( metric[j], focusVector[j].focus, mi_ids[j], cbi );
    }
	
	totalInstTime.stop();
	
	cbi->makeCallback();
}

// synchronous, for propogating metrics
T_dyninstRPC::instResponse
dynRPC::enableDataCollection2(MRN::Stream * /* stream */,pdvector<u_int> focus,
                                pdstring met,
                                int mid,
                                u_int daemon_id )
{

  extern u_int sdm_id;
  daemon_id = sdm_id;

  processInstrDeletionRequests();

  totalInstTime.start();

  metFocInstResponse *cbi = new metFocInstResponse( mid, daemon_id );

  startCollecting( met, focus, mid, cbi );
  totalInstTime.stop();
  
  return *cbi;
}

//
// computes new sample multiple value, and modifies the value of the
// symbol _DYNINSTsampleMultiple which will affect the frequency with
// which performance data is sent to the paradyn process 
//
void dynRPC::setSampleRate(MRN::Stream * /* stream */,double sampleInterval)
{
    // TODO: implement this:
    // want to change value of DYNINSTsampleMultiple to corr. to new
    // sampleInterval (sampleInterval % baseSampleInterval) 
    // if the sampleInterval is less than the BASESAMPLEINTERVAL ignore
    // use currSamplingRate to determine if the change to DYNINSTsampleMultiple
    // needs to be made
   timeLength newSampleRate(timeLength(sampleInterval, timeUnit::sec()));
   
   if(newSampleRate != getCurrSamplingRate()){
      // sample_multiple:  .2 sec  => 1 ;  .4 sec => 2 ;  .8 sec => 4
      setCurrSamplingRate(newSampleRate);
      machineMetFocusNode::updateAllAggInterval(newSampleRate);
   }
   return;
}

bool dynRPC::detachProgram(MRN::Stream * /* stream */,int program, bool pause)
{
   pd_process *proc = getProcMgr().find_pd_process(program);
   if (proc)
      return(proc->detachProcess(pause));
   else
      return false;
}

//
// Continue all processes
//
void dynRPC::continueApplication(MRN::Stream * /* stream */)
{
    continueAllProcesses();
}
//
// set the stream used by defaut to do upcalls
// and get and send daemon information
//
int 
dynRPC::setDaemonDefaultStream(MRN::Stream * stream)
{
  extern MRN::Stream * defaultStream;
  defaultStream = stream;

  return 0;
}

void dynRPC::setEquivClassReportStream( MRN::Stream *stream )
{
  extern MRN::Stream * equivClassReportStream;
  equivClassReportStream = stream;
}

void dynRPC::reportInitialResources( MRN::Stream * /* stream */ )
{
    resource::report_ResourcesToFE( );
}

//
// set the stream used by defaut to do upcalls
// and get and send daemon information
//
T_dyninstRPC::daemonInfo
dynRPC::getDaemonInfo(MRN::Stream *,
                      pdvector<T_dyninstRPC::daemonSetupStruc> dss )
{
    extern pdstring machine_name_out;
    extern pdstring program_name;
    extern pdstring flavor_name;
    extern unsigned sdm_id;
	extern bool sdm_id_set;

	for(unsigned i = 0 ; i < dss.size() ; i++) {
        if(machine_name_out == dss[i].daemonName) {
            if(!sdm_id_set) {
                sdm_id = dss[i].daemonId;
                sdm_id_set = true;
            }
            break;
        }
    }
	
    if( !sdm_id_set ) {
        extern MRN::Rank myRank;
        //darnold: we should only get here for manually started daemons
        sdm_id = myRank;
        sdm_id_set=true;
    }

    T_dyninstRPC::daemonInfo * di = new T_dyninstRPC::daemonInfo;
	
    di->machine = machine_name_out;
    di->program = program_name;
    di->pid = getpid();
    di->flavor = flavor_name;
    di->d_id = sdm_id;	
    return *di;
}

//
// Continue a process
//
void dynRPC::continueProgram(MRN::Stream * stream,int pid)
{

   pd_process *proc = getProcMgr().find_pd_process(pid);
   if (!proc) {
      if(getProcMgr().hasProcessExited(pid)) {
         // do nothing, a possible and reasonable condition
         // a process forks, the front-end registers the new forked process
         // the forked process exits (eg. if the process was "ssh cmd &")
         // the front-end signals the daemon to continue the forked process
         // however, the forked process is now exited
         return;
      } else {
         sprintf(errorLine,
                 "Internal error: cannot continue PID %d\n", pid);
         pdlogLine(errorLine);
         showErrorCallback(stream, 62,(const char *) errorLine,
                           machineResource->part_name());
         return;
      }
   }
   if( proc->isStopped()) {
     if (proc->isTerminated()) {
         sprintf(errorLine,
                 "%s[%d]: Internal error: PID %d terminated\n", 
                 __FILE__, __LINE__, pid);
         pdlogLine(errorLine);
         showErrorCallback(stream,62,(const char *) errorLine,
                           machineResource->part_name());

     }
     else
       proc->continueProc();
   }
   // we are no longer paused, are we?
   pdstatusLine("application running");
   if (!markApplicationRunning()) {
       return;
   }
}

//
//  Stop all processes 
//
bool dynRPC::pauseApplication(MRN::Stream * /* stream */ )
{
    pauseAllProcesses();
    return true;
}
 
//
//  Stop a single process
//
bool dynRPC::pauseProgram(MRN::Stream * stream,int program)
{
   pd_process *proc = getProcMgr().find_pd_process(program);
   if (!proc) {
      sprintf(errorLine, "Internal error: cannot pause PID %d\n", program);
      pdlogLine(errorLine);
      showErrorCallback(stream,63,(const char *) errorLine,
		        machineResource->part_name());
      return false;
   }
   return proc->pauseProc();
}

bool dynRPC::startProgram(MRN::Stream * /* stream */,int /* dummy */ )
{
    pdstatusLine("starting application");
    continueAllProcesses();
    return(false);
}

//
// Monitor the dynamic call sites contained in function <function_name>
//
void dynRPC::MonitorDynamicCallSites(MRN::Stream * /* stream */,pdstring function_name){
  processMgr::procIter itr = getProcMgr().begin();
  while(itr != getProcMgr().end()) {
     pd_process *p = *itr++;
     if (!p)
       continue;
     p->MonitorDynamicCallSites(function_name);
  }
}

//
// start a new program for the tool.
//

int dynRPC::addExecutable(MRN::Stream * /* stream */,
                          pdvector<pdstring> argv,
                          pdstring dir)
{
   if (strcmp(dir.c_str(), "") == 0) {
      dir = (char *) NULL;
   }
   pd_process *p = pd_createProcess(argv, dir);
   metricFocusNode::handleNewProcess(p);

   if (p) {
		resource::report_ChecksumToFE( );
      return 1;
   }
   else
      return -1;
}

//
// Attach is the other way to start a process (application?)
// path gives the full path name to the executable, used _only_ to read
// the symbol table off disk.
// values for 'afterAttach': 1 --> pause, 2 --> run, 0 --> leave as is
//

bool dynRPC::attach(MRN::Stream * /* stream */,pdstring progpath, int pid, int afterAttach)
{
  startup_cerr << "WELCOME to dynRPC::attach" << endl;
  startup_cerr << "progpath=" << progpath << endl;
  startup_cerr << "pid=" << pid << endl;
  startup_cerr << "afterAttach=" << afterAttach << endl;
	pd_process *p = pd_attachProcess(progpath, pid);
	
	if (!p) return false;
	
	metricFocusNode::handleNewProcess(p);

   resource::report_ChecksumToFE( );
   return true;
}


//
// report the current time 
//
double dynRPC::getTime(MRN::Stream * /* stream */) {
  return getWallTime().getD(timeUnit::sec(), timeBase::bStd());
}


void
dynRPC::reportSelfDone(MRN::Stream * /* stream */ )
{
    if( startOnReportSelfDone )
    {
        // The front-end is done handling our reportSelf request
        // We can start our process now
        StartOrAttach();
    }
}


void 
dynRPC::send_mdl(MRN::Stream * /* stream */, pdvector<T_dyninstRPC::rawMDL> /*mdlBufs*/ )
{
    // should never be called; pdRPC::send_mdl should be called instead.
    assert( false );
}

void
pdRPC::send_mdl(MRN::Stream * stream, pdvector<T_dyninstRPC::rawMDL> mdlBufs )
{
    assert( !saw_mdl );
    // parse the MDL data we've been given
    for( pdvector<T_dyninstRPC::rawMDL>::const_iterator iter = mdlBufs.begin();
				 iter != mdlBufs.end();
				 iter++ )
			{
        mdlBufPtr = (const char*)iter->buf.c_str();

        mdlBufRemaining = iter->buf.length();
				
        int pret = mdlparse();
        if( pret != 0 )
					{
            // indicate the error
#if READY
            // how to indicate error?
#else
            cerr << "failed to parse MDL"
								 << endl;
#endif // READY
						
            break;
					}
			}
		
    // we've now parsed all MDL data -
    // process it as the front-end processed it
    if( !mdl_apply() )
			{
        // indicate the error
#if READY
        // how to indicate the error?
#else
        cerr << "failed to apply MDL" << endl;
#endif // READY
			}
    else if( !mdl_check_node_constraints() )
			{
        // indicate the error
#if READY
        // how to indicate the error?
#else
        cerr << "MDL node constraint check failed" << endl;
#endif // READY
			}

    // now we've done just what the front-end had done before sending us
    // the raw MDL.
    // 
    // now we process the MDL within our own context
    mdl_data* fe_context = mdl_data::cur_mdl_data;
    mdl_data::cur_mdl_data = new mdld_data();
    mdl_init_be( pd_flavor );
		
    send_stmts(stream, &(fe_context->stmts) );
    send_constraints( stream, &(fe_context->all_constraints) );
    send_metrics(stream, &(fe_context->all_metrics) );
    if( fe_context->lib_constraints.size() > 0 )
			{
        send_libs(stream, &(fe_context->lib_constraints) );
			}
    else
			{
        send_no_libs(stream);
			}
		
    saw_mdl = true;
}

//#define TimeVal2Double(tv) ((tv).tv_sec + (tv).tv_usec / 1000000.0)

pdvector<double> dynRPC::save_LocalClockSkew( MRN::Stream *, double parent_send_time )
{
    pdvector<double> timestamps;

    struct timeval recvTimeVal, sendTimeVal;
    double recvTime, sendTime;

    // sample "receive" time
    gettimeofday(&recvTimeVal, NULL);
		recvTime = ((double)recvTimeVal.tv_sec) + (((double)recvTimeVal.tv_usec)/1000000U);
    //recvTime = TimeVal2Double( recvTimeVal );

    // we send to our parent the time we received its message and the
    // time we sent this message.

    // indicate where our send timestamp should go
    timestamps.push_back( parent_send_time );
    timestamps.push_back( recvTime );
     
    // sample "send" time as late as possible for outgoing message(s)
    gettimeofday( &sendTimeVal, NULL );
		sendTime = ((double)sendTimeVal.tv_sec) + (((double)sendTimeVal.tv_usec)/1000000U);
    //sendTime = TimeVal2Double( sendTimeVal );
    timestamps.push_back( sendTime );

		//fprintf( stderr, "BE: psend=%lf, recv=%lf, send=%lf\n",
    //            parent_send_time, recvTime, sendTime );

    return timestamps;
}

pdvector<double> dynRPC::get_ClockSkew( MRN::Stream * )
{
    pdvector<double> clock_skew_vector;
		double local_result = 0.0;
		clock_skew_vector.push_back(local_result);
    return clock_skew_vector;
}


//We assume that all processes managed by the same daemon
//has the same callgraph and only report that from the 1st
void dynRPC::reportCallGraphEquivClass( MRN::Stream *)
{
    processMgr::procIter itr = getProcMgr().begin();

    assert(itr != getProcMgr().end() );

    pd_process *p = *itr++;
    assert(p);
    p->report_CallGraphChecksumToFE( );
}

void dynRPC::reportStaticCallgraph( MRN::Stream * )
{
    processMgr::procIter itr = getProcMgr().begin();
    assert( itr != getProcMgr().end());
    pd_process *p = *itr++;
    assert(p);
    p->FillInCallGraphStatic();
}

void dynRPC::staticCallgraphReportsComplete( MRN::Stream * )
{
    PARADYN_bootstrapStruct bs_record;

    extern resource *machineResource;
    const bool calledFromExec   = (bs_record.event == 4);
    extern pdRPC *tp;

    processMgr::procIter itr = getProcMgr().begin();
    assert( itr != getProcMgr().end());
    pd_process *p = *itr++;
    assert(p);

    tp->newProgramCallbackFunc(defaultStream, p->getPid(), p->arg_list, 
                               machineResource->part_name(),
                               calledFromExec,
                               p->wasRunningWhenAttached());
    p->setCanReportResources(true);
    p->reportInitialThreads();
    pdstring buffer = pdstring("PID=") + pdstring(itos(p->getPid()));
    buffer += pdstring(", ready");
    pdstatusLine(buffer.c_str());

    extern bool readyToRun;
    readyToRun = false;
    extern void doDelayedReport();
    doDelayedReport();
}

