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
 * Define the trace format that is generated by the system.
 *
 */

#ifndef _TRACE_H
#define _TRACE_H

#include "rtinst/h/rtinst.h"
#include "util/h/sys.h"
#include <sys/types.h> /* key_t */

#ifndef False
#define False 0
#define True 1
#endif

/*
 * Need to make this double word since we send long longs now and then
 *
 */
#define WORDSIZE sizeof(long)

#define ALIGN_TO_WORDSIZE(x)    (((x) + (WORDSIZE-1)) & ~(WORDSIZE-1))

/* 
 * configuration notification stream id.
 *
 */
#define CONFIG_NOTIFY_STREAM	1

/* header that is at the front of every trace record. */
struct _traceHeader {
    time64	wall;		/* wall time of the event */
    time64	process;	/* process time of the event */
    short	type;		/* event type */
    short	length;		/* length of event specifc data */
};
typedef struct _traceHeader traceHeader;

/* 
 * now define each of the event specific data types.
 *
 */

/*
 * when a new process/thread is created.
 *
 */

#ifndef SHM_SAMPLING
#define TR_SAMPLE		2
#endif

#define TR_NEW_RESOURCE		3
#define TR_NEW_ASSOCIATION	4
#define TR_EXIT			6
#define TR_COST_UPDATE		9
#define TR_CP_SAMPLE		10 /* critical path */
#define TR_EXEC_FAILED          12

#if defined(SHM_SAMPLING) && defined(MT_THREAD)
#define TR_THREAD               13
#define TR_THRSELF              14
#endif


#define TR_NEW_MEMORY           15
/* trace data streams */
#define TR_DATA                 16
#define TR_SYNC                 20

/* types for resources that may be reported */
#define RES_TYPE_INT    0
#define RES_TYPE_STRING 1

#if defined(SHM_SAMPLING) && defined(MT_THREAD)
struct _traceThread {
    int	ppid;	/* id of creating thread */
    int	tid;	/* id of new thread */
    int pos;    /* position of the new thread in the thread table */
    int ntids;	/* number of new threads */
    int stride;	/* offset to next pid in multi */
};

typedef struct _traceThread traceThread;

struct _traceThrSelf {
    int	ppid;	/* id of creating thread */
    int	tid;	/* id of new thread */
    int pos;    /* position in the thread table */
};

typedef struct _traceThrSelf traceThrSelf;

#endif

#ifndef SHM_SAMPLING
struct _traceSample {
    sampleId	id;		/* id of the sample */
    sampleValue value;		/* sample value */
};
typedef struct _traceSample traceSample;
#endif

/* Shared-memory resource for Blizzard */
struct _traceMemory {
    char        name[32];               /* name of the variable*/
    int         va ;                    /* va                  */
    unsigned    memSize ;               /* size of this piece  */
    unsigned    blkSize ;               /* block size          */
    /* type of leaf is integer */
} ;
typedef struct _traceMemory traceMemory ;


/* a map from one name space to another.  
 *   For example 
 *        type = "fd-fileName";
 *	  key = "3";
 *        value = "/tmp/bigFile";
 *
 * defines the maping from file descriptior 3 to its file (/tmp/bigFile).
 *
 * This is used to generalize the association of values with resources.
 *
 */
struct _association {
    char        abstraction[64];
    char	type[64];
    char	key[255];
    char	value[64];
};

struct _newresource {
    char	name[255];		/* full path name of resource */
    char	abstraction[64];        /* name of abstraction */
    unsigned short type;                /* type of leaf resource */
};


struct _costUpdate {
/*    int 	slotsExecuted;	(number of base tramps halves called.) */
/*    float	pauseTime;	(total pause time this interval) */
    float 	obsCostIdeal;	/* ideal memory model time */

    /* the following fields are used if profil()'ing: */
    float 	obsCostLow;	/* using profil */
    float 	obsCostHigh;	/* combined */
};
typedef struct _costUpdate costUpdate;

/* XXX - should this be a general vector?? (jkh 1/19/95) */
struct _cpSample {
    int   id;
    float length;
    float share;
};
typedef struct _cpSample cpSample;

struct DYNINST_bootstrapStruct {
   int event; /* 0 --> nothing; 1 --> end of DYNINSTinit (normal);
		 2 --> end of DYNINSTinit (forked process);
		 3 --> start of DYNINSTexec (before exec has actually happened) */
   int pid;
   int ppid; /* parent of forked process */

#ifdef SHM_SAMPLING
   void *appl_attachedAtPtr;
#endif

   char path[512]; /* only used in exec */
};

#endif
