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


#include "RTthread.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RTcompat.h"
#include "dyninstAPI_RT/h/dyninstRTExport.h"

void DEBUG_VIRTUAL_TIMER_START(virtualTimer *timer, int context) {
  switch(context) {
  case THREAD_UPDATE: {
    fprintf(stderr, "THREAD_UPDATE--->start virtual timer(%d), lwp_id=%d\n", timer-(virtualTimers), timer->lwp);
    break ;
  }
  case THREAD_CREATE: {
    fprintf(stderr, "THREAD_CREATE--->start virtual timer(%d), lwp_id=%d\n", timer-(virtualTimers), timer->lwp);
    break ;
  }
  case THREAD_DETECT: {
    fprintf(stderr, "THREAD_DETECT--->start virtual timer(%d), lwp_id=%d\n", timer-(virtualTimers), timer->lwp);
    break ;
  }
  case VIRTUAL_TIMER_CREATE: {
    fprintf(stderr, "VIRTUAL_TIMER_CREATE--->start virtual timer(%d), lwp_id=%d\n", timer-(virtualTimers), timer->lwp);
    break ;
  }
  case VIRTUAL_TIMER_START: {
    fprintf(stderr, "VIRTUAL_TIMER_START--->start virtual timer(%d), lwp_id=%d\n", timer-(virtualTimers), timer->lwp);
    break ;
  }
  case THREAD_TIMER_START: {
    fprintf(stderr, "THREAD_TIMER_START--->start virtual timer(%d), lwp_id=%d\n", timer-(virtualTimers), timer->lwp);
    break ;
  }
  }
}

/* Start the virtual timer running on the known lwp */

void _VirtualTimerStart(virtualTimer *timer, int context){
  assert(timer->protector1 == timer->protector2);
  timer->protector1++;

  if (timer->counter ==0) {
      timer->lwp = P_lwp_self();
      timer->rt_fd = PARADYNgetFD(timer->lwp);
      timer->start = DYNINSTgetCPUtime_LWP(timer->lwp, timer->rt_fd);
      timer->rt_previous = timer->start;
  }
  timer->counter++;
  
  timer->protector2++;
  
  //fprintf(stderr, "vtimer_start at addr 0x%x, lwp is %d\n", timer,
  //        timer->lwp);

  assert(timer->protector1 == timer->protector2);
}

void _VirtualTimerStop(virtualTimer* timer) {
  if (!timer) { return; }
  
  assert(timer->protector1 == timer->protector2);
  timer->protector1++;
  if (timer->counter == 1) {
    rawTime64 now;
    now = DYNINSTgetCPUtime_LWP(timer->lwp, timer->rt_fd);
    if (now < timer->start) {
      ;
    }
    else
      timer->total += (now - timer->start);
    timer->lwp = 0;
#if defined( os_linux )
	/* Not sure why Solaris doesn't care about this... */
	if( timer->rt_fd != -1 && timer->rt_fd != 0 ) {
		// /* DEBUG */ fprintf( stderr, "%s[%d]: closing timer fd %d on virtual timer stop.\n", __FILE__, __LINE__, timer->rt_fd );
		close( timer->rt_fd );
		}
#endif
    timer->rt_fd = 0;
  }
  timer->counter--;
  timer->protector2++;
  assert(timer->protector1 == timer->protector2);
  //fprintf(stderr, "vtimer_stop at addr 0x%x, lwp is %d\n", timer,
  //        timer->lwp);
}

void _VirtualTimerFinalize(virtualTimer *vt) {
#if !defined(os_windows)
    if (vt->rt_fd) close (vt->rt_fd);
#endif
}

/* called when the virtual timer is reused by another thread */
void _VirtualTimerDestroy(virtualTimer* vt) {
  if (vt->lwp) {
    _VirtualTimerFinalize(vt);
    memset((char*) vt, '\0', sizeof(virtualTimer)) ;
  }
}

/* getThreadCPUTime */
/* We're basically "sampling" the per-lwp virtual timer. So
   use the works: check p1 == p2, check rollbacks, the lot. */
/* Hrm... since only one thread gets to access a timer, why are we bothering?
   We have serialized access to these suckers */
rawTime64 getThreadCPUTime(unsigned index, int *valid) {
  volatile int protector1, protector2;
  volatile rawTime64 total, start ;
  rawTime64 now = -1;
  volatile int    count, vt_lwp_id;
  virtualTimer *vt = &(virtualTimers[index]);

  protector2 = vt->protector2 ;
  count = vt->counter;
  total = vt->total;
  vt_lwp_id = vt->lwp;
  start = vt->start ;
  protector1 = vt->protector1 ;
  
  if (protector1 != protector2) {
    *valid = 0 ; /* not a valid value */
    return 0 ;
  }
  
  *valid = 1 ;
  if (count > 0) {
#if defined(os_solaris) || defined(os_linux)
    /* Might not have set the file descriptor */
    if (!vt->rt_fd)
      vt->rt_fd = PARADYNgetFD(vt->lwp);
#endif
    now = DYNINSTgetCPUtime_LWP(vt->lwp, vt->rt_fd);
    
    /* Check against rollback value */
    if (now < vt->rt_previous) {
      now = vt->rt_previous;
    }
    else {
       vt->rt_previous = now;
    }

    if (now >= start) {
      return total + (now-start) ;
    } else  {
      return total ;
    }
  } else {
    return total ;
  }
}

#define PRINTOUT_TIMER(l, s, t)   fprintf(stderr, "%d %s Timer (%x): total %lld, start %lld, counter %d, index %u, p1 %d, p2 %d\n", l, s, (unsigned) t, t->total, t->start, t->counter, t->index, t->protector1, t->protector2);


/*
  //THREAD TIMER   
*/
void DYNINSTstartThreadTimer(tTimer* timer)
{
   rawTime64 start;
   int valid = 0;  
   int i;
   
   unsigned index = dyninst_threadIndex();

   if(!timer)
      i = *((int *)0); /* abort() sometimes doesn't leave good stack traces */
   
   assert(timer->protector1 == timer->protector2);
   timer->protector1++;
   MEMORY_BARRIER;
   if (timer->counter == 0) {
      if (!(timer->index)) { /* No INDEX associated with this timer yet */
         /* INDEX could be set in daemon, which would make this much easier */
         timer->index = index;
         /* fprintf(stderr, "Setting timer INDEX to %d, tid %d\n", timer->index, 
                    P_thread_self()); */
      }
      /* We sample the virtual timer, so we may need to retry */
      while (!valid) {
         start = getThreadCPUTime(timer->index, &valid);
      }
      timer->start = start;
   }
   timer->counter++;
   MEMORY_BARRIER;
   timer->protector2++;

   //   virtualTimer *vt = &(virtualTimers[index]);
   //   PRINTOUT_TIMER(vt->lwp, "start", timer);

   assert(timer->protector1 == timer->protector2);
}

void DYNINSTstopThreadTimer(tTimer* timer)
{
    int i;

    unsigned index = dyninst_threadIndex();
   
    if (!timer)
        i = *((int *)0);
    
    assert(timer->protector1 == timer->protector2);
    timer->protector1++;
    MEMORY_BARRIER;
    if (timer->counter == 1) {
      int valid = 0;
      rawTime64 now;

      while (!valid) {
         now = getThreadCPUTime(timer->index, &valid);
      }
      if (now < timer->start) {
         assert(0 && "Rollback in DYNINSTstopThreadTimer");
      }
      timer->total += (now - timer->start);
   }
   
    if (timer->counter > 0) timer->counter--; 
   MEMORY_BARRIER;
   timer->protector2++;

   //virtualTimer *vt = &(virtualTimers[index]);
   //PRINTOUT_TIMER(vt->lwp, "stop", timer);
   assert(timer->protector1 == timer->protector2);
}

