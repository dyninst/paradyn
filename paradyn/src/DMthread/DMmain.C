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

// $Id: DMmain.C,v 1.169 2006/05/10 11:40:05 darnold Exp $

#include <assert.h>
extern "C" {
#include <malloc.h>
#include <stdio.h>
}

#include "paradyn/src/pdMain/paradyn.h"
#include "pdthread/h/thread.h"
#include "paradyn/src/TCthread/tunableConst.h"
#include "dataManager.thread.SRVR.h"
#include "dyninstRPC.mrnet.CLNT.h"
#include "DMdaemon.h"
#include "DMmetric.h"
#include "DMperfstream.h"
#include "paradyn/src/UIthread/Status.h"

#include "common/h/Vector.h"
#include "common/h/Dictionary.h"
#include "common/h/String.h"
#include "common/h/Time.h"
#include "common/h/timing.h"
// trace data streams
#include "pdutil/h/ByteArray.h"
#include "DMphase.h"
#include "DMmetricFocusReqBundle.h"

#include "common/h/Ident.h"

#include "CallGraph.h"

#include "paradyn/src/met/metricExt.h"

#if !defined(i386_unknown_nt4_0)
#include "termWin.xdr.CLNT.h"
#endif // !defined(i386_unknown_nt4_0)

extern "C" const char V_paradyn[];
extern const Ident V_id;

#if defined(i386_unknown_nt4_0)
#define	S_ISDIR(m)	(((m)&0xF000) == _S_IFDIR)
#define	S_ISREG(m)	(((m)&0xF000) == _S_IFREG)
#endif // defined(i386_unknown_nt4_0)

typedef pdvector<pdstring> blahType;

// bool parse_metrics(pdstring metric_file);

// this has to be declared before baseAbstr, cmfAbstr, and rootResource 
PDSOCKET dataManager::sock_desc;  
int dataManager::sock_port;  
int dataManager::termWin_port = -1;
PDSOCKET dataManager::termWin_sock= INVALID_PDSOCKET;
dataManager *dataManager::dm = NULL;  

dictionary_hash<pdstring,metric*>  metric::allMetrics(pdstring::hash);
dictionary_hash<metricInstanceHandle,metricInstance *> 
		metricInstance::allMetricInstances(metricInstance::mhash);
dictionary_hash<perfStreamHandle,performanceStream*>  
		performanceStream::allStreams(performanceStream::pshash);
dictionary_hash<pdstring, resource*> resource::allResources(pdstring::hash, 8192);
dictionary_hash<pdstring,resourceList *> resourceList::allFoci(pdstring::hash);

dictionary_hash<unsigned, resource*>resource::resources(uiHash);
pdvector<pdstring> resource::lib_constraints;
pdvector<unsigned> resource::lib_constraint_flags;
pdvector< pdvector<pdstring> > resource::func_constraints;
pdvector<unsigned> resource::func_constraint_flags;
bool resource::func_constraints_built = false;
bool resource::lib_constraints_built = false;

pdvector<metric*> metric::metrics;
pdvector<paradynDaemon*> paradynDaemon::allDaemons;
pdvector<daemonEntry*> paradynDaemon::allEntries;
pdvector<executable*> paradynDaemon::programs;
unsigned paradynDaemon::procRunning;
pdvector<resourceList *> resourceList::foci;
pdvector<phaseInfo *> phaseInfo::dm_phases;
u_int metricInstance::next_id = 1;
// u_int performanceStream::next_id = 0;
u_int paradynDaemon::countSync = 0;

// to distinguish the enableDataRequest calls only for samples 
// from those for both samples and traces 
// 0 is reserved for non-trace use
u_int performanceStream::next_id = 1;

resource *resource::rootResource = new resource();
timeLength metricInstance::curr_bucket_width = Histogram::getMinBucketWidth();
timeLength metricInstance::global_bucket_width =Histogram::getMinBucketWidth();
phaseHandle metricInstance::curr_phase_id;
u_int metricInstance::num_curr_hists = 0;
u_int metricInstance::num_global_hists = 0;

timeStamp paradynDaemon::earliestStartTime;
void newSampleRate(timeLength rate);

extern void histDataCallBack(pdSample *buckets, relTimeStamp, int count, 
			     int first, void *callbackData);
extern void histFoldCallBack(const timeLength *_width, void *callbackData);

#if !defined(os_windows)
void StartTermWin( bool useGUI );
void mpichUnlinkWrappers( void );
#endif // !defined(os_windows)

//upcall from paradynd to notify the datamanager that the static
//portion of the call graph is completely filled in.
void dynRPCUser::CallGraphFillDone(MRN::Stream *, pdstring exe_name){
  CallGraph *cg;
  cg = CallGraph::FindCallGraph(exe_name);
  cg->CallGraphFillDone();

  extern unsigned num_dmns_to_report_callgraph;
  num_dmns_to_report_callgraph--;
  if( num_dmns_to_report_callgraph == 0 ){
    paradynDaemon *pd;
    pd = paradynDaemon::allDaemons[0];
    MRN::Stream * stream  = pd->getNetwork()->new_Stream(pd->getNetwork()->get_BroadcastCommunicator());
    staticCallgraphReportsComplete(stream);
   delete stream;
  }


}

void dynRPCUser::CallGraphAddDynamicCallSiteCallback(MRN::Stream * , pdstring exe_name, pdstring parent){
  CallGraph *cg;
  cg = CallGraph::FindCallGraph(exe_name);
  resource *r = resource::string_to_resource(parent);
  assert(r != NULL);
  cg->AddDynamicCallSite(r);
}


void dynRPCUser::CallGraphAddProgramCallback(MRN::Stream *, pdstring exe_name){
  CallGraph::AddProgram(exe_name);
}

// upcall from paradynd to register the entry function for a given
//  call graph.... 
//This function is called with a previously unseen program ID to create a new
// call graph.
void dynRPCUser::CallGraphSetEntryFuncCallback(MRN::Stream *, pdstring exe_name, 
                                               pdstring entry_func, int tid) {
    CallGraph *cg;

		//fprintf(stdout,"in dynRPCUser::CallGraphSetEntryFuncCallback DMmain.C exe_name = %s  entry_func = %s\n",exe_name.c_str(),entry_func.c_str());
    // get/create call graph corresponding to program....
    cg = CallGraph::FindCallGraph(exe_name);
    assert(cg);

    // resource whose name is passed in <resource> should have been previously
    //  registered w/ data manager....
    resource* r = resource::string_to_resource(entry_func);
    assert(r != NULL);

    cg->SetEntryFunc(r, tid);
}

// upcall from paradynd to register new function resource with call graph....
// parameters are an integer corresponding to the program to which a node
// is being added, and a pdstring that is the name of the function being added

void dynRPCUser::AddCallGraphNodeCallback(MRN::Stream *, pdstring exe_name,
                                          pdstring r_name) {
    CallGraph *cg;

    // get (or create) call graph corresponding to program....
    cg = CallGraph::FindCallGraph(exe_name);
		if (!cg)
			{
				fprintf(stderr, "[%s:%u] - Fatal Error.  Tried to add resource %s to "
								"non-existant exec %s\n", __FILE__, __LINE__, r_name.c_str(),
								exe_name.c_str());
				assert(cg);
			}
		
		// resource whose name is passed in <resource> should have been previously
		//  registered w/ data manager....
		resource* r = resource::string_to_resource(r_name);
		assert( r != NULL );
    cg->AddResource(r);
}

//Same as AddCallGraphNodeCallback, only adds multiple children at once,
//and associates these children with a give parent (r_name)
void dynRPCUser::AddCallGraphStaticChildrenCallback(MRN::Stream *, pdstring exe_name, 
                                                    pdstring r_name, 
                                                    pdvector<pdstring>children)
{
    unsigned u;
    CallGraph *cg;
    resource *r, *child;
    pdvector <resource *> children_as_resources;

    // call graph for program <program> should have been previously defined....
    cg = CallGraph::FindCallGraph(exe_name);
    assert(cg);
    // resource whose name is passed in <resource> should have been previously
    //  registered w/ data manager....
    r = resource::string_to_resource(r_name);
    assert(r);

    // convert pdvector of resource names to pdvector of resource *'s....
    for(u=0;u<children.size();u++) {
      child = resource::string_to_resource(children[u]);
      assert(child);
      children_as_resources += child;
    }

    cg->SetChildren(r, children_as_resources);
}

void dynRPCUser::AddCallGraphDynamicChildCallback(MRN::Stream *, pdstring exe_name,
                                                  pdstring parent,
                                                  pdstring child) {
  resource *p, *c;
  CallGraph *cg;
  cg = CallGraph::FindCallGraph(exe_name);
  assert(cg);
  
  p = resource::string_to_resource(parent);
  assert(p);
  c = resource::string_to_resource(child);
  assert(c);
  perfConsult->notifyDynamicChild(p->getHandle(),c->getHandle());
  cg->AddDynamicallyDeterminedChild(p,c);
}





/*should be removed for output redirection
left untouched for paradynd log mesg use
*/
//
// IO from application processes.
//
void dynRPCUser::applicationIO(MRN::Stream *, int,int, pdstring data)
{

    // NOTE: this fixes a purify error with the commented out code (a memory
    // segment error occurs occasionally with the line "cout << rest << endl") 
    // this is problably not the best fix,  but I can't figure out why 
    // the error is occuring (rest is always '\0' terminated when this
    // error occurs)---tn 
	if( data.length() > 0 )
    {
			//fprintf(stdout,data.c_str());
			fflush(stdout);
		}
	else
		{
			fprintf( stderr, "paradyn: warning: empty IO string sent from daemon...ignoring..." );
		}

#ifdef n_def
    char *ptr;
    char *rest;
    // extra should really be per process.
    static pdstring extra;

    rest = P_strdup(data.c_str());

    char *tp = rest;
    ptr = P_strchr(rest, '\n');
    while (ptr) {
	*ptr = '\0';
	if (pid) {
	    fprintf(stderr, "pid %d:", pid);
	} else {
	    fprintf(stderr, "paradynd: ");
	}
	if (extra.length()) {
	    cout << extra;
	    extra = (char*) NULL;
	}
	cout << rest << endl;
	rest = ptr+1;
	if(rest)
	    ptr = P_strchr(rest, '\n');
        else
	    ptr = 0;
    }
    extra += rest;
    delete tp;
    rest = 0;
#endif
}

extern status_line *DMstatus;

void dynRPCUser::resourceBatchMode(MRN::Stream*, bool) // bool onNow
{
   fprintf(stderr, "error calling virtual func: dynRPCUser::resourceBatchMode\n");
}

//
// upcalls from remote process.
//
void dynRPCUser::resourceInfoCallback(MRN::Stream*, u_int , pdvector<pdstring> ,
                                      pdstring , u_int, u_int)
{

printf("error calling virtual func: dynRPCUser::resourceInfoCallback\n");

}
void dynRPCUser::resourceUpdateCallback(MRN::Stream *,
                                        pdvector<pdstring>,
                                        pdvector<pdstring>,
                                        pdstring )
{
   fprintf(stderr,
           "error calling virtual func: dynRPCUser::resourceUpdateCallback\n");
}

//
// upcalls from remote process.
//
void dynRPCUser::retiredResource(MRN::Stream*, pdstring) {
   fprintf(stderr, "error calling virtual func: dynRPCUser::retiredResource\n");
}
void dynRPCUser::resourceEquivClassReportCallback(MRN::Stream*, pdvector<T_dyninstRPC::equiv_class_entry> )
{
  printf("error calling virtual func: dynRPCUser::resourceEquivClassReportCallback\n");
}

void dynRPCUser::callGraphEquivClassReportCallback(MRN::Stream*, pdvector<T_dyninstRPC::equiv_class_entry> )
{
  printf("error calling virtual func: dynRPCUser::resourceEquivClassReportCallback\n");
}

void dynRPCUser::severalResourceInfoCallback(MRN::Stream*, pdvector<T_dyninstRPC::resourceInfoCallbackStruct>) {
printf("error calling virtual func: dynRPCUser::severalResourceInfoCallback\n");
}

void dynRPCUser::resourceReportsDone( MRN::Stream *, int )
{
    printf("error calling virtual func: dynRPCUser::severalResourceInfoCallback\n");
}

//
// handle an enable response from a daemon. If all daemons have responded
// then make response callback to calling thread, and check the outstanding
// enables list to see if this enable response satisfies any waiting requests.
// and enable for an MI is successful if its done entry is true and if its
// MI* is not 0
//
void dynRPCUser::enableDataCallback(MRN::Stream*, T_dyninstRPC::instResponse resp) {

    if(resp.rinfo.size() == 0)
      return;

    metricFocusReqBundle *matching_bundle = 
       metricFocusReqBundle::findActiveBundle(resp.request_id);


    if(matching_bundle == NULL) {
       // a request entry can be removed if a new phase event occurs
       // between the enable request and response, so ignore the response
       return;
    }
    matching_bundle->updateWithEnableCallback(resp);
}

//
// Upcall from daemon in response to getPredictedDataCost call
// id - perfStreamHandle assoc. with the call
// req_id - an identifier assoc. with the request 
// val - the cost of enabling the metric/focus pair
//
void dynRPCUser::getPredictedDataCostCallback(MRN::Stream*, u_int id,
					      u_int req_id,
					      float val,
					      u_int clientID)
{
    // find the assoc. perfStream and update it's pred data cost value
    performanceStream::psIter_t allS = performanceStream::getAllStreamsIter();
    perfStreamHandle h; performanceStream *ps;
    while(allS.next(h,ps)){
	if(h == (perfStreamHandle)id){
            ps->predictedDataCostCallback(req_id,val,clientID);
	    return;
    } }
    // TODO: call correct routine
    assert(0);
}

//
// Display errors using showError function from the UIM class
// This function allows to display error messages from paradynd
// using the "upcall" or "call back" mechanism.
// Parameters: 	errCode = Error code
//		errString = Error message
//		hostName = Host name where the error occur
// Call: there is a macro defined in "showerror.h". This macro must be
//       used when calling this function. A typical call is:
//       showErrorCallback(99, "Erro message test"). This macro will
//       automatically insert the additional host info required.
//
void dynRPCUser::showErrorCallback(MRN::Stream*, int errCode, 
				   pdstring errString,
				   pdstring hostName)
{
    pdstring msg;

    if (errString.length() > 0) {
       if (hostName.length() > 0) {
    	    msg = pdstring("<Msg from daemon on host ") + hostName + 
	          pdstring("> ") + errString;
       }
       else { 
          msg = pdstring("<Msg from daemon on host ?> ") + errString; 
       }
       uiMgr->showError(errCode, P_strdup(msg.c_str()));
    }
    else {
       uiMgr->showError(errCode, ""); 
    }

    //
    // hostName.length() should always be > 0, otherwise
    // hostName is not defined (i.e. "?" will be used instead).
    // if errString.length()==0, (i.e. errString.c_str()==""),
    // then we will use the default error message in errorList.tcl
    // This message, however, will not include any info about the current
    // host name.
    //
}

//
// Paradynd calls this igen fn when it starts a new process (more
// specifically, after it starts the new process and the new process
// has completed running DYNINSTinit).
//
void dynRPCUser::newProgramCallbackFunc(MRN::Stream*, int pid,
                                        pdvector<pdstring> argvString,
                                        pdstring machine_name,
                                        bool calledFromExec,
                                        bool runMe)
{
    static bool first_time=true;
    static unsigned int num_process_left;

    //TODO: major hack. we will wait for this many procs to report before
    //      we enable pause/run buttons
    if( first_time ) 
			{
        first_time = false;
				
        num_process_left = paradynDaemon::allDaemons.size();
			}
		
    // there better be a paradynd running on this machine!
    paradynDaemon *last_match = 0;
		
    for (unsigned i = 0; i < paradynDaemon::allDaemons.size(); i++)
			{
				paradynDaemon *pd = paradynDaemon::allDaemons[i];
				
        //fprintf(stderr, "comparing local daemon name \"%s\" with recvd daemon name \"%s\"\n",
				//getNetworkName(pd->machine).c_str(), getNetworkName(machine_name).c_str() );
				
        if ( pd->machine.length()  && (getNetworkName(pd->machine) == getNetworkName(machine_name) )) {
					last_match = pd;
        }
			}
    if (last_match != 0)
			{
				
				//cout <<"newProgramCallbackFunc last_match->get_id = "<<last_match->get_id()<<endl;
				//cout <<"newProgramCallbackFunc pid = "<<pid<<endl;
				//cout <<"newProgramCallbackFunc argvString = "<<argvString<<endl;
				//cout <<"newProgramCallbackFunc last_match= "<<last_match<<endl;
				// cout <<"newProgramCallbackFunc last_match= "<<last_match<<endl;
				// cout <<"newProgramCallbackFunc calledFromExec = "<<calledFromExec<<endl;
				//cout <<"newProgramCallbackFunc runMe = "<<runMe<<endl;
        
        if (!paradynDaemon::addRunningProgram(pid, argvString, last_match,
                                              calledFromExec, runMe)) 
					{
            assert(false);
					}
        num_process_left--;
        if ( num_process_left == 0 )
					{
						uiMgr->enablePauseOrRun();
					}
			}
    else
			{
        // for now, abort if there is no paradynd, this should not happen
        fprintf(stderr, "process started on %s, can't find paradynd "
                "there\n", machine_name.c_str());
        fprintf(stderr,"paradyn error #1 encountered\n");
        //exit(-1);
			}
}

void dynRPCUser::newMetricCallback(MRN::Stream*, T_dyninstRPC::metricInfo info)
{
    addMetric(info);
}

void dynRPCUser::setDaemonStartTime(MRN::Stream*, int, double) 
{
  assert(0 && "Invalid virtual function");
}

void dynRPCUser::setInitialActualValueFE(MRN::Stream*, int, double) 
{
  assert(0 && "Invalid virtual function");
}

void dynRPCUser::cpDataCallbackFunc(MRN::Stream*, int,double,int,double,double)
{
    assert(0 && "Invalid virtual function");
}

// batch the sample delivery
void dynRPCUser::batchSampleDataCallbackFunc(MRN::Stream*, int,
		    pdvector<T_dyninstRPC::batch_buffer_entry>)
{
    assert(0 && "Invalid virtual function");
}

// batch the trace delivery
void dynRPCUser::batchTraceDataCallbackFunc(MRN::Stream*, int,
                    pdvector<T_dyninstRPC::trace_batch_buffer_entry>)
{
    assert(0 && "Invalid virtual function");
}

//
// When a paradynd is started remotely, ie not by paradyn, this upcall
// reports the information for that paradynd to paradyn
//
void 
dynRPCUser::reportSelf(MRN::Stream*, pdstring , pdstring , int , pdstring)
{
  assert(0);
  return;
}

void 
dynRPCUser::reportStatus(MRN::Stream*, pdstring)
{
    assert(0 && "Invalid virtual function");
}

void
dynRPCUser::processStatus(MRN::Stream*, int, u_int)
{
    assert(0 && "Invalid virtual function");
}

void
dynRPCUser::endOfDataCollection(MRN::Stream*, int)
{
  assert(0 && "Invalid virtual function");
}

void dataManager::displayParadynGeneralInfo()
{
  uiMgr->showError(104, "\0");
}

void dataManager::displayParadynLicenseInfo()
{
  uiMgr->showError(105, "\0");
}

void dataManager::displayParadynReleaseInfo()
{
  uiMgr->showError(106, "\0");
}

void dataManager::displayParadynVersionInfo()
{
  pdstring msg = pdstring("Paradyn Version Identifier:\n")
                 + pdstring(V_paradyn) + pdstring("\n")
                 + pdstring("\n");
  static char buf[1000];
  sprintf(buf, "%s", msg.c_str());
  uiMgr->showError(107, buf);
}

// displayDaemonStartInfo() presents the information necessary to manually
// start up a Paradyn daemon and have it connect to this Paradyn front-end.

void dataManager::displayDaemonStartInfo() 
{
    const pdstring machine = getNetworkName();
    const pdstring port    = pdstring(dataManager::dm->sock_port);
    pdstring command = pdstring("paradynd -z<flavor> -l1")
                       + pdstring(" -m") + machine + pdstring(" -p") + port;
#if !defined(i386_unknown_nt4_0)
    pdstring term_port = pdstring(dataManager::termWin_port);
    command += pdstring(" -P");
    command += term_port;
#endif
    static char buf[1000];

    pdstring msg = pdstring("To start a paradyn daemon on a remote machine,")
      + pdstring(" login to that machine and run paradynd")
      + pdstring(" with the following arguments:\n\n    ") + command
      + pdstring("\n\n(where flavor is one of: unix, mpi, winnt).\n");
    
    sprintf(buf, "%s", msg.c_str());
    uiMgr->showError(99, buf);
    //fprintf(stderr, msg.c_str());
}

// printDaemonStartInfo() provides the information necessary to manually
// start up a Paradyn daemon and have it connect to this Paradyn front-end,
// writing this information to the file whose name is provided as argument
// (after doing some sanity checks to ensure that the file can be written
// and that it (probably) won't overwrite another file mistakenly).

void dataManager::printDaemonStartInfo(const char *filename)
{
    const pdstring machine = getNetworkName();
    const pdstring port  = pdstring(dataManager::dm->sock_port);
    pdstring command = pdstring("paradynd -z<flavor> -l1")
                     + pdstring(" -m") + machine + pdstring(" -p") + port;
#if !defined(i386_unknown_nt4_0)
    pdstring term_port = pdstring(dataManager::termWin_port);
    command += pdstring(" -P");
    command += term_port;
#endif
    static char buf[1000];

    assert (filename && (filename[0]!='\0'));
    //cerr << "dataManager::printDaemonStartInfo(" << filename << ")" << endl;

    struct stat statBuf;
    int rd = stat(filename, &statBuf);
    if (rd == 0) {                                      // file already exists
        if (S_ISDIR(statBuf.st_mode)) { // got a directory!
            pdstring msg = pdstring("Paradyn connect file \"") +
                           pdstring(filename) +
                           pdstring("\" is a directory! - skipped.\n");
            sprintf(buf, "%s", msg.c_str());
            uiMgr->showError(103, buf);
            return;             
        } else if (S_ISREG(statBuf.st_mode) && (statBuf.st_size > 0)) {
            FILE *fp = fopen(filename, "r");        // check whether file exists
            if (fp) {
                int n=fscanf(fp, "paradynd");       // look for daemon info
                if (n<0) {
                    pdstring msg = pdstring("Aborted overwrite of unrecognized \"") 
                      + pdstring(filename)
                      + pdstring("\" contents with daemon start-up information.");
                    sprintf(buf, "%s", msg.c_str());
                    uiMgr->showError(103, buf);
                    fclose(fp);
                    return;
                } else {
                    //fprintf(stderr,"Overwriting daemon start-up information!\n");
                }
                fclose(fp);
            }
        }
    }
    FILE *fp = fopen(filename, "w");
    if (fp) {
        // go ahead and actually (re-)write the connect file
        fprintf(fp, "%s\n", command.c_str());;
        fclose(fp);
    } else {
        pdstring msg = pdstring("Unable to open file \"") +
                       pdstring(filename) +
                       pdstring("\" to write daemon start information.");
        sprintf(buf, "%s", msg.c_str());
        uiMgr->showError(103, buf);
    }
}

bool dataManager::DM_sequential_init(const char* met_file){
   pdstring mfile = met_file;
   return(metMain(mfile)); 
}

int
dataManager::DM_post_thread_create_init( DMthreadArgs* dmArgs )
{
    thr_name("Data Manager");
    dataManager::dm = new dataManager( dmArgs->mainTid );

    // supports argv passed to paradynDaemon
    // new paradynd's may try to connect to well known port
    // DMsetupSocket(dataManager::dm->sock_desc);

#if !defined(os_windows)
    StartTermWin( dmArgs->useTermWinGUI );
#endif // (os_windows)

    bool aflag;
#if !defined(i386_unknown_nt4_0)
    aflag=(RPC_make_arg_list(paradynDaemon::args,
  	 	             dataManager::dm->sock_port,dataManager::termWin_port, 1, 1, "", false));
#else
    aflag=(RPC_make_arg_list(paradynDaemon::args,
  	 	             dataManager::dm->sock_port, 1, 1, "", false));
#endif
    assert(aflag);
     // start initial phase
    pdstring dm_phase0 = "phase_0";
    phaseInfo::startPhase(dm_phase0, false, false);

    char DMbuff[64];
    unsigned int msgSize = 64;

    msg_send( dmArgs->mainTid, MSG_TAG_DM_READY, (char *) NULL, 0);
    tag_t tag = MSG_TAG_ALL_CHILDREN_READY;
		thread_t from = dmArgs->mainTid;

    msg_recv (&from, &tag, DMbuff, &msgSize);
		assert( from == dmArgs->mainTid );
    return 1;
}

#if defined (JUNK)
static bool poll_callback(PDSOCKET sock)
{
    //if socket belongs to mrnet, check mrnet buffers for data
    for(unsigned i=0; i<paradynDaemon::allDaemons.size(); i++)
  {
    paradynDaemon *pd = paradynDaemon::allDaemons[i]; 
    if (pd->get_sock() == sock)
      return xdrrec_eof(pd->net_obj());
  }
  return false;
}
#endif /* JUNK */

// returns -1 on error, number of requests processed otherwise
static int check_MRNetForData()
{
    int num_requests=0;
    bool processed_request;
    int ret=0;

    if( paradynDaemon::allDaemons.size() != 0 ) {
        paradynDaemon * pd = paradynDaemon::allDaemons[0];

        //checking mrnet, waitLoop doesn't block if no requests are there.
        do{
            processed_request=false;
            //TODO: handle multiple networks

            assert( pd->getNetwork() );

            ret = pd->waitLoop( pd->getNetwork(), &processed_request );
            if( processed_request ){
                num_requests++;
            }
	  
            //TODO: handle errors
        } while( ret != T_dyninstRPC::error && processed_request );
    }
    
    if ( ret == T_dyninstRPC::error ){
        return -1;
    }

    return num_requests;
}

// returns -1 on error, number of requests processed otherwise
static int check_AsyncBuffersForData()
{
    int num_requests=0;
    int ret=0;

    if( paradynDaemon::allDaemons.size() != 0 ){
        paradynDaemon * pd = paradynDaemon::allDaemons[0];

        // handle async requests buffered while blocking on a sync request
        while (pd->buffered_requests()){
            ret = pd->process_buffered();

            if( ret == T_dyninstRPC::error) {
                cout << "error on paradyn daemon\n";
                paradynDaemon::removeDaemon(pd, true);
                break;
            }
            else{
                num_requests++;
            }
        }
    }

    if ( ret == T_dyninstRPC::error ){
        return -1;
    }

    return num_requests;
}
//
// Main loop for the dataManager thread.
//
void* 
DMmain( void* varg )
{
    DMthreadArgs* dmArgs = (DMthreadArgs*)varg;
    assert( dmArgs != NULL );


    unsigned fd_first = 0;
    // We declare the "printChangeCollection" tunable constant here; it will
    // last for the lifetime of this function, which is pretty much forever.
    // (used to be declared as global in DMappContext.C.  Globally declared
    //  tunables are now a no-no).  Note that the variable name (printCC) is
    // unimportant.   -AT
    tunableBooleanConstantDeclarator printCC("printChangeCollection", 
	      "Print the name of metric/focus when enabled or disabled",
	      false, // initial value
	      NULL, // callback
	      developerConstant);

    // Now the same for "printSampleArrival"
    extern bool our_print_sample_arrival;
    our_print_sample_arrival = false;
    extern void printSampleArrivalCallback(bool);
    tunableBooleanConstantDeclarator printSA("printSampleArrival", 
              "Print out status lines to show the arrival of samples",
	      our_print_sample_arrival, // init val
	      printSampleArrivalCallback,
	      developerConstant);

    // Now the same for "PersistentData"
    tunableBooleanConstantDeclarator persData("persistentData",
	      "Don't delete internal paradyn data when instrumentation disabled",
	      false, // init val
	      NULL,
	      userConstant);

    dataManager::DM_post_thread_create_init( dmArgs );

    thread_t tid;
    unsigned int tag;
    int err;

    //check mrnet and async bufs for any new/backlogged requests
    int ret = check_MRNetForData();
    if (ret == -1 ){
        //TODO: handle error
    }
       
    ret = check_AsyncBuffersForData();
    if (ret == -1 ){
        //TODO: handle error
    }

    while (1) {
        // wait for next message from anyone, blocking till available
        tid = THR_TID_UNSPEC;
        tag = MSG_TAG_ANY;
        //msg_dump_state();
        err = msg_poll_preference(&tid, &tag, true,fd_first);
        assert(err != THR_ERR);
        fd_first = !fd_first;
	
        if (tag == MSG_TAG_DO_EXIT_CLEANLY) {
            // we're done handling events
            break;
        }
        
        if (tag == MSG_TAG_SOCKET) {
            // must be something on an mrnet network
            PDSOCKET fromSock = thr_socket( tid );
            assert(fromSock != INVALID_PDSOCKET);
            
            //if there are any daemons, check mrnet and async bufs for requests
            ret = check_MRNetForData();
            clear_ready_sock(fromSock);
            if (ret == -1 ) {
                //TODO: handle error
            }

            //check async bufs for requests
            ret = check_AsyncBuffersForData();
            if (ret == -1 ) {
                //TODO: handle error
            }
        } else  if (dataManager::dm->isValidTag((T_dataManager::message_tags)tag)) {
            
            if (dataManager::dm->waitLoop(true,
                                          (T_dataManager::message_tags)tag) 
                == T_dataManager::error) {
                // handle error
                assert(0);
            }
            
            //We must process mrnet and async buffers here for the case where
            //this thread request resulted in a synchronous mrnet request
            //during which the buffers could have been filled with async events
            //if there are any daemons, check mrnet and async bufs for requests
            ret = check_MRNetForData();
            if (ret == -1 ) {
                //TODO: handle error
            }
		
            //check async bufs for requests
            ret = check_AsyncBuffersForData();
            if (ret == -1 ) {
                //TODO: handle error
            }
        } 
	    else {
            cerr << "Unrecognized message in DMmain.C: tag = "
                 << tag << ", tid = "
                 << tid << '\n';
            assert(0);
        }
    }

    //
    // cleanup
    //
#if !defined(i386_unknown_nt4_0)
    mpichUnlinkWrappers();

    if( twUser != NULL ) {
        twUser->shutdown();
    }

#endif // !defined(i386_unknown_nt4_0)

    return NULL;
}


void addMetric(T_dyninstRPC::metricInfo &info)
{

    // if metric already exists return
    if(metric::allMetrics.defines(info.name)){
        return;
    }
    metric *met = new metric(info);

    // now tell all perfStreams
    performanceStream::psIter_t allS = performanceStream::getAllStreamsIter();
    perfStreamHandle h;
    performanceStream *ps;
    while(allS.next(h,ps)){
       controlCallback controlData = ps->getControlCallbackData();
	if(controlData.mFunc) 
	  {
	    // set the correct destination thread.
	    dataManager::dm->setTid(ps->getThreadID());
	    dataManager::dm->newMetricDefined(controlData.mFunc, 
					      ps->Handle(),
					      met->getName(),
					      met->getStyle(),
					      met->getAggregate(),
					      met->getUnits(),
					      met->getHandle(),
					      met->getUnitsType());
	  }
    }
}

void ps_retiredResource(pdstring resource_name) {
   resource *res = resource::string_to_resource(resource_name);
   if(! res) {
      cerr << "Couldn't find resource " << resource_name << "\n";
      return;
   }
   res->markAsRetired();

   /* inform others about it if they need to know */
   performanceStream::psIter_t allS = performanceStream::getAllStreamsIter();
   perfStreamHandle h;
   performanceStream *ps;
   resourceHandle r_handle = res->getHandle();

   while(allS.next(h,ps)) {
       ps->callResourceRetireFunc(r_handle, res->getFullName().c_str());
   }
}

void newSampleRate(timeLength rate)
{
    paradynDaemon * pd = paradynDaemon::allDaemons[0];

    MRN::Communicator * comm = pd->network->new_Communicator();

    for(unsigned i = 0; i < paradynDaemon::allDaemons.size(); i++){
        comm->add_EndPoint( paradynDaemon::allDaemons[i]->endpoint );
    }

    MRN::Stream * local_stream = pd->network->new_Stream( comm );
	pd->setSampleRate(local_stream, rate.getD(timeUnit::sec()));

    delete local_stream;
    delete comm;
}


#if !defined(os_windows)
void
StartTermWin( bool useTermWinGUI )
{
    bool sawStartupError = false;

    assert( dataManager::termWin_sock == INVALID_PDSOCKET );
    assert( twUser == NULL );

    dataManager::termWin_port = RPC_setup_socket(dataManager::termWin_sock,
                                                    AF_INET, 
                                                    SOCK_STREAM);
    char buffer[256];
    sprintf(buffer,"%d",dataManager::termWin_sock);
    pdvector<pdstring>* av = new pdvector<pdstring>;
    av->push_back( buffer );

    // if desired, ask GUI to use command-line interface
    if( !useTermWinGUI )
    {
        av->push_back( "-cl" );
    }

    PDSOCKET tw_sock = RPCprocessCreate("localhost","","termWin","",*av);
    if( tw_sock != PDSOCKET_ERROR )
    {

        // bind the termWin connection so that we're given notice of
        // available data on the termWin connection (like its response
        // to our initial version number handshake)
        thread_t tw_sock_tid;
        msg_bind_socket( tw_sock,   // socket
                         true,   // we will read data off connection
                         NULL,   // no special will_block function
                         NULL,
                         &tw_sock_tid ); // tid assigned to bound socket

        twUser = new termWinUser( tw_sock, NULL, NULL, 0 );

        if( twUser->errorConditionFound )
        {
            // the termWin igen interface handshake failed
            sawStartupError = true;
        }
    }
    else
    {
        // the creation of the termWin process failed
        sawStartupError = true; 
    }

    if( sawStartupError )
    {
        // report the error to the user
        uiMgr->showError( 121, "Paradyn failed to start the terminal window." );
    
        // ensure that we know we don't have a termWin connection
        delete twUser;
        twUser = NULL;
    }

    delete av;
    P_close(dataManager::termWin_sock);
    dataManager::termWin_sock = INVALID_PDSOCKET;
}
#endif // !defined(i386_unknown_nt4_0)

