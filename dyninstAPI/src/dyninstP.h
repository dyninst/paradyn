/*
 *  Copyright 1993 Jeff Hollingsworth.  All rights reserved.
 *
 */

/*
 * private structures used by the implementation of the instrumentation 
 *   interface.  modules that use the instrumentation interface should not
 *   include this file.
 *
 * $Log: dyninstP.h,v $
 * Revision 1.1  1994/01/27 20:31:18  hollings
 * Iinital version of paradynd speaking dynRPC igend protocol.
 *
 * Revision 1.6  1993/08/23  23:14:58  hollings
 * removed unused pid field.
 *
 * Revision 1.5  1993/07/13  18:27:00  hollings
 * new include file syntax.
 *
 * Revision 1.4  1993/06/22  19:00:01  hollings
 * global inst state.
 *
 * Revision 1.3  1993/06/08  20:14:34  hollings
 * state prior to bc net ptrace replacement.
 *
 * Revision 1.2  1993/04/27  14:39:21  hollings
 * signal forwarding and args tramp.
 *
 * Revision 1.1  1993/03/19  22:51:05  hollings
 * Initial revision
 *
 *
 */
#include "util/h/list.h"
#include "dyninst.h"

typedef enum { selfTermination, controlTermination } executableType;

class executableRec {
    public:
	char *name;
	char *machine;
	char *user;
	int argc;
	char **argv;
	executableType type;
	processState state;
	executableRec *next;
	FILE *controlPath;
	process *proc;	/* for directly connected processes */
};

class _perfStreamRec {
    public:
	dataType		type;
	dataCallback		dataFunc;
	controlCallback		controlFunc;
	List<metricInstance>	enabledMetrics;
};

class _applicationRec {
    public:
	executableRec 	*programs;
	errorHandler 		errorFunc;
	List<performanceStream>	streams;
};

typedef List<performanceStream> perfStreamList;

class _resourceRec {
    public:
	_resourceRec(Boolean full = True) {
	    if (full) {
		parent = NULL;
		handle = NULL;
		children = NULL;
		info.name = "";
		info.fullName = "";
		info.creation = 0.0;
	    }
	};
	resource parent;		/* parent of this resource */
	void *handle;		/* handle to resource specific data */
	resourceList children;	/* children of this resource */
	resourceInfo info;
	perfStreamList notifyList;	/* who to inform of creations */
};

class _resourceListRec {
    public:
	resource *elements;		/* actual data in list */
	int count;			/* number of items in the list */
	int maxItems;		/* limit of current array */
};

class _metricListRec {
    public:
	metric elements;	/* actual data in list */
	int count;		/* number of items in the list */
	int maxItems;	/* limit of current array */
};

void invokeSampleCallback(metricInstance mi, timeStamp start, 
    timeStamp end, sampleValue value);

Boolean isApplicationPaused();
