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

/*
 * $Id: rtinst.h,v 1.38 1999/07/13 04:32:15 csserra Exp $
 * This file contains the standard insrumentation functions that are provied
 *   by the instrumentation layer.
 *
 */

#ifndef _RTINST_H
#define _RTINST_H

/* We sometimes include this into assembly files, so guard the struct defs. */
#if !defined(__ASSEMBLER__)

/* If we must make up a boolean type, we should make it unique */
#define RT_Boolean unsigned char
#define RT_TRUE 1
#define RT_FALSE 0

/*typedef void (*instFunc)(void *cdata, int type, char *eventData);*/

/* parameters to a instremented function */
typedef enum { processTime, wallTime } timerType;

/* explicitly sized types */
#if defined(i386_unknown_nt4_0)
typedef __int64       time64;
typedef __int64       int64;
typedef __int32       int32;
#elif defined(alpha_dec_osf4_0)
typedef long int      time64;
typedef long int      int64;
typedef int           int32;
#else
typedef long long int time64;
typedef long long int int64;
typedef int           int32;
#endif

/* see "Address" in util/h/Types.h */
typedef unsigned long int Address;

struct sampleIdRec {
    unsigned int id;
    /* formerly an aggregate bit, but that's now obsolete */
};
typedef struct sampleIdRec sampleId;

struct endStatsRec {
    int alarms;
    int numReported;
    float instCycles;
    float instTime;
    float handlerCost;
    float totalCpuTime;
    int samplesReported;
    float samplingRate;
    float totalWallTime;
    int userTicks;
    int instTicks;
#if defined(i386_unknown_linux2_0)
	int totalTraps;
#endif
};



struct intCounterRec {
   int value;		/* this field must be first for setValue to work -jkh */
   sampleId id;
    
   unsigned char theSpinner;
   /* mutex serving 2 purposes: (1) so paradynd won't sample while we're in middle of
      updating and (2) so multiple LWPs or threads won't update at the same time */ 
};
typedef struct intCounterRec intCounter;

struct floatCounterRec {
    float value;
    sampleId id;
};
typedef struct floatCounterRec floatCounter;

#ifdef SHM_SAMPLING
struct tTimerRec {
   volatile time64 total;
   volatile time64 start;
   volatile int counter;
   volatile sampleId id; /* can be made obsolete in the near future */
#if defined(MT_THREAD)
   volatile int lwp_id;  /* we need to save the lwp id so paradynd can get in sync */
   volatile int in_inferiorRPC; /* flag to avoid time going backwards - naim */
   volatile struct tTimerRec  *vtimer; /* position in the threadTable */
#endif

   /* the following 2 vrbles are used to implement consistent sampling.
      Updating by rtinst works as follows: bump protector1, do action, then
      bump protector2.  Shared-memory sampling by paradynd works as follows:
      read protector2, read the 3 vrbles above, read protector1.  If
      the 2 protector values differ then try again, else the sample got
      a good snapshot.  Don't forget to be sure paradynd reads the protector
      vrbles in the _opposite_ order that rtinst writes them!!! */
   volatile int protector1;
   volatile int protector2;
};
#else
struct tTimerRec {
    volatile int 	counter;	/* must be 0 to start; must be 1 to stop */
    volatile time64	total;
    volatile time64	start;
    volatile time64     lastValue;      /* to check for rollback */
    volatile time64	snapShot;	/* used to get consistant value 
					   during st/stp */
    volatile int	normalize;	/* value to divide total by to 
					   get seconds */
                                        /* always seems to be MILLION; can we get rid
                                           of this? --ari */
    volatile timerType 	type;
    volatile sampleId 	id;
    volatile char mutex;
    /*volatile char sampled;*/

    /*volatile unsigned char theSpinner;*/
   /* mutex serving 2 purposes: (1) so paradynd won't sample while we're in middle of
      updating and (2) so multiple LWPs or threads won't update at the same time */ 
};
#endif
typedef struct tTimerRec tTimer;

typedef int traceStream;

void DYNINSTgenerateTraceRecord(traceStream sid, short type, 
			        short length,
                                void *eventData, int flush,
			        time64 wall_time,time64 process_time);
extern time64 DYNINSTgetCPUtime(void);
extern time64 DYNINSTgetWalltime(void);

#if defined(MT_THREAD)
extern time64 DYNINSTgetCPUtime_LWP(int lwp_id);
#include "rtinstMT/src/RTthread.h"
#endif

/*
   The tramp table is used when we need to insert traps in instrumentation
   points. It is used by the trap handler to lookup the base tramp for
   an address (point).

   The table is updated by the paradyn daemon.
*/

#define TRAMPTABLESZ (4096)

#define HASH1(x) ((x) % TRAMPTABLESZ)
#define HASH2(x) (((x) % TRAMPTABLESZ-1) | 1)

typedef struct trampTableEntryStruct trampTableEntry;
struct trampTableEntryStruct {
  unsigned key;
  unsigned val;
};

#endif

/*
 * Define the size of the per process data area.
 *
 *  This should be a power of two to reduce paging and chacing shifts.
 */

/* The only possible problem with 1024*1024 instead of 1024*256 is that
 * HP needs to handle longjumps in mini-trampolines...sparc doesn't have
 * this problem until the size gets much bigger...
 */

#define SYN_INST_BUF_SIZE	1024*1024*4

#endif
