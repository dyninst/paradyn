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

/************************************************************************
 * RTsolaris.c: clock access functions for solaris-2.
************************************************************************/

#include <signal.h>
#include <sys/ucontext.h>
#include <sys/time.h>
#include <assert.h>
#include <sys/syscall.h>

#include <sys/procfs.h> /* /proc PIOCUSAGE */
#include <stdio.h>
#include <fcntl.h> /* O_RDONLY */
#include <unistd.h> /* getpid() */
#include <limits.h>  /* for INT_MAX */

#include "rtinst/h/rtinst.h"
#include "rtinst/h/trace.h"

extern void perror(const char *);




/************************************************************************
 * symbolic constants.
************************************************************************/

static const double NANO_PER_USEC = 1.0e3;
static const double MILLION       = 1.0e6;




static int procfd = -1;

void DYNINSTgetCPUtimeInitialize(void) {
   /* This stuff is done just once */
   char str[20];

   sprintf(str, "/proc/%d", (int)getpid());
   /* have to use syscall here for applications that have their own
      versions of open, poll...In these cases there is no guarentee that
      things have been initialized so that the application's version of
      open can be used when this open call occurs (in DYNINSTinit)
   */
   procfd = syscall(SYS_open,str, O_RDONLY);
   if (procfd < 0) {
      fprintf(stderr, "open of /proc failed in DYNINSTgetCPUtimeInitialize\n");
      perror("open");
      abort();
   }
}


/************************************************************************
 * void DYNINSTos_init(void)
 *
 * os initialization function---currently null.
************************************************************************/

void
DYNINSTos_init(int calledByFork, int calledByAttach) {

    /*
       Install trap handler.
       This is currently being used only on the x86 platform.
    */
#ifdef i386_unknown_solaris2_5
    void DYNINSTtrapHandler(int sig, siginfo_t *info, ucontext_t *uap);
    struct sigaction act;
    act.sa_handler = DYNINSTtrapHandler;
    act.sa_flags = 0;
    sigfillset(&act.sa_mask);
    if (sigaction(SIGTRAP, &act, 0) != 0) {
        perror("sigaction(SIGTRAP)");
	assert(0);
	abort();
    }
#endif

    /* It is necessary to call DYNINSTgetCPUtimeInitialize here to make sure
       that it is called again for a child process during a fork - naim */
    DYNINSTgetCPUtimeInitialize();
}





/************************************************************************
 * time64 DYNINSTgetCPUtime(void)
 *
 * get the total CPU time used for "an" LWP of the monitored process.
 * this functions needs to be rewritten if a per-thread CPU time is
 * required.  time for a specific LWP can be obtained via the "/proc"
 * filesystem.
 * return value is in usec units.
 *
 * XXXX - This should really return time in native units and use normalize.
 *	conversion to float and division are way too expensive to
 *	do everytime we want to read a clock (slows this down 2x) -
 *	jkh 3/9/95
************************************************************************/


static unsigned long long div1000(unsigned long long in) {
  /* WARNING - Don't use if input is greater than 32 bits */

   /* Divides by 1000 without an integer division instruction or library call, both of
    * which are slow.
    * We do only shifts, adds, and subtracts.
    *
    * We divide by 1000 in this way:
    * multiply by 1/1000, or multiply by (1/1000)*2^30 and then right-shift by 30.
    * So what is 1/1000 * 2^30?
    * It is 1,073,742.   (actually this is rounded)
    * So we can multiply by 1,073,742 and then right-shift by 30 (neat, eh?)
    *
    * Now for multiplying by 1,073,742...
    * 1,073,742 = (1,048,576 + 16384 + 8192 + 512 + 64 + 8 + 4 + 2)
    * or, slightly optimized:
    * = (1,048,576 + 16384 + 8192 + 512 + 64 + 16 - 2)
    * for a total of 8 shifts and 6 add/subs, or 14 operations.
    *
    */

   unsigned long long temp = in << 20; /* multiply by 1,048,576 */
   /* beware of overflow; left shift by 20 is quite a lot.
      If you know that the input fits in 32 bits (4 billion) then
      no problem.  But if it's much bigger then start worrying...
   */

   temp += in << 14; /* 16384 */
   temp += in << 13; /* 8192  */
   temp += in << 9;  /* 512   */
   temp += in << 6;  /* 64    */
   temp += in << 4;  /* 16    */
   temp -= in >> 2;  /* 2     */

   return (temp >> 30); /* divide by 2^30 */
}

static unsigned long long divMillion(unsigned long long in) {
   /* Divides by 1,000,000 without an integer division instruction or library call,
    * both of which are slow.
    * We do only shifts, adds, and subtracts.
    *
    * We divide by 1,000,000 in this way:
    * multiply by 1/1,000,000, or multiply by (1/1,000,000)*2^30 and then right-shift
    * by 30.  So what is 1/1,000,000 * 2^30?
    * It is 1,074.   (actually this is rounded)
    * So we can multiply by 1,074 and then right-shift by 30 (neat, eh?)
    *
    * Now for multiplying by 1,074
    * 1,074 = (1024 + 32 + 16 + 2)
    * for a total of 4 shifts and 4 add/subs, or 8 operations.
    *
    * Note: compare with div1000 -- it's cheaper to divide by a million than
    *       by a thousand (!)
    *
    */

   unsigned long long temp = in << 10; /* multiply by 1024 */
   /* beware of overflow...if the input arg uses more than 52 bits
      than start worrying about whether (in << 10) plus the smaller additions
      we're gonna do next will fit in 64...
   */

   temp += in << 5; /* 32 */
   temp += in << 4; /* 16 */
   temp += in << 1; /* 2  */

   return (temp >> 30); /* divide by 2^30 */
}

static unsigned long long mulMillion(unsigned long long in) {
   unsigned long long result = in;

   /* multiply by 125 by multiplying by 128 and subtracting 3x */
   result = (result << 7) - result - result - result;

   /* multiply by 125 again, for a total of 15625x */
   result = (result << 7) - result - result - result;

   /* multiply by 64, for a total of 1,000,000x */
   result <<= 6;

   /* cost was: 3 shifts and 6 subtracts
    * cost of calling mul1000(mul1000()) would be: 6 shifts and 4 subtracts
    *
    * Another algorithm is to multiply by 2^6 and then 5^6.
    * The former is super-cheap (one shift); the latter is more expensive.
    * 5^6 = 15625 = 16384 - 512 - 256 + 8 + 1
    * so multiplying by 5^6 means 4 shift operations and 4 add/sub ops
    * so multiplying by 1000000 means 5 shift operations and 4 add/sub ops.
    * That may or may not be cheaper than what we're doing (3 shifts; 6 subtracts);
    * I'm not sure.  --ari
    */

   return result;
}

time64
DYNINSTgetCPUtime_LWP(int lwp_id) {
/* 
 * gethrvtime()/1000 does work right shm sampling because it
 * returns values that are in sync with /proc's PIOCUSAGE (pr_utime field
 * only), so when a fudge factor needs to be added by paradynd's shm sampling
 * of an active timer, things work ok. 
 *
 */
  hrtime_t lwpTime;
  time64 now = 0;
  prusage_t theUsage;
  int lwp_fd;
  if (lwp_id != -1) {
    lwp_fd = ioctl(procfd, PIOCOPENLWP, &(lwp_id));
    if (lwp_fd != -1) {
      if (ioctl(lwp_fd, PIOCUSAGE, &theUsage) == -1) {
	assert(0);
      }
      now = mulMillion(theUsage.pr_utime.tv_sec); /* sec to usec */
      now += div1000(theUsage.pr_utime.tv_nsec);  /* nsec to usec */
    }
  } else {
    lwpTime = gethrvtime();
    now = div1000(lwpTime);  /* nsec to usec */
  }
  return(now);  
}

/*static int MaxRollbackReport = 0; /* don't report any rollbacks! */
/*static int MaxRollbackReport = 1; /* only report 1st rollback */
static int MaxRollbackReport = INT_MAX; /* report all rollbacks */

time64
DYNINSTgetCPUtime(void) {
  static time64 cpuPrevious=0;
  static int cpuRollbackOccurred = 0;
  time64 now;

  now = gethrvtime() / 1000;

  if (now < cpuPrevious) {
    if(cpuRollbackOccurred < MaxRollbackReport) {
      rtUIMsg traceData;
      sprintf(traceData.msgString, "CPU time rollback with current time:"
          " %lld usecs, using previous value %lld usecs.",now,cpuPrevious);
      traceData.errorNum = 112;
      traceData.msgType = rtWarning;
      DYNINSTgenerateTraceRecord(0, TR_ERROR, sizeof(traceData), &traceData, 1,
				 1, 1);
    }
    cpuRollbackOccurred++;
    now = cpuPrevious;
  }
  else  cpuPrevious = now;

  return now;  
}





/************************************************************************
 * time64 DYNINSTgetWalltime(void)
 *
 * get the total walltime used by the monitored process.
 * return value is in usec units.
************************************************************************/

time64
DYNINSTgetWalltime(void) {
  static time64 wallPrevious=0;
  static int wallRollbackOccurred = 0;
  time64 now;

  now = gethrtime() / 1000;

  if (now < wallPrevious) {
    if(wallRollbackOccurred < MaxRollbackReport) {
      rtUIMsg traceData;
      sprintf(traceData.msgString, "Wall time rollback with current time:"
          " %lld usecs, using previous value %lld usecs.",now,wallPrevious);
      traceData.errorNum = 112;
      traceData.msgType = rtWarning;
      DYNINSTgenerateTraceRecord(0, TR_ERROR, sizeof(traceData), &traceData, 1,
				 1, 1);
    }
    wallRollbackOccurred++;
    now = wallPrevious;
  }
  else  wallPrevious = now;

  return now;
}

/****************************************************************************
   The trap handler. Currently being used only on x86 platform.

   Traps are used when we can't insert a jump at a point. The trap
   handler looks up the address of the base tramp for the point that
   uses the trap, and set the pc to this base tramp.
   The paradynd is responsible for updating the tramp table when it
   inserts instrumentation.
*****************************************************************************/

#ifdef i386_unknown_solaris2_5
trampTableEntry DYNINSTtrampTable[TRAMPTABLESZ];
unsigned DYNINSTtotalTraps = 0;

static unsigned lookup(unsigned key) {
    unsigned u;
    unsigned k;
    for (u = HASH1(key); 1; u = (u + HASH2(key)) % TRAMPTABLESZ) {
      k = DYNINSTtrampTable[u].key;
      if (k == 0)
        return 0;
      else if (k == key)
        return DYNINSTtrampTable[u].val;
    }
    /* not reached */
}

void DYNINSTtrapHandler(int sig, siginfo_t *info, ucontext_t *uap) {
    unsigned pc = uap->uc_mcontext.gregs[PC];
    unsigned nextpc = lookup(pc);

    if (!nextpc) {
      /* kludge: maybe the PC was not automatically adjusted after the trap */
      /* this happens for a forked process */
      pc--;
      nextpc = lookup(pc);
    }

    if (nextpc) {
      uap->uc_mcontext.gregs[PC] = nextpc;
    } else {
      assert(0);
      abort();
    }
    DYNINSTtotalTraps++;
}
#endif



