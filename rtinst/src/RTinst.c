/*
 * Copyright (c) 1996-2001 Barton P. Miller
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

/************************************************************************
 *
 * $Id: RTinst.c,v 1.78 2003/10/22 17:57:05 pcroth Exp $
 * RTinst.c: platform independent runtime instrumentation functions
 *
 ************************************************************************/

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>

#if defined(i386_unknown_nt4_0) /* ccw 4 jun 2002 */
#include <process.h>
#define getpid _getpid
#endif

#if !defined(i386_unknown_nt4_0)
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#include "kludges.h"
#include "rtinst/h/rtinst.h"
#include "rtinst/h/trace.h"
#if defined(MT_THREAD)
#include "RTthread.h"
#endif

#ifdef PARADYN_MPI
#include "/usr/lpp/ppe.poe/include/mpi.h"
/* #include <mpi.h> */
#endif

#include "RTcompat.h"
#include "pdutil/h/resource.h"

extern void makeNewShmSegCopy();

#if defined(MT_THREAD)
extern const char V_libparadynMT[];
#else
extern const char V_libparadynRT[];
#endif

#ifdef DEBUG_PRINT_RT
int PARADYNdebugPrintRT = 1;
#else
int PARADYNdebugPrintRT = 0;
#endif

/* Pointer to the shared data structure */
RTsharedData_t RTsharedData;
/* And max # of threads allowed */
unsigned MAX_NUMBER_OF_THREADS;

/*extern void   DYNINSTos_init(int calledByFork, int calledByAttach); ccw 22 apr 2002 : SPLIT */
extern void   PARADYNos_init(int calledByFork, int calledByAttach);

/* platform dependent functions */
extern void PARADYNbreakPoint(void);
extern void DYNINST_install_ualarm(unsigned interval);
extern void DYNINSTinitTrace(int);
extern void DYNINSTflushTrace(void);
extern int DYNINSTwriteTrace(void *, unsigned);
extern void DYNINSTcloseTrace(void);
extern void *DYNINST_shm_init(int, int, int *);

void DYNINSTprintCost(void);
int DYNINSTTGroup_CreateUniqueId(int);
void DYNINSTtagGroupInfo_Init( void );

int DYNINSTCalleeSearch(unsigned int callSiteAddr,                                                        unsigned int calleeAddr);
/************************************************************************/

static float  DYNINSTsamplingRate   = 0;
static int    DYNINSTtotalSamples   = 0;
static tTimer DYNINSTelapsedCPUTime;
static tTimer DYNINSTelapsedTime;

static int DYNINSTnumReported = 0;
static rawTime64 DYNINSTtotalSampleTime = 0;

char DYNINSThasInitialized = 0; /* 0 : has not initialized
				   2 : initialized by Dyninst
				   3 : initialized by Paradyn */

/************************************************************************/

/************************************************************************/

/* these vrbles are global so that fork() knows the attributes of the
   shm segs we've been using */
int DYNINST_shmSegKey;
int DYNINST_shmSegNumBytes;
int DYNINST_shmSegShmId; /* needed? */
void *DYNINST_shmSegAttachedPtr;
int DYNINST_mutatorPid = -1; /* set in DYNINSTinit(); pass to connectToDaemon();
				 needed if we fork */
int local_DYNINST_mutatorPid = -1;

#ifdef USE_PROF
int DYNINSTbufsiz;
int DYNINSTprofile;
int DYNINSTprofScale;
int DYNINSTtoAddr;
short *DYNINSTprofBuffer;
#endif


/*
 * New heap naming scheme: we encode the size and type of the heap
 * in the name, instead of having it implicitly known. The format is:
 * DYNINSTstaticHeap_<size>_<type>_garbage
 */

/* ccw 22 apr 2002 : SPLIT removed these heaps because they are in
 	DYNINST */
/* 
double DYNINSTstaticHeap_32K_lowmemHeap_1[(32*1024)/sizeof(double)];
double DYNINSTstaticHeap_4M_anyHeap_1[(4*1024*1024)/sizeof(double)];
*/
/* As DYNINSTinit() completes, it has information to pass back
   to paradynd.  The data can differ; more stuff is needed
   when SHM_SAMPLING is defined, for example.  But in any event,
   the following gives us a general framework.  When DYNINSTinit()
   is about to complete, we used to send a TR_START trace record back and then
   DYNINSTbreakPoint().  But we've seen strange behavior; sometimes
   the breakPoint is received by paradynd first.  The following should
   work all the time:  DYNINSTinit() writes to the following vrble
   and does a DYNINSTbreakPoint(), instead of sending a trace
   record.  Paradynd reads this vrble using ptrace(), and thus has
   the info that it needs.
   Note: we use this framework for DYNINSTfork(), too -- so the TR_FORK
   record is now obsolete, along with the TR_START record.
   Additionally, we use this framework for DYNINSTexec() -- so the
   TR_EXEC record is now obsolete, too */
/* struct DYNINST_bootstrapStruct DYNINST_bootstrap_info; ccw 18 apr 2002 : SPLIT */
struct PARADYN_bootstrapStruct PARADYN_bootstrap_info;

#define N_FP_REGS 33

volatile int DYNINSTsampleMultiple    = 1;
   /* written to by dynRPC::setSampleRate() (paradynd/dynrpc.C)
      (presumably, upon a fold) */

/* Written to by daemon just before launching an inferior RPC */
/*rpcInfo curRPC = { 0, 0, 0 }; ccw 18 apr 2002 SPLIT */

unsigned pcAtLastIRPC;  /* just used to check for errors */
/* 1 = a trap was ignored and needs to be regenerated
   0 = there is not a trap that hasn't been processed */
int trapNotHandled = 0;

/* These variables are used for the rtinst library to suggest to the daemon
   which timer level the daemon should setup for the rtinst library.  The
   daemon has the responsibility for making the final decision.  The purpose
   for this is that sometimes the rtinst library only has access to needed to
   make decision and sometimes the decision is best handled by the daemon. */
int hintBestCpuTimerLevel  = UNASSIGNED_TIMER_LEVEL;
int hintBestWallTimerLevel = UNASSIGNED_TIMER_LEVEL;



/* These are used to assign to PARADYNgetCPUtime.  On some systems like AIX,
   a function pointer is actually a pointer to a structure which then points
   to the function.  In the case of AIX, these variables will contain the
   address of the structure (with a member that points to the function).  The
   daemon can then read these variables to find the address of the structure.
   On other systems, these variables just store the actual function address.
   FPtrInfo = the address of the object which gives information as to how to
              call a function pointer
*/
timeQueryFuncPtr_t swCpuTimeFPtrInfo  = &DYNINSTgetCPUtime_sw;
timeQueryFuncPtr_t hwCpuTimeFPtrInfo  = &DYNINSTgetCPUtime_hw;
timeQueryFuncPtr_t swWallTimeFPtrInfo = &DYNINSTgetWalltime_sw;
timeQueryFuncPtr_t hwWallTimeFPtrInfo = &DYNINSTgetWalltime_hw;

/* Set time retrieval functions to the software level.  The daemon
   will reset these to a "better" time querying function if available. */
timeQueryFuncPtr_t PARADYNgetCPUtime  = &DYNINSTgetCPUtime_sw;
timeQueryFuncPtr_t PARADYNgetWalltime = &DYNINSTgetWalltime_sw;

/************************************************************************
 * void DYNINSTstartProcessTimer(tTimer* timer)
************************************************************************/
void
DYNINSTstartProcessTimer(tTimer* timer) {
   /* For shared-mem sampling only: bump protector1, do work, then bump
      protector2 */
   assert(timer->protector1 == timer->protector2);
   timer->protector1++;
   MEMORY_BARRIER;
   if (timer->counter == 0) {
      timer->start     =  DYNINSTgetCPUtime();
   }
   timer->counter++;
   MEMORY_BARRIER;
   timer->protector2++; /* ie. timer->protector2 == timer->protector1 */
   assert(timer->protector1 == timer->protector2);
}


/************************************************************************
 * void DYNINSTstopProcessTimer(tTimer* timer)
************************************************************************/
void
DYNINSTstopProcessTimer(tTimer* timer) {
  assert(timer->protector1 == timer->protector2);
  timer->protector1++;
  MEMORY_BARRIER;
  if (timer->counter == 0) {
    /* a strange condition; shouldn't happen.  Should we make it an assert
       fail? */
  }
  else {
    if (timer->counter == 1) {
        const rawTime64 now = DYNINSTgetCPUtime();
        timer->total += (now - timer->start);
      if (now < timer->start) {
          abort();
      }
    }
    timer->counter--;
  }
  MEMORY_BARRIER;
  timer->protector2++; /* ie. timer->protector2 == timer->protector1 */
  assert(timer->protector1 == timer->protector2);
}

/************************************************************************
 * void DYNINSTstartWallTimer(tTimer* timer)
 ************************************************************************/

void
DYNINSTstartWallTimer(tTimer* timer) {
  assert(timer->protector1 == timer->protector2);
  timer->protector1++;
  MEMORY_BARRIER;
  if (timer->counter == 0) {
    timer->start = DYNINSTgetWalltime();
  }
  timer->counter++;
  MEMORY_BARRIER;
  timer->protector2++; /* ie. timer->protector2 == timer->protector1 */
  assert(timer->protector1 == timer->protector2);
}


/************************************************************************
 * void DYNINSTstopWallTimer(tTimer* timer)
************************************************************************/
void
DYNINSTstopWallTimer(tTimer* timer) {
  int i;
  assert(timer->protector1 == timer->protector2);
  timer->protector1++;
  MEMORY_BARRIER;
  if (timer->counter == 0)
    ;
  else if (--timer->counter == 0) {
    const rawTime64 now = DYNINSTgetWalltime();
    
    timer->total += (now - timer->start);
  }
  MEMORY_BARRIER;
  timer->protector2++; /* ie. timer->protector2 == timer->protector1 */
  assert(timer->protector1 == timer->protector2);
}

/************************************************************************
 * void saveFPUstate(float* base)
 * void restoreFPUstate(float* base)
 *
 * save and restore state of FPU on signals.  these are null functions
 * for most well designed and implemented systems.
************************************************************************/
static void
saveFPUstate(float* base) {
#if defined(i386_unknown_solaris2_5) || defined(i386_unknown_linux2_0) || defined(ia64_unknown_linux2_4) /* Temporary duplication - TLM */
    /* kludge for the pentium: we need to reset the FPU here, or we get 
       strange results on fp operations.
    */
    asm("finit");
#endif
}

static void
restoreFPUstate(float* base) {
}

/************************************************************************
 * void DYNINSTgenerateTraceRecord(traceStream sid, short type,
 *   short length, void* data, int flush, rawTime64 wall_time, 
 *   rawTime64 process_time)
************************************************************************/
void
DYNINSTgenerateTraceRecord(traceStream sid, short type, short length,
			   void *eventData, int flush,
			   rawTime64 wall_time, rawTime64 process_time) {
    static unsigned pipe_gone = 0;
    static unsigned inDYNINSTgenerateTraceRecord = 0;
    traceHeader     header;
    int             count;
    char            buffer[1024];

#if defined(MT_THREAD)
    if (DYNINST_DEAD_LOCK == tc_lock_lock(&DYNINST_traceLock)){
      return ;
    }
#endif /*MT_THREAD*/

    if (inDYNINSTgenerateTraceRecord) return;
    inDYNINSTgenerateTraceRecord = 1;

    if (pipe_gone) {
        inDYNINSTgenerateTraceRecord = 0;
        return;
    }

    header.wall    = wall_time;
    header.process = process_time;
#ifdef ndef
    if(type == TR_SAMPLE){
	 traceSample *s = (traceSample*)eventData;
         printf("wall time = %f processTime = %f value = %f\n",
		(double)(header.wall/1000000.0), 
		(double)(header.process/1000000.0),
		s->value);
    }
#endif

    length = ALIGN_TO_WORDSIZE(length);

    header.type   = type;
    header.length = length;

    count = 0;
    memcpy(&buffer[count], &sid, sizeof(traceStream));
    count += sizeof(traceStream);

    memcpy(&buffer[count], &header, sizeof(header));
    count += sizeof(header);

    count = ALIGN_TO_WORDSIZE(count);
    memcpy(&buffer[count], eventData, length);
    count += length;

    
    if (!DYNINSTwriteTrace(buffer, count))
      pipe_gone = 1;

    if (flush) DYNINSTflushTrace();

    inDYNINSTgenerateTraceRecord = 0;
#if defined(MT_THREAD)
    tc_lock_unlock(&DYNINST_traceLock) ;
#endif /*MT_THRAED*/

}

static void shmsampling_printf(const char *fmt, ...) {
#ifdef SHM_SAMPLING_DEBUG
   va_list args;
   va_start(args, fmt);

   vfprintf(stderr, fmt, args);

   va_end(args);

   fflush(stderr);
#endif
}

double DYNINSTdummydouble = 4321.71; /* Global so the compiler won't
					optimize away FP code in initFPU */
static void initFPU()
{
       /* Init the FPU.  We've seen bugs with Linux (e.g., Redhat 6.2
	  stock kernel on PIIIs) where processes started by Paradyn
	  started with FPU uninitialized. */
       double x = 17.1234;
       DYNINSTdummydouble *= x;
}



/************************************************************************
 * void DYNINSTinit()
 *
 * initialize the DYNINST library.  this function is called at the start
 * of the application program, as well as after a fork, and after an
 * attach.
 *
 ************************************************************************/
void PARADYNinit(int paradyndPid,
                 int creationMethod,
                 int numThreads,
                 int theKey, 
                 unsigned shmSegSize,
                 unsigned offsetToSharedData) {
    /* On many platforms we can't call this function directly, so
       at initialization we write values into global variables and
       then call a wrapper function (below) */
    
    int i;
    int calledFromAttachToCreated = (creationMethod == 3);
    int calledFromFork = (creationMethod == 2);
    int calledFromAttach = (creationMethod == 1);
    int calledFromExec = (creationMethod == 4);

    MAX_NUMBER_OF_THREADS = numThreads;
    
#ifdef SHM_SAMPLING_DEBUG
    char thehostname[80];
    extern int gethostname(char*,int);
    
   (void)gethostname(thehostname, 80);
   thehostname[79] = '\0';
   
   /*  
       shmsampling_printf("WELCOME to DYNINSTinit (%s, pid=%d), args are %d, %d, %d\n",
       thehostname, (int)getpid(), theKey, shmSegNumBytes,
       paradyndPid);
   */
#endif
   
   initFPU();
   DYNINSThasInitialized = 3;

   /* sanity check */
   assert(sizeof(rawTime64) == 8);
   assert(sizeof(int64_t) == 8);
   assert(sizeof(int32_t) == 4);
 
   DYNINST_mutatorPid = paradyndPid; /* important -- needed in case we fork() */
  
   /* initialize the tag and group info */
   DYNINSTtagGroupInfo_Init();
   
   if (!calledFromFork) {
      RTsharedData_t *RTsharedInShm;
      char *endOfShared;
      Address shmBase;
      unsigned i;
      rpcToDo **pendingRPCs;
      
      DYNINST_shmSegKey = theKey;
      DYNINST_shmSegNumBytes = shmSegSize;
      DYNINST_shmSegAttachedPtr = DYNINST_shm_init(theKey, shmSegSize,
                                                   &DYNINST_shmSegShmId);      
      shmBase = (Address) DYNINST_shmSegAttachedPtr;

      /* Yay, pointer arithmetic */
      RTsharedInShm = (RTsharedData_t *)
         (shmBase + offsetToSharedData);
      RTsharedData.cookie = (unsigned *)
         ((Address) RTsharedInShm->cookie + shmBase);
      RTsharedData.inferior_pid = (unsigned *)
         ((Address) RTsharedInShm->inferior_pid + shmBase);
      RTsharedData.daemon_pid = (unsigned *)
         ((Address) RTsharedInShm->daemon_pid + shmBase);
      RTsharedData.observed_cost = 
         (unsigned *) ((Address) RTsharedInShm->observed_cost + shmBase);

      RTsharedData.virtualTimers = (virtualTimer *)
         ((Address) RTsharedInShm->virtualTimers + shmBase);
      RTsharedData.indexToThread = (unsigned *)
         ((Address) RTsharedInShm->indexToThread + shmBase);
      RTsharedData.pendingIRPCs = 
         malloc(sizeof(rpcToDo *)*MAX_NUMBER_OF_THREADS);
      RTsharedData.pendingIRPCs[0] = NULL;
      
      pendingRPCs = (rpcToDo **)
      ((unsigned) RTsharedInShm->pendingIRPCs + (unsigned) shmBase);
 
      for (i = 0; i < MAX_NUMBER_OF_THREADS; i++) { 
          RTsharedData.pendingIRPCs[i] = (rpcToDo *)
          ((Address) pendingRPCs[i] + shmBase);
      }
   }
   
   /*
     In accordance with usual stdio rules, stdout is line-buffered and stderr
     is non-buffered.  Unfortunately, stdio is a little clever and when it
     detects stdout/stderr redirected to a pipe/file/whatever, it changes to
     fully-buffered.  This indeed occurs with us (see paradynd/src/process.C
     to see how a program's stdout/stderr are redirected to a pipe). So we
     reset back to the desired "bufferedness" here.  See stdio.h for these
     calls.  When attaching, stdio isn't under paradynd control, so we don't
     do this stuff.

     Note! Since we are messing with stdio stuff here, it should go without
     saying that DYNINSTinit() (or at least this part of it) shouldn't be
     invoked until stdio has been initialized!
   */
  
   if (!calledFromAttach) {
      setvbuf(stdout, NULL, _IOLBF, 0);
      /* make stdout line-buffered.  "setlinebuf(stdout)" is cleaner but HP
         doesn't seem to have it */
      setvbuf(stderr, NULL, _IONBF, 0);
      /* make stderr non-buffered */
   }
  
#if defined(MT_THREAD)
   RTprintf("%s\n", V_libparadynMT);
#else
   RTprintf("%s\n", V_libparadynRT);
#endif

#ifdef PAPI
   initPapi(); 
#endif

   PARADYNos_init(calledFromFork, calledFromAttach);

#ifdef USE_PROF
   {
      extern int end;
    
      DYNINSTprofScale = sizeof(short);
      DYNINSTtoAddr = sizeof(short);
      DYNINSTbufsiz = (((unsigned int) &end)/DYNINSTprofScale) + 1;
      DYNINSTprofBuffer = (short *) calloc(sizeof(short), DYNINSTbufsiz);
      profil(DYNINSTprofBuffer, DYNINSTbufsiz*sizeof(short), 0, 0xffff);
      DYNINSTprofile = 1;
   }
#endif

   /* Fill in info for paradynd to receive: */
  
   PARADYN_bootstrap_info.ppid = -1; /* not needed really */ /* was DYNINST_ ccw 18 apr 2002 SPLIT */

  
   if (!calledFromFork) {
      shmsampling_printf("DYNINSTinit setting appl_attachedAtPtr in bs_record"
                         " to 0x%x\n", (Address)DYNINST_shmSegAttachedPtr);
      PARADYN_bootstrap_info.appl_attachedAtPtr.ptr = DYNINST_shmSegAttachedPtr;
   }
   PARADYN_bootstrap_info.pid = getpid(); /* was DYNINST_ ccw 18 apr 2002 SPLIT */ 
#if !defined(i386_unknown_nt4_0)
   if (calledFromFork)
      PARADYN_bootstrap_info.ppid = getppid(); /* was DYNINST_ ccw 18 apr 2002 SPLIT */
#else
   PARADYN_bootstrap_info.ppid = 0; /* was DYNINST_ ccw 18 apr 2002 SPLIT */
#endif 

   PARADYN_bootstrap_info.tramp_guard_base = (int *)
      malloc(numThreads*sizeof(int));
   for (i = 0; i < numThreads; i++)
      PARADYN_bootstrap_info.tramp_guard_base[i] = 1; /* default value */

   /* We do this field last as a way to synchronize; paradynd will ignore what it
      sees in this structure until the event field is nonzero */
   /* was DYNINST_ ccw 18 apr 2002 SPLIT */
   if (calledFromFork)
      PARADYN_bootstrap_info.event = 2; /* 2 --> end of DYNINSTinit (forked process) */
   else if (calledFromAttach)
      PARADYN_bootstrap_info.event = 3; /* 3 --> end of DYNINSTinit (attached proc) */
   else if (calledFromExec)
      PARADYN_bootstrap_info.event = 4;
   else				   
      PARADYN_bootstrap_info.event = 1; /* 1 --> end of DYNINSTinit (normal or when
                                           called by exec'd proc or attachedTocreated case) */

   /* If attaching, now's the time where we set up the trace stream connection fd */
   if (calledFromAttach) {
      int pid = getpid();
#if !defined(i386_unknown_nt4_0)
      int ppid = getppid();
#else
      int ppid = 0;
#endif
      unsigned attach_cookie = 0x22222222;
      int32_t ptr_size;
    
      DYNINSTinitTrace(paradyndPid);
    
      DYNINSTwriteTrace(&attach_cookie, sizeof(attach_cookie));
      DYNINSTwriteTrace(&pid, sizeof(pid));
      DYNINSTwriteTrace(&ppid, sizeof(ppid));
      DYNINSTwriteTrace(&DYNINST_shmSegKey, sizeof(DYNINST_shmSegKey));
      ptr_size = sizeof(DYNINST_shmSegAttachedPtr);
      DYNINSTwriteTrace(&ptr_size, sizeof(int32_t));
      DYNINSTwriteTrace(&DYNINST_shmSegAttachedPtr, ptr_size);
      DYNINSTflushTrace();
   }
   else if (!calledFromFork) {
      /* either normal startup or startup via a process having exec'd or attachtoCreated case */
#if !defined(i386_unknown_nt4_0)
      if (calledFromAttachToCreated){
         /* unique case identified as attachToCreated */
         int cookie = 0;  /*  not attach, not fork */
         int pid = getpid();
         int ppid = paradyndPid; /* paradynd is seen as the father because it is attached
                                    to the application */
         int32_t ptr_size;

         DYNINSTinitTrace(paradyndPid);
         DYNINSTwriteTrace(&cookie, sizeof(cookie));
         DYNINSTwriteTrace(&pid, sizeof(pid));
         DYNINSTwriteTrace(&ppid, sizeof(ppid));
         DYNINSTflushTrace();/* we needed it */
         DYNINSTwriteTrace(&DYNINST_shmSegKey, sizeof(DYNINST_shmSegKey)); 
         ptr_size = sizeof(DYNINST_shmSegAttachedPtr);
         DYNINSTwriteTrace(&ptr_size, sizeof(int32_t));
         DYNINSTwriteTrace(&DYNINST_shmSegAttachedPtr, ptr_size);
         DYNINSTflushTrace(); /* we needed it */
      }
      else {/* trace stream is already open */ 
         DYNINSTinitTrace(-1);
      }
#else
      /* need to get a connection to daemon */
      int cookie = 0;
      int pid = getpid();
      int ppid = paradyndPid;
      int32_t ptr_size;

      DYNINSTinitTrace(paradyndPid);
      DYNINSTwriteTrace(&cookie, sizeof(cookie));
      DYNINSTwriteTrace(&pid, sizeof(pid));
      DYNINSTwriteTrace(&ppid, sizeof(ppid));
      DYNINSTwriteTrace(&DYNINST_shmSegKey, sizeof(DYNINST_shmSegKey));
      ptr_size = sizeof(DYNINST_shmSegAttachedPtr);
      DYNINSTwriteTrace(&ptr_size, sizeof(int32_t));
      DYNINSTwriteTrace(&DYNINST_shmSegAttachedPtr, ptr_size);
#endif
   }
   else
      /* calledByFork -- DYNINSTfork already called DYNINSTinitTrace */
      ;
  
   /* MT_THREAD */
#if defined(MT_THREAD)
   {
      /* Note: until this point we are not multithread-safe. */
      DYNINST_initialize_once((char*) DYNINST_shmSegAttachedPtr, numThreads);

      DYNINST_reportThreadUpdate(calledFromAttach?FLAG_ATTACH:FLAG_INIT) ;
   }
#endif

   /* db_init(db_shmKey, sizeof(db_shmArea_t)); */

   DYNINSTstartWallTimer(&DYNINSTelapsedTime);
   DYNINSTstartProcessTimer(&DYNINSTelapsedCPUTime);
   shmsampling_printf("leaving DYNINSTinit (pid=%d) --> the process is running freely now\n", (int)getpid());
}


/*
 * These are used by DllMain/_init to call PARADYNinit
 */
int libparadynRT_init_localparadynPid=-1;
int libparadynRT_init_localcreationMethod=-1;
int libparadynRT_init_localmaxThreads=-1;
int libparadynRT_init_localtheKey=-1;
int libparadynRT_init_localshmSegNumBytes=-1;
int libparadynRT_init_localoffset=-1;


#if defined(i386_unknown_nt4_0)  /*ccw 4 jun 2002*/
#include <windows.h>

/* this function is automatically called when windows loads this dll
 if we are launching a mutatee to instrument, dyninst will place
 the correct values in libparadynRT_init_local* 
 and they will be passed to
 PARADYNinit to correctly initialize the dll.  this keeps us
 from having to instrument two steps from the mutator (load and then 
 the execution of PARADYNinit()
*/
int pDllMainCalledOnce=0;
BOOL WINAPI DllMain(
  HINSTANCE hinstDLL,  /* handle to DLL module */
  DWORD fdwReason,     /* reason for calling function */
  LPVOID lpvReserved   /* reserved */
){
	if(pDllMainCalledOnce){
	}else{
		if(libparadynRT_init_localparadynPid != -1 &&
           libparadynRT_init_localcreationMethod != -1 &&
           libparadynRT_init_localtheKey != -1 &&
           libparadynRT_init_localshmSegNumBytes != -1){
            pDllMainCalledOnce++;
            PARADYNinit(libparadynRT_init_localparadynPid,
                        libparadynRT_init_localcreationMethod,
                        libparadynRT_init_localmaxThreads, /* Number of threads */
                        libparadynRT_init_localtheKey,
                        libparadynRT_init_localshmSegNumBytes,
                        libparadynRT_init_localoffset);
		}
	}
	return 1; 
}
 

#else


#ifdef __GNUC
__attribute__ ((constructor)) void libparadynRT_init(void);
#endif


// Unix platforms
int PARADYNinitCalledOnce=0;

void libparadynRT_init() {
    if(PARADYNinitCalledOnce) return;
    else {
		if(libparadynRT_init_localparadynPid != -1 &&
           libparadynRT_init_localcreationMethod != -1 &&
           libparadynRT_init_localtheKey != -1 &&
           libparadynRT_init_localshmSegNumBytes != -1){
            PARADYNinitCalledOnce++;
            PARADYNinit(libparadynRT_init_localparadynPid,
                        libparadynRT_init_localcreationMethod,
                        libparadynRT_init_localmaxThreads, /* Number of threads */
                        libparadynRT_init_localtheKey,
                        libparadynRT_init_localshmSegNumBytes,
                        libparadynRT_init_localoffset);
		}
	}
}
#endif



/* bootstrap structure extraction info (see rtinst/h/trace.h) */
static struct PARADYN_bootstrapStruct _bs_dummy;
int32_t PARADYN_attachPtrSize = sizeof(_bs_dummy.appl_attachedAtPtr.ptr);


/* debug code should call forkexec_printf as a way of avoiding
   putting #ifdef FORK_EXEC_DEBUG around their fprintf(stderr, ...) statements,
   which can lead to horribly ugly code --ari */
/* similar for SHM_SAMPLING_DEBUG */
void forkexec_printf(const char *fmt, ...) {
#ifdef FORK_EXEC_DEBUG
   va_list args;
   va_start(args, fmt);

   vfprintf(stderr, fmt, args);

   va_end(args);

   fflush(stderr);
#endif
}

#if !defined(i386_unknown_nt4_0)
int PARADYN_init_child_after_fork() {
   int pid;
   int ppid;
   int ptr_size;
   int cookie = 0;
   /* we are the child process */

   /* don't want to call alloc_pos with fork, but want to reuse existing
      positions.  This is setup through DYNINSTregister_running_thread() */
#if defined(MT_THREAD)
   /* setting this to 0 here and to 1 at the end of this function
      fixes a bug we had where a threadCreate occurred in the rtinst
      library when this function was called for MT.  For MT, we don't
      want to create a new thread and pos, but want to reuse the position
      variables, using the same tid<->pos mapping as in the parent app.
   */
   DYNINST_initialize_done = 0;  
#endif
   pid = getpid();
   ppid = getppid();
   ptr_size = sizeof(DYNINST_shmSegAttachedPtr);

   setvbuf(stdout, NULL, _IOLBF, 0);
   /* make stdout line-buffered.  "setlinebuf(stdout)" is cleaner but HP
      doesn't seem to have it */
   setvbuf(stderr, NULL, _IONBF, 0);
   /* make stderr non-buffered */
   
   /* Here, we need to detach from the old shm segment, create a new one
      (in the same virtual memory location as the old one), and attach 
      to it */
   makeNewShmSegCopy();

   /* Some aspects of initialization need to occur right away (such
      as resetting the PMAPI counters on AIX) because the daemon
      may use aspects of the process before pDYNINSTinit is called */
   PARADYN_forkEarlyInit();
   PARADYNos_init(1, 0);

   DYNINSTinitTrace(DYNINST_mutatorPid);

   DYNINSTwriteTrace(&cookie, sizeof(cookie));
   DYNINSTwriteTrace(&pid, sizeof(pid));
   DYNINSTwriteTrace(&ppid, sizeof(ppid));
   DYNINSTwriteTrace(&DYNINST_shmSegKey, sizeof(DYNINST_shmSegKey));
   ptr_size = sizeof(DYNINST_shmSegAttachedPtr);
   DYNINSTwriteTrace(&ptr_size, sizeof(int32_t));
   DYNINSTwriteTrace(&DYNINST_shmSegAttachedPtr, ptr_size);
   DYNINSTflushTrace();

   return 123;
}

void DYNINSTmpi_fork(int pid) {
	if (pid == 0) {
		forkexec_printf("DYNINSTmpi_fork: child\n");

		/* we are the child process */
		makeNewShmSegCopy();
		
		if (DYNINSTnullTrace() < 0) {
			return;
		}
		forkexec_printf("DYNINSTmpi_fork: child done\n");
	}
}
#endif

void
DYNINSTexecFailed() {
    /* DYNINSTgenerateTraceRecord resets errno back to zero */
    int saved = errno;
    rawTime64 process_time = DYNINSTgetCPUtime();
    rawTime64 wall_time = DYNINSTgetWalltime();
    int pid = getpid();
    
    forkexec_printf("DYNINSTexecFAILED errno = %d\n", errno);
    
    DYNINSTgenerateTraceRecord(0, TR_EXEC_FAILED, sizeof(int), &pid, 1,
			       process_time, wall_time);
    errno = saved;
}



/************************************************************************
 * void DYNINSTprintCost(void)
 *
 * print a detailed summary of the cost of the application's run.
 * Note:  Used to be called by DYNINSTexit, since 
************************************************************************/
void
DYNINSTprintCost(void) {
    FILE *fp;
    int64_t value;
    struct endStatsRec stats;

    DYNINSTstopProcessTimer(&DYNINSTelapsedCPUTime);
    DYNINSTstopWallTimer(&DYNINSTelapsedTime);

    value = *(unsigned*)((char*)PARADYN_bootstrap_info.appl_attachedAtPtr.ptr + 12);
    stats.instCycles = value;
    stats.alarms      = 0;
    stats.numReported = DYNINSTnumReported;
    /* used only by alarm sampling */
    stats.handlerCost = 0;

    stats.totalCpuTime  = DYNINSTelapsedCPUTime.total;
    stats.totalWallTime = DYNINSTelapsedTime.total;

    stats.samplesReported = DYNINSTtotalSamples;

    stats.userTicks = 0;
    stats.instTicks = 0;

#if defined(i386_unknown_linux2_0) || defined(ia64_unknown_linux2_4) /* Temporary duplication - TLM */
    stats.totalTraps = 0; /* value in DYNINSTtotalTraps, defined in
			     libdyninstAPI_RT.so */
#endif


#ifdef USE_PROF
    fp = fopen("stats.out", "w");

    if (DYNINSTprofile) {
	int i;
	int limit;
	int pageSize;
	int startInst;
	extern void DYNINSTfirst();


	limit = DYNINSTbufsiz;
	/* first inst code - assumes data area above code space in virtual
	 * address */
	startInst = (int) &DYNINSTfirst;
	fprintf(fp, "startInst = %x\n", startInst);
	fprintf(fp, "limit = %x\n", limit * DYNINSTprofScale);
	for (i=0; i < limit; i++) {
#ifdef notdef
	    if (DYNINSTprofBuffer[i]) {
		fprintf(fp, "%x %d\n", i * DYNINSTtoAddr, 
			DYNINSTprofBuffer[i]);
	    }
#endif
	    if (i * DYNINSTtoAddr > startInst) {
		stats.instTicks += DYNINSTprofBuffer[i];
	    } else {
		stats.userTicks += DYNINSTprofBuffer[i];
	    }
	}

	/* fwrite(DYNINSTprofBuffer, DYNINSTbufsiz, 1, fp); */
	fprintf(fp, "stats.instTicks %d\n", stats.instTicks);
	fprintf(fp, "stats.userTicks %d\n", stats.userTicks);
    }
#endif

#ifdef notdef
    fprintf(fp, "DYNINSTtotalAlarmExpires %d\n", stats.alarms);
    fprintf(fp, "DYNINSTnumReported %d\n", stats.numReported);
    fprintf(fp,"Raw cycle count = %lld\n", stats.instCycles);
    fprintf(fp,"Total handler cost = %lld\n", stats.handlerCost);
    fprintf(fp,"Total cpu time of program %lld\n", stats.totalCpuTime);
    fprintf(fp,"Elapsed wall time of program %lld\n",stats.totalWallTime);
    fprintf(fp,"total data samples %d\n", stats.samplesReported);
    fprintf(fp,"Application program ticks %d\n", stats.userTicks);
    fprintf(fp,"Instrumentation ticks %d\n", stats.instTicks);

    fclose(fp);
#endif

    /* record that the exec is done -- should be somewhere better. */
    DYNINSTgenerateTraceRecord(0, TR_EXIT, sizeof(stats), &stats, 1,
			       DYNINSTgetWalltime(), DYNINSTgetCPUtime());
}


/*****************************************************************************
 * Support for throttling the resource creation mechanism when resources
 * are being used in an ephemeral manner (e.g., lots of message tags used,
 * but only one time each).
 ****************************************************************************/

/*
 * maximum number of resource creations to track when checking for
 * a high resource creation rate
 *
 * this value should be large enough that if we stop tracking creations
 * because we've run out of slots, we would likely throttle the 
 * resource creation anyway
 */
#define	DYNINSTmaxResourceCreates			1000

/* 
 * length of the sliding window used to check for high rates of
 * message tag creation
 * units are seconds
 */
#define	DYNINSTtagCreateRateWindowLength	(10)

/* the value used to indicate an undefined tag */
#define	DYNINSTGroupUndefinedTag			(-999)


/*
 * limit on the creation rate of message tags within a group
 * units are number of creations per second
 */
#define	DYNINSTtagCreationRateLimit			(1.0)


/*
 * DYNINSTresourceCreationInfo
 * 
 * Maintains info about the number of resource discovery events 
 * within a sliding window.
 */
typedef struct DYNINSTresourceCreationInfo_
{
	rawTime64		creates[DYNINSTmaxResourceCreates];	/* time of create events */
	unsigned int	nCreatesInWindow;
	double			windowLength;		/* in seconds */
	unsigned int	head;
	unsigned int	tail;
} DYNINSTresourceCreationInfo;

/*
 * DYNINSTresourceCreationInfo_Init
 *
 * Initialize a DYNINSTresourceCreationInfo to be empty.
 */
void
DYNINSTresourceCreationInfo_Init( DYNINSTresourceCreationInfo* ci,
								double windowLength )
{
	ci->windowLength = windowLength;
	ci->nCreatesInWindow = 0;
	ci->head = 0;
	ci->tail = 0;
}

/*
 * DYNINSTresourceCreationInfo_Add
 *
 * Record a resource creation event.  Has the effect of sliding
 * the sliding window forward to the indicated time.
 */
int
DYNINSTresourceCreationInfo_Add( DYNINSTresourceCreationInfo* ci,
									rawTime64 ts )
{
	int ret = 0;

	if( ci->nCreatesInWindow < (DYNINSTmaxResourceCreates - 1) )
	{
		/*
		 * ts is the leading edge of the sliding window
		 * determine new trailing edge of the sliding window
		 *
		 * Note: assumes windowLength is in seconds, and ts in nanoseconds
		 */
		rawTime64 trailingEdge = (rawTime64)(ts - (ci->windowLength * 1000000000));

		/* drop any creation events that have fallen out of the window */
		while( (ci->nCreatesInWindow > 0) && (ci->creates[ci->tail] < trailingEdge))
		{
			ci->tail = (ci->tail + 1) % DYNINSTmaxResourceCreates;
			ci->nCreatesInWindow--;
		}

		/* add the new creation event */
		if( ci->nCreatesInWindow == 0 )
		{
			/* window was empty - reset to beginning of array */
			ci->head = 0;
			ci->tail = 0;
		}
		else
		{
			ci->head = (ci->head + 1) % DYNINSTmaxResourceCreates;
		}
		ci->creates[ci->head] = ts;
		ci->nCreatesInWindow++;

		ret = 1;
	}
	return ret;
}


/*****************************************************************************
 * Support for discovering new communication groups and message tags.
 ****************************************************************************/

#define DYNINSTTagsLimit      1000
#define DYNINSTTagGroupsLimit 100
#define DYNINSTNewTagsLimit   50 /* don't want to overload the system */

typedef struct DynInstTag_st {
  int   TagGroupId;                 /* group as used by the program */
  int   TGUniqueId;                 /* our unique identifier for the group */
  int   NumTags;                    /* number of tags in our TagTable */
  int   TagTable[DYNINSTTagsLimit]; /* known tags for this group */
  DYNINSTresourceCreationInfo	tagCreateInfo;	/* record of recent tag creations */

  struct DynInstTag_st* Next;       /* next defined group info struct */
} DynInstTagSt;



typedef struct DynInstNewTagInfo_
{
	int tagId;                   /* id of new tag */
	int groupId;                 /* group tag belongs to */
	int isNewGroup;              /* is this the first time we saw this group? */
} DynInstNewTagInfo;


typedef struct {
  int            TagHierarchy; /* True if hierarchy, false single level */
  int            NumGroups;     /* Number of groups, tag arrays */
  /* Group table, each index pointing to an array of tags */
  DynInstTagSt*  GroupTable[DYNINSTTagGroupsLimit]; 

  int                NumNewTags;   /* number of entries in the NewTags array */
  DynInstNewTagInfo  NewTags[DYNINSTNewTagsLimit];
} DynInstTagGroupSt;




static DynInstTagGroupSt  TagGroupInfo;




/************************************************************************
 * DYNINSTtagGroupInfo_Init
 *
 * Initialize the singleton TagGroupInfo struct.
 *
 ************************************************************************/
void
DYNINSTtagGroupInfo_Init( void )
{
  unsigned int dx;

  TagGroupInfo.TagHierarchy = 0; /* FALSE; */
  TagGroupInfo.NumGroups = 0;
  TagGroupInfo.NumNewTags = 0;
  for(dx=0; dx < DYNINSTTagGroupsLimit; dx++) {
    TagGroupInfo.GroupTable[dx] = NULL;
  } 
}


/************************************************************************
 * void DYNINSTreportNewTags(void)
 *
 * Inform the paradyn daemons of new message tags and/or groups.
 *
************************************************************************/
void DYNINSTreportNewTags(void)
{
  int    dx;
  rawTime64 process_time;
  rawTime64 wall_time;

  if(TagGroupInfo.NumNewTags > 0) {
    /* not used by consumer [createProcess() in perfStream.C], so can prob.
     * be set to a dummy value to save a little time.  */
    process_time = DYNINSTgetCPUtime();
    /* this _is_ needed; paradynd keeps the 'creation' time of each resource
     *  (resource.h) */
    wall_time = DYNINSTgetWalltime();
  }
  
  for(dx=0; dx < TagGroupInfo.NumNewTags; dx++) {
    struct _newresource newRes;
    
    if((TagGroupInfo.TagHierarchy) && (TagGroupInfo.NewTags[dx].isNewGroup)) {
      memset(&newRes, '\0', sizeof(newRes));
      sprintf(newRes.name, "SyncObject/Message/%d",
	      TagGroupInfo.NewTags[dx].groupId);
      strcpy(newRes.abstraction, "BASE");
      newRes.mdlType = RES_TYPE_INT;
      newRes.btype = MessageGroupResourceType;

      DYNINSTgenerateTraceRecord(0, TR_NEW_RESOURCE,
				 sizeof(struct _newresource), &newRes, 1,
				 wall_time,process_time);
    }
    
    memset(&newRes, '\0', sizeof(newRes));
    if(TagGroupInfo.TagHierarchy) {
      sprintf(newRes.name, "SyncObject/Message/%d/%d", 
	      TagGroupInfo.NewTags[dx].groupId, TagGroupInfo.NewTags[dx].tagId);
      newRes.btype = MessageTagResourceType;
    } else {
      sprintf(newRes.name, "SyncObject/Message/%d", 
	      TagGroupInfo.NewTags[dx].tagId);
      newRes.btype = MessageGroupResourceType;
    }
    strcpy(newRes.abstraction, "BASE");
    newRes.mdlType = RES_TYPE_INT;

    DYNINSTgenerateTraceRecord(0, TR_NEW_RESOURCE, 
			       sizeof(struct _newresource), &newRes, 1,
			       wall_time,process_time);
  }
  TagGroupInfo.NumNewTags = 0;
}


/************************************************************************
 * DYNINSTrecordTagGroupInfo
 *
 * Handle a (possibly) new group and/or message tag.  This routine
 * checks whether we've seen the given group and tag before, and
 * if not, reports them to the Paradyn daemon.
 *
 * We use a sort of hash table to keep tags and groups, so this routine
 * involves looking up the table entry to see whether we've seen
 * the tag or group before.
 *
 * There are two issues to consider -
 *
 * 1. We limit the total number of tags and groups so that an application
 *    that uses many of them will not overwhelm Paradyn by reporting
 *    new tags or groups.
 *
 * 2. We throttle the reporting of tags and groups whenever their
 *    creation rate is too high, again to avoid overwhelming the
 *    rest of Paradyn with the creation of new resources.
 *
 ************************************************************************/
static unsigned int rateCheck = 0;

void DYNINSTrecordTagGroupInfo(int tagId, int groupId)
{
  DynInstTagSt* tagSt;
  int           dx;
  int           newGroup;
  int			tagDx = (tagId % DYNINSTTagsLimit);
  int			groupDx = (groupId % DYNINSTTagGroupsLimit);
  double		recentTagCreateRate;
  rawTime64		ts;
  

  /* 
   * check if we've reached the limit on the number of new tags 
   * to be reported at one time
   * (In the current implementation, this should never happen.  All
   * platforms support shared memory sampling, so we report new tags 
   * and groups at the end of this function whenever we are called.
   */
  if(TagGroupInfo.NumNewTags == DYNINSTNewTagsLimit) return;


  /*
   * Find the info about the group we were given
   */
  tagSt = TagGroupInfo.GroupTable[groupDx];
  while((tagSt != NULL) && (tagSt->TagGroupId != groupId)) {
    tagSt = tagSt->Next;
  }
  if(tagSt == NULL) {
    /* We have not seen this group before, so add a group info struct for it. */
    tagSt = (DynInstTagSt *)malloc(sizeof(DynInstTagSt));
    assert(tagSt != NULL);

    tagSt->TagGroupId = groupId;
    tagSt->TGUniqueId = DYNINSTTGroup_CreateUniqueId(groupId);
    tagSt->NumTags = 0;
    for(dx=0; dx < DYNINSTTagsLimit; dx++) {
      tagSt->TagTable[dx] = DYNINSTGroupUndefinedTag;
    }
	DYNINSTresourceCreationInfo_Init( &(tagSt->tagCreateInfo),
										DYNINSTtagCreateRateWindowLength );
    tagSt->Next = TagGroupInfo.GroupTable[groupDx];
    TagGroupInfo.GroupTable[groupDx] = tagSt;
    TagGroupInfo.NumGroups++;
    newGroup = 1;
  } else {
    /* We have seen this group before - nothing to do */
    assert(tagSt->TagGroupId == groupId);
    newGroup = 0;
  }


  /*
   * Check if we've reached the limit on the number of tags to track.
   */
  if(tagSt->NumTags == DYNINSTTagsLimit) return;


  /*
   * Find the info about the tag we were given
   */
  dx = tagDx;
  while((tagSt->TagTable[dx] != tagId) && (tagSt->TagTable[dx] != DYNINSTGroupUndefinedTag)) {
    dx++;
    if(dx == DYNINSTTagsLimit)
	{
       dx = 0;
    }
    assert(dx != tagId);
  }

  if(tagSt->TagTable[dx] == tagId)
  {
    /* We've already seen this tag - nothing to do */
	return;
  }


  /*
   * check if the program is using new tags too quickly...
   */

  /* ...record that a tag was discovered for this group at this time
   * (we chose to use wall time to compute creation rate because the
   * daemon and front end are separate processes from this one, and the
   * issue here is how quickly they are able to handle new resource
   * notifications)...
   */
  ts = DYNINSTgetWalltime();
  if( DYNINSTresourceCreationInfo_Add( &(tagSt->tagCreateInfo), ts ) == 0 )
  {
		/*
		 * we ran out of space in the creation info -
		 * most likely, the application is creating resources too
		 * quickly, so we will throttle
		 *
	 	 * don't report this tag to the daemon now (in fact,
	 	 * don't record that we ever saw this tag so that we will
		 * handle it correctly in case we see the tag in the future
		 * when the program is not coming up with new tags so quickly
		 */
		return;
  }

  /* ...determine the creation rate during this group's sliding window... */
  recentTagCreateRate = (((double)tagSt->tagCreateInfo.nCreatesInWindow) /
							tagSt->tagCreateInfo.windowLength);

  /* ...check whether the creation rate is too high */
  if( recentTagCreateRate > DYNINSTtagCreationRateLimit )
  {
    /* 
	 * the program is using new tags too quickly -
	 * don't report this tag to the daemon now (in fact,
	 * don't record that we ever saw this tag so that we will
	 * handle it correctly in case we see the tag in the future
	 * when the program is not coming up with new tags so quickly
	 */
	 return;
  }


  assert(tagSt->TagTable[dx] == DYNINSTGroupUndefinedTag);

  /* allocate and initialize a new tag info struct */
  tagSt->TagTable[dx] = tagId;
  tagSt->NumTags++;
  
  TagGroupInfo.NewTags[TagGroupInfo.NumNewTags].tagId = tagId;
  TagGroupInfo.NewTags[TagGroupInfo.NumNewTags].groupId = tagSt->TGUniqueId;
  TagGroupInfo.NewTags[TagGroupInfo.NumNewTags].isNewGroup = newGroup;
  TagGroupInfo.NumNewTags++;

  /* report new tag and/or group to our daemon */
  DYNINSTreportNewTags();
}



/************************************************************************
 * 
 * DYNINSTrecordTag
 * DYNINSTrecordTagAndGroup
 * DYNINSTrecordGroup
 *
 * Handle recording of (possibly) new tag and/or group.
 * Message-passing routines such as MPI_Send are instrumented
 * to call these functions.
 *
 ************************************************************************/

void DYNINSTrecordTag(int tagId)
{
  assert(tagId >= 0);
  assert(TagGroupInfo.TagHierarchy == 0); /* FALSE); */
  DYNINSTrecordTagGroupInfo(tagId, -1);
}

void DYNINSTrecordTagAndGroup(int tagId, int groupId)
{
  assert(tagId >= 0);
  assert(groupId >= 0);
  TagGroupInfo.TagHierarchy = 1; /* TRUE; */
  DYNINSTrecordTagGroupInfo(tagId, groupId );
}

void DYNINSTrecordGroup(int groupId)
{
  assert(groupId >= 0);
  TagGroupInfo.TagHierarchy = 1;  /* TRUE; */
  DYNINSTrecordTagGroupInfo(-1, groupId);
}

/************************************************************************
 * DYNINST test functions.
 *
 * Since these routines aren't used regularly, shouldn't they be surrounded
 * with an ifdef? --ari
************************************************************************/
#ifdef notused
void DYNINSTsimplePrint(void) {
    printf("inside dynamic inst function\n");
}

void DYNINSTentryPrint(int arg) {
    printf("enter %d\n", arg);
}

void DYNINSTcallFrom(int arg) {
    printf("call from %d\n", arg);
}

void DYNINSTcallReturn(int arg) {
    printf("return to %d\n", arg);
}

void DYNINSTexitPrint(int arg) {
    printf("exit %d\n", arg);
}
#endif

/************************************************************************
 *
 ************************************************************************/
int DYNINSTTGroup_CreateUniqueId(int commId)
{
#ifdef PARADYN_MPI
  MPI_Group commGroup;
  MPI_Group worldGroup;
  int       ranks1[1];
  int       ranks2[1];
  int       rc;

  /* assert(commId < (1<<16)); */
  assert(commId < 100000);
  
  rc = MPI_Comm_group(commId, &commGroup);
  assert(rc == 0);
  rc = MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
  assert(rc == 0);
  
  ranks1[0] = 0;
  rc = MPI_Group_translate_ranks(commGroup, 1, ranks1, worldGroup, ranks2);
  assert(rc == 0);

  assert(ranks2[0] != MPI_UNDEFINED);
  /* assert(ranks2[0] < (1 << 16));
     return(commId + (ranks2[0] * (1<<16)));
  */
  assert(ranks2[0] < 20000);
  return(commId + (ranks2[0] * 100000));
#else
  return(commId);
#endif
}



/************************************************************************
 *
 ************************************************************************/
int DYNINSTTGroup_CreateLocalId(int tgUniqueId)
{
#ifdef PARADYN_MPI
  /* return(tgUniqueId | ((1<<16)-1)); */
  return(tgUniqueId % 100000);
#else
  return(tgUniqueId);
#endif
}


/************************************************************************
 *
 ************************************************************************/
int DYNINSTTGroup_FindUniqueId(int groupId)
{
  int           groupDx;
  DynInstTagSt* tagSt;

  if ( groupId < 0 )
  {
    rtUIMsg traceData;
    sprintf(traceData.msgString,
            "Invalid group ID %d encountered.", groupId);
    traceData.errorNum = 116;
    traceData.msgType = rtWarning;
    DYNINSTgenerateTraceRecord(0, TR_ERROR, sizeof(traceData),&traceData,
                               1, 1, 1);

    return(-1);
  }
  
  groupDx = groupId % DYNINSTTagGroupsLimit;

  for(tagSt = TagGroupInfo.GroupTable[groupDx]; tagSt != NULL;
      tagSt = tagSt->Next)
  {
    if(tagSt->TagGroupId == groupId)
        return(tagSt->TGUniqueId);
  }
  
  return(-1);
}

int DYNINSTTwoComparesAndedExpr(int arg1, int arg2, int arg3, int arg4)
{
  return((arg1 == arg2) && (arg3 == arg4));
}

int DYNINSTCountElmsInArray(int* array, int num)
{
  int sum = 0;

  for(num--; num >= 0; num--) {
    sum += array[num];
  }
  return(sum);
}

int DYNINSTSub(int num1, int num2)
{
  return(num1-num2);
}

int DYNINSTAdd(int num1, int num2)
{
  return(num1+num2);
}

int DYNINSTMult(int num1, int num2)
{
  return(num1*num2);
}

int DYNINSTArrayField(int* array, int index)
{
  return(array[index]);
}

struct callee{
  unsigned address;
  struct callee *next;
};

struct call_site_info{
  unsigned address;
  struct callee *next_callee;
  struct call_site_info *next;
};

struct call_site_info *callSiteList = NULL;

void DYNINSTRegisterCallee(unsigned int calleeAddress,
			   unsigned callSiteAddress){
  if(DYNINSTCalleeSearch(callSiteAddress, calleeAddress) == 0){
    struct callercalleeStruct c;
    c.caller = callSiteAddress;
    c.callee = calleeAddress;
    DYNINSTgenerateTraceRecord(0, TR_DYNAMIC_CALLEE_FOUND,
			       sizeof(struct callercalleeStruct), 
			       &c, 1, DYNINSTgetWalltime(), 
			       DYNINSTgetCPUtime());
  }
}

/*Needs to return 1 if the call site has been seen, 0 otherwise*/
int DYNINSTCalleeSearch(unsigned int callSiteAddr,
			unsigned int calleeAddr){
  
  if(callSiteList == NULL){
    callSiteList = (struct call_site_info *) 
      malloc(sizeof(struct call_site_info));
    callSiteList->address = callSiteAddr;
    callSiteList->next = NULL;
    callSiteList->next_callee = (struct callee *) 
      malloc(sizeof(struct callee));
    callSiteList->next_callee->next = NULL;
    callSiteList->next_callee->address = calleeAddr;
    return 0;
  }
  else {
    struct call_site_info *curr = callSiteList;
    while(curr != NULL){
      if(curr->address == callSiteAddr)
        break;
      else curr = curr->next;
    }
    
    if(curr == NULL){
      /*need to create new call_site_info, add link to new callee_struct,
	and send message back to the daemon*/
      curr = callSiteList;
      while(curr->next != NULL)
	curr = curr->next;
      curr->next = (struct call_site_info *) 
	malloc(sizeof(struct call_site_info));
      curr = curr->next;
      curr->address = callSiteAddr;
      curr->next = NULL;
      curr->next_callee = (struct callee *) 
	malloc(sizeof(struct callee));
      curr->next_callee->next = NULL;
      curr->next_callee->address = calleeAddr;
      return 0;
    }
    else {
      /*in any event, curr should now point to the appropriate call
	site struct, so we should do a search in the callee list*/
      struct callee *curr_callee = curr->next_callee;
      while(curr_callee != NULL){
	if(curr_callee->address == calleeAddr){
	  return 1;
	}
	curr_callee = curr_callee->next;
      }
      
      curr_callee = (struct callee *)
	malloc(sizeof(struct callee));
      curr_callee->next = curr->next_callee;
      curr_callee->address = calleeAddr;
      curr->next_callee = curr_callee;
      return 0;
    }
  }
}

#define KNOWN_MPI_TYPES 34

/*
  This is an approximation to the real MPI_Pack_size routine,
  which may not be present in an MPI application
  Constants are given for MPICH-1.2.0 on x86/Linux
*/
int MPI_Pack_size (int incount, int datatype, int comm, int *size)
{
	const int type_size[KNOWN_MPI_TYPES+1] = {
		0, 1, 1, 1, 2, 2, 4, 4, 4, 4, 
		4, 8,12, 8, 1, 0, 0, 8,12, 8, 
		6, 8,16, 8,16, 4, 4, 8, 4, 8,
		16,32, 8,16};
	static int warned = 0;

	if (!warned) {
		fprintf(stderr, "WARNING: Bogus MPI_Pack_size: relink "
			"the application with \"-u MPI_Pack_size\" "
			"for accurate results\n");
		warned = 1;
	}
	if (datatype <= 0 || datatype > KNOWN_MPI_TYPES) {
		fprintf(stderr, "WARNING: Paradyn_Pack_size: unknown type\n");
		*size = 0;
		return (-1);
	}
	*size = incount * type_size[datatype] + 16;
	
	return 0;
}




