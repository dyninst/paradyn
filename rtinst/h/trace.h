/*
 * Define the trace format that is generated by the system.
 *
 * $Log: trace.h,v $
 * Revision 1.9  1994/07/11 22:29:10  jcargill
 * Got rid of old/unused trace-record types, and added a new one for cm5 node
 * paradynd printing
 *
 * Revision 1.8  1994/06/27  21:29:52  rbi
 * Abstraction-specific resources and mapping info
 *
 * Revision 1.7  1994/03/17  14:55:49  jcargill
 * Changed type of "Boolean" to be a char, to be consistent with X convention
 * and to stop a type conflict with the definition in util/h/list.h
 *
 * Revision 1.6  1994/01/27  20:33:39  hollings
 * Fix for paradynd
 *
 * Revision 1.5  1994/01/26  04:54:26  hollings
 * Change to <module>/h/<interface>.h
 *
 * Revision 1.4  1994/01/25  20:52:36  hollings
 * changed Boolean to keep xdr happy.
 *
 * Revision 1.3  1993/12/13  20:10:53  hollings
 * added ALIGN_TO_WORDSIZE macro
 *
 * Revision 1.2  1993/08/26  18:22:03  hollings
 * added EXIT and ACK.
 *
 * Revision 1.1  1993/07/01  17:03:52  hollings
 * Initial revision
 *
 *
 */

#ifndef _TRACE_H
#define _TRACE_H

#include "rtinst/h/rtinst.h"

#ifndef False
#define False 0
#define True 1
#endif

#ifndef WORDSIZE
#define WORDSIZE sizeof(int)
#endif

#define ALIGN_TO_WORDSIZE(x)    (((x) + (WORDSIZE-1)) & ~(WORDSIZE-1))

typedef char Boolean;

typedef float sampleValue;

#define CONTROLLER_FD       3
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
#define TR_FORK			1
#define TR_SAMPLE		2
#define TR_NEW_RESOURCE		3
#define TR_NEW_ASSOCIATION	4
#define TR_MULTI_FORK		5
#define TR_EXIT			6
#define TR_NODE_PRINT		7

struct _traceSample {
    sampleId	id;		/* id of the sample */
    sampleValue value;		/* sample value */
};

typedef struct _traceSample traceSample;

struct _traceFork {
    int	ppid;	/* id of creating thread */
    int	pid;	/* id of new thread */
    int npids;	/* number of new threads */
    int stride;	/* offset to next pid in multi */
};


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
};

typedef struct _traceFork traceFork;

struct _ptraceAck {
    int seqNumber;              /* sequence number of ptrace request */
    int bytes_read;             /* bytes which made up the ptrace request. */
                                /* We can extend the window by this much. */
};

typedef struct _ptraceAck ptraceAck;

#endif
