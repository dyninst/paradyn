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

// $Id: pd_process.h,v

// Put Paradyn specific process code in the object, not in
// dyninstAPI/src/process.[hC]

#ifndef __PD_PROCESS__
#define __PD_PROCESS__

#include "common/h/Vector.h"
#include "dyninstAPI/h/BPatch_Vector.h"
#include "dyninstAPI/h/BPatch_process.h"
#include "dyninstAPI/h/BPatch_snippet.h"

#include "paradynd/src/threadMgr.h"
#include "paradynd/src/timeMgr.h"
#include "paradynd/src/shmMgr.h"
#include "paradynd/src/sharedMetaData.h"
#include "paradynd/src/variableMgr.h"
#include "paradynd/src/resource.h"
#include "paradynd/src/instReqNode.h"
#include "rtinst/h/trace.h" // for PARADYN_bootstrapStruct

#define FUNC_ENTRY      0x1             /* entry to the function */
#define FUNC_EXIT       0x2             /* exit from function */
#define FUNC_CALL       0x4             /* subroutines called from func */
#define FUNC_ARG        0x8             /* use arg as argument */

typedef enum  {
  libUnloaded, 
  libLoading, 
  libInitializing, 
  libLoaded, 
  libReady 
} libraryState_t;

bool reachedLibState(libraryState_t lib, libraryState_t state);
void setLibState(libraryState_t &lib, libraryState_t state);


static unsigned  MAX_NUMBER_OF_THREADS =  32;
class pdinstMapping {
public:
  // other parameters:  allow_trap, useTrampGuard (might have to add)
  //  PDSEP
  pdinstMapping(const pdstring f, const pdstring i, const int w,
               BPatch_callWhen wh, BPatch_snippetOrder ord, 
               BPatch_snippet *s = NULL, bool warn_on_error = false) :
    func(f),
    inst(i),
    where(w),
    when(wh),
    order(ord),
    mt_only(false),
    quiet_fail(!warn_on_error) 
  {  if (s) args.push_back(s); }

  pdinstMapping(const pdstring f, const pdstring i, const int w,
               BPatch_snippet *s = NULL, bool warn_on_error = false) :
    func(f),
    inst(i),
    where(w),
    when(BPatch_callBefore),
    order(BPatch_lastSnippet),
    mt_only(false),
    quiet_fail(!warn_on_error) 
  {  if (s) args.push_back(s); }

  pdinstMapping(const pdstring f, const pdstring i, const int w,
               pdvector<BPatch_snippet *> &snips, bool warn_on_error = false) :
    func(f),
    inst(i),
    where(w),
    when(BPatch_callBefore),
    order(BPatch_lastSnippet),
    mt_only(false),
    quiet_fail(!warn_on_error) 
  {  
     for (unsigned int j = 0; j < snips.size(); ++j) 
       args.push_back(snips[j]);
  }

  pdinstMapping(const pdstring f, const pdstring i, const int w,
               BPatch_callWhen wh, BPatch_snippetOrder ord,
               pdvector<BPatch_snippet *> &snips, bool warn_on_error = false) :
    func(f),
    inst(i),
    where(w),
    when(wh),
    order(ord),
    mt_only(false),
    quiet_fail(!warn_on_error)
  {
     for (unsigned int j = 0; j < snips.size(); ++j)
       args.push_back(snips[j]);
  }

  ~pdinstMapping() {} // might want to delete all snippets here?

  bool is_MTonly() { return mt_only; }
  void markAs_MTonly() { mt_only = true; }

//private:
  pdstring func;
  pdstring inst;
  int where;
  BPatch_callWhen when;
  BPatch_snippetOrder order;
  bool mt_only;
  bool quiet_fail;
  BPatch_Vector<BPatch_snippet *> args;
  pdvector<BPatchSnippetHandle *> snippetHandles;  
  // what about useTrampGuard, and allow_trap?
  //  (looks like we can ignore ? )
};

class pd_image;

class pd_process {
   BPatch_process *dyninst_process;

   threadMgr thr_mgr;

   variableMgr *theVariableMgr;

   pd_image *img;
   resource *rid;
   bool created_via_attach;
   BPatch_function *monitorFunc; // func in RT lib used for monitoring call sites

   unsigned long os_handle; //pid on unixes, handle on windows
 public:
   // Paradyn daemon arguments, etc.
   static pdvector<pdstring> arg_list; // the arguments of paradynd
   // Paradyn RT name if set on the command line
   static pdstring defaultParadynRTname;

   // Type of daemon
   static pdstring pdFlavor;

   // Program name we were started with
   static pdstring programName;

   unsigned numOfActCounters_is;
   unsigned numOfActProcTimers_is;
   unsigned numOfActWallTimers_is;
   
 public:
   // Creation constructor
   pd_process(const pdstring argv0, pdvector<pdstring> &argv,
              const pdstring dir, int stdin_fd, int stdout_fd, int stderr_fd);

   // Attach constructor
   pd_process(const pdstring &progpath, int pid);
  
   // fork constructor
   pd_process(const pd_process &parent, BPatch_process *childDynProc);
   
   ~pd_process();
   
   void init();
   
   threadMgr &thrMgr() { return thr_mgr; }
   void addThread(pd_thread *thr) { thr_mgr.addThread(thr); }
   void removeThread(pd_thread *thr) { thr_mgr.removeThread(thr); }
   void removeThread(int tid) { thr_mgr.removeThread(tid); }
   pd_thread *findThread(int tid) { return thr_mgr.find_pd_thread(tid); }
   threadMgr::thrIter beginThr() { return thr_mgr.begin(); }
   threadMgr::thrIter endThrMark() { return thr_mgr.end(); }
   unsigned numThr() const { return thr_mgr.size(); }

   void findThreads();
	 void reportInitialThreads();
	 void reportOneThread(pd_thread *thr);
   bool doMajorShmSample();
   bool doMinorShmSample();

   const variableMgr &getVariableMgr() const {
      return *theVariableMgr;
   }
   variableMgr &getVariableMgr() {
      return *theVariableMgr;
   }

   // handle to resource for this process
   resource *get_rid() { return rid; }

   void handleExit(int exitStatus);

   void FillInCallGraphStatic( bool init_graph = false, unsigned *checksum = 0 );

   void MonitorDynamicCallSites(pdstring function_name);

  // ==== Cpu time related functions and members =======================

  // called by process object constructor
  void initCpuTimeMgr();
  // called by initCpuTimeMgr, sets up platform specific aspects of cpuTimeMgr
  void initCpuTimeMgrPlt();

  // Call getCpuTime to get the current cpu time of process. Time conversion
  // from raw to primitive time units is done in relevant functions by using
  // the units ratio as defined in the cpuTimeMgr.  getCpuTime and getRawTime
  // use the best level as determined by the cpuTimeMgr.
  enum { time_for_whole_program = -1 };
  timeStamp getCpuTime(int lwp=time_for_whole_program);
  timeStamp units2timeStamp(int64_t rawunits);
  timeLength units2timeLength(int64_t rawunits);
  rawTime64 getRawCpuTime(int lwp=time_for_whole_program);
  rawTime64 getAllLwpRawCpuTime_hw();
  rawTime64 getAllLwpRawCpuTime_sw();

  // Verifies that the wall and cpu timer levels chosen by the daemon are
  // also available within the rtinst library.  This is an issue because the
  // daemon chooses the wall and cpu timer levels to use at daemon startup
  // and process object initialization respectively.  There is an outside
  // chance that the level would be determined unavailable by the rtinst
  // library upon application startup.  Asserts if there is a mismatch.
  void verifyTimerLevels();
  // Sets the wall and cpu time retrieval functions to use in the the rtinst
  // library by setting a function ptr in the rtinst library to the address
  // of the chosen function.
  void writeTimerLevels();
  private:
  // helper routines for writeTimerLevels
  void writeTimerFuncAddr(const char *rtinstVar, const char *rtinstHelperFPtr);
  bool writeTimerFuncAddr_(const char *rtinstVar,const char *rtinstHelperFPtr);

  // returns the address to assign to a function pointer that will
  // allow the time querying function to be called (in the rtinst library)
  // on AIX, this address returned will be the address of a structure which 
  //   has a field that points to the proper querying function (function 
  //   pointers are handled differently on AIX)
  // on other platforms, this address will be the address of the time
  // querying function in the rtinst library
  Address getTimerQueryFuncTransferAddress(const char *helperFPtr);

  // handles setting time retrieval functions for the case of a 64bit daemon
  // and 32bit application
  // see process.C definition for why being disabled
  //bool writeTimerFuncAddr_Force32(const char *rtinstVar, 
  //			  const char *rtinstFunc);

 private:
  // Platform dependent (ie. define in platform files) process time retrieval
  // function for daemon.  Use process::getCpuTime instead of calling these
  // functions directly.  If platform doesn't implement particular level,
  // still need to define a definition (albeit empty).  Ignores lwp arg if
  // lwp's are irrelevant for platform. The file descriptor argument "fd"
  // is used.

  // NOTE: It is the caller's responsibility to check for rollbacks!

  rawTime64 getRawCpuTime_hw(int lwp);
  rawTime64 getRawCpuTime_sw(int lwp);

  // function always returns true, used when timer level is always available
  bool yesAvail();
  // The process time time mgr.  This handles choosing the best timer level
  // to use.  Call getTime member with a process object and an integer lwp
  // as args.
  typedef timeMgr<pd_process, int> cpuTimeMgr_t;
  cpuTimeMgr_t *cpuTimeMgr;

#if defined(os_linux)
  bool isPapiAvail();           
#endif
#ifdef rs6000_ibm_aix4_1
  bool isPmapiAvail();
#endif
  public:
  
   // ========  PASS THROUGH FUNCTIONS ===============================
	static unsigned calculate_Checksum( pdstring graph);

   BPatch_process *get_dyn_process() {
      return dyninst_process;
   }

   bool isStopped() const;
   bool isTerminated() const;
   bool isDetached() const;
   bool continueProc();
   bool pauseProc();

   int getPid() const { return dyninst_process->getPid(); }

   shmMgr *getSharedMemMgr() { return sharedMemManager; }

   //Callbacks that are invoked from Dyninst when a thread is created or destroyed
   static void pdNewThread(BPatch_process *proc, BPatch_thread *thr); 
   static void pdDeadThread(BPatch_process *proc, BPatch_thread *thr); 

   bool detachProcess(const bool leaveRunning) {
      return dyninst_process->detach(leaveRunning);
   }

   bool hasExited() { return dyninst_process->isTerminated();}
   bool wasCreatedViaAttach() const {return created_via_attach;}

   bool isPARADYNBootstrappedYet() const {
       // Good enough approximation (should use a flag here)
       return reachedLibState(paradynRTState, libReady);       
   }

   bool installInstrRequests(const pdvector<pdinstMapping*> &requests); 

   unsigned postRPCtoDo(BPatch_snippet &action, bool noCost,
                        BPatchOneTimeCodeCallback callbackFunc,
                        void *userData, 
                        bool runWhenFinished,
                        bool lowmem,
                        BPatch_thread *thr);
   
   //bool triggeredInStackFrame(BPatch_frame &frame, BPatch_point *point,
   //BPatch_callWhen when, BPatch_snippetOrder order);
   
   bool walkStacks(pdvector<BPatch_Vector<BPatch_frame> > &stackWalks);
   
   /*
   int_function *findOnlyOneFunction(resource *func, resource *mod) {
      process *llproc = dyninst_process->lowlevel_process();
      return llproc->findOnlyOneFunction(func, mod);
   }

   int_function *findOnlyOneFunction(const pdstring &func_name) const {
      process *llproc = dyninst_process->lowlevel_process();
      return llproc->findOnlyOneFunction(func_name);
   }
   */

   BPatch_Vector<BPatch_module *> *getAllModules() {
      return dyninst_process->getImage()->getModules();
   }
   // Parse name into library name and function name. If no library is specified,
   // lib gets the empty string "".
   //
   // e.g. libc.so/read => lib_name = libc.so, func_name = read
   //              read => lib_name = "", func_name = read
   void getLibAndFunc(const pdstring &name, pdstring &lib_name, pdstring &func_name) {

     unsigned index = 0;
     unsigned length = name.length();
     bool hasNoLib = true;

     // clear the strings
     lib_name = "";
     func_name = "";

     for (unsigned i = length-1; i > 0 && hasNoLib; i--) {
       if (name[i] == '/') {
         index = i;
         hasNoLib = false;
       }
     }

     if (hasNoLib) {
       // No library specified
       func_name = name;
     } else {
         // Grab library name and function name
         lib_name = name.substr(0, index);
         func_name = name.substr(index+1, length-index);
     }
   }


   bool findAllFuncsByName(const pdstring &name,
                           BPatch_Vector<BPatch_function *> &res) {
     //process *llproc = dyninst_process->lowlevel_process();
     BPatch_image *appImage = dyninst_process->getImage();

     // Split name into library and function
     pdstring lib_name;
     pdstring func_name;
     getLibAndFunc(name, lib_name, func_name);

     if (lib_name != "") {
        // library specified, search for module first, then do function
        //  search by module.
        BPatch_Vector<BPatch_module *> *mods;
        mods = appImage->getModules();
        BPatch_module *target_mod = NULL;
        for (unsigned int i = 0; i < mods->size(); ++i) {
          char mname[512];
          BPatch_module *mod = (*mods)[i];
          assert(mod);
          mod->getName(mname, 512);
          if (pdstring(mname) == lib_name) {
            target_mod = mod;
            break;
          }
        }
        if (!target_mod) {
           //fprintf(stderr, "%s[%d]:  could not find module %s in search for %s\n",
           //       __FILE__, __LINE__, lib_name.c_str(), func_name.c_str());
           //goto func_only_search;
           return false;
        }
        unsigned old_size = res.size();
        if (!target_mod->findFunction(func_name.c_str(), res) 
            || old_size == res.size()) {
          fprintf(stderr, "%s[%d]:  could not find func %s in mod %s\n",
                 __FILE__, __LINE__, func_name.c_str(), lib_name.c_str());
          return false;
        }
        else 
          return true;
     }
  //func_only_search:
    unsigned  old_size = res.size();
    if (NULL == appImage->findFunction(func_name.c_str(), res, false)
       || old_size == res.size()) {
       //fprintf(stderr, "%s[%d]: function %s not found in image\n",
       //        __FILE__, __LINE__, func_name.c_str());
      return false;
    } 
    return true;
   }

   bool findAllFuncsByName(resource *func, resource *mod,
                           BPatch_Vector<BPatch_function *> &res);
   pd_image *getImage() const { return img; }

   unsigned long getOSHandle();
   void initOSHandle();
   void closeOSHandle();

   BPatch_Vector<BPatch_function *> *getIncludedFunctions(BPatch_module *mod); 

   BPatch_Vector<BPatch_function *> *getIncludedFunctions(); 

   pdvector<BPatch_module *> *getIncludedModules(pdvector<BPatch_module *> *buf); 

   BPatch_module *findModule(const pdstring &mod_name,bool check_excluded); 

   unsigned maxNumberOfThreads() {
     if(multithread_capable())
        return MAX_NUMBER_OF_THREADS;
     else
        return 1;
   }

   BPatch_function *getMainFunction() const {
      BPatch_Vector<BPatch_function *> bpfv;
      if (!dyninst_process->getImage()->findFunction("main", bpfv) || !bpfv.size()) {
        fprintf(stderr, "%s[%d]:  findFunction(main... ) failed\n", __FILE__, __LINE__);
        return NULL;
      }
      if (bpfv.size() > 1) {
        fprintf(stderr, "%s[%d]:  findFunction(main... ) got %d matches\n", __FILE__, __LINE__, bpfv.size());
      }
      return bpfv[0];
   }

   bool dumpCore(const pdstring coreFile) { 
     return dyninst_process->dumpCore(coreFile.c_str(), false);
   }

   virtualTimer *getVirtualTimer(unsigned index);

#ifdef PAPI
   papiMgr* getPapiMgr() {  return dyninst_process->getPapiMgr();  }
#endif

   bool multithread_capable() {
      return dyninst_process->isMultithreadCapable();
   }

   bool wasRunningWhenAttached() { 
       //  this is only used by newProgramCallbackFunc, but we still need it???
       return dyninst_process->wasRunningWhenAttached();
   }

   pd_thread *STthread() { 
      assert(! multithread_capable());
      if(numThr() == 0)
         return NULL;
      return (*beginThr());
   }

   void initAfterFork(pd_process *parentProc);
      
  /************************************
   *** Fork and Exec handling       ***
   ************************************/
  public:
   static void paradynPreForkDispatch(BPatch_thread *parent,
                                      BPatch_thread *child);
   static void paradynPostForkDispatch(BPatch_thread *parent,
                                       BPatch_thread *child);
   static void paradynExecDispatch(BPatch_thread *thread);
   static void paradynExitDispatch(BPatch_thread *thread,
                                   BPatch_exitType exit_type);

  private:
   void preForkHandler();
   void postForkHandler(BPatch_process *child);
   void execHandler();
   void exitHandler();
   
  /************************************
   *** Runtime library functions    ***
   ************************************/
  public:
  // Returns once paradyn lib is loaded and initialized
   typedef enum { create_load, attach_load, exec_load } load_cause_t;
   bool loadParadynLib(load_cause_t ldcause);
   void report_CallGraphChecksumToFE( void );
   resource::ChecksumType calculateCallGraphChecksum( void );
	 void setCanReportResources(bool b) { canReportResources_ = b; }

  private:
   libraryState_t paradynRTState;
   libraryState_t auxLibState; // Needed for solaris
   
   void setParadynLibLoaded() { setLibState(paradynRTState, libLoaded); }

   //This function allows the OS specific files to do initialization
   //just before the Paradyn RT library loads
   void initOSPreLib();

   // Replace with BPatch::loadLibrary
   bool loadAuxiliaryLibrary(pdstring libname);
   static void loadAuxiliaryLibraryCallback(process *, unsigned /* rpc_id */,
                                            void *data, void *ret);
   
   // Sets the parameters to paradynInit
   bool runParadynInit(load_cause_t ldcause);
   
   // Handles final initialization
   bool finalizeParadynLib(load_cause_t ldcause);
   bool getParadynRTname();
   
   bool canReportResources() { return canReportResources_; }
   /*************************************************************
    **** Process state variables                             ****
    *************************************************************/

  // Total observed cost. To avoid 64-bit math in the base tramps, we
  // use a 32-bit temporary accumulator and periodically dump it to
  // this variable.
  uint64_t cumulativeObsCost;
  unsigned lastObsCostLow; // Value of counter last time we checked it

  void processCost(unsigned obsCostLow, timeStamp wallTime,
                   timeStamp processTime);

 private:
   
   bool inExec;
   
   pdstring paradynRTname;
   
   bool canReportResources_;
   /*************************************************************
    **** Shared Memory handling                              ****
    *************************************************************/
   shmMgr *sharedMemManager;
   sharedMetaData *shmMetaData;

};


// Shouldn't these be static members of class pd_process?
pd_process *pd_createProcess(pdvector<pdstring> &argv, pdstring dir);
pd_process *pd_attachProcess(const pdstring &progpath, int pid);
pd_process *pd_attachToCreatedProcess(const pdstring &progpath, int pid);

pdstring formatLibParadynName(pdstring orig);

#endif













