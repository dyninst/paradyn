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





/************************************************************************
 * header files.
************************************************************************/
#include <signal.h>
#include <sys/ucontext.h>
#include <sys/time.h>
#include "rtinst/h/rtinst.h"

/*extern int    gettimeofday(struct timeval *, struct timezone *);*/
extern void perror(const char *);




/************************************************************************
 * symbolic constants.
************************************************************************/

static const double NANO_PER_USEC = 1.0e3;
static const double MILLION       = 1.0e6;





/************************************************************************
 * void DYNINSTos_init(void)
 *
 * os initialization function---currently null.
************************************************************************/

void
DYNINSTos_init(void) {

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
	abort();
    }
#endif


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

time64
DYNINSTgetCPUtime(void) {
  static time64 previous=0;

  while (1) {
     time64 now = (time64)gethrvtime()/(time64)1000;
     if (now < previous) continue;

     previous = now;
     return (now);
  }
}





/************************************************************************
 * time64 DYNINSTgetWalltime(void)
 *
 * get the total walltime used by the monitored process.
 * return value is in usec units.
************************************************************************/

time64
DYNINSTgetWalltime(void) {
  static time64 previous=0;
  time64 now;

  while (1) {
    struct timeval tv;
    if (gettimeofday(&tv,NULL) == -1) {
        perror("gettimeofday");
        abort();
    }
    now = (time64)tv.tv_sec*(time64)1000000 + (time64)tv.tv_usec;
    if (now < previous) continue;
    previous = now;
    return(now);
  }
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
    for (u = HASH1(key); 1; u += HASH2(key) % TRAMPTABLESZ) {
      k = DYNINSTtrampTable[u].key;
      if (k == 0)
        return 0;
      else if (k == key)
        return DYNINSTtrampTable[u].val;
    }
    /* not reached */
    abort();
}

void DYNINSTtrapHandler(int sig, siginfo_t *info, ucontext_t *uap) {
    unsigned pc = uap->uc_mcontext.gregs[PC];
    unsigned nextpc = lookup(pc);
    if (nextpc) {
      uap->uc_mcontext.gregs[PC] = nextpc;
    } else {
      abort();
    }
    DYNINSTtotalTraps++;
}
#endif
