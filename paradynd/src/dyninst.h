/*
 *  Copyright 1993 Jeff Hollingsworth.  All rights reserved.
 *
 */

/*
 * dyninst.h - exported interface to instrumentation.
 *
 * $Log: dyninst.h,v $
 * Revision 1.9  1994/06/27 21:28:08  rbi
 * Abstraction-specific resources and mapping info
 *
 * Revision 1.8  1994/05/18  00:52:26  hollings
 * added ability to gather IO from application processes and forward it to
 * the paradyn proces.
 *
 * Revision 1.7  1994/05/16  22:31:49  hollings
 * added way to request unique resource name.
 *
 * Revision 1.6  1994/04/09  18:34:52  hollings
 * Changed {pause,continue}Application to {pause,continue}AllProceses, and
 * made the RPC interfaces use these.  This makes the computation of pause
 * Time correct.
 *
 * Revision 1.5  1994/03/31  01:48:32  markc
 * Added default args to addProcess definition.
 *
 * Revision 1.4  1994/03/24  16:41:58  hollings
 * Moved sample aggregation to lib/util (so paradyn could use it).
 *
 * Revision 1.3  1994/03/20  01:53:04  markc
 * Added a buffer to each process structure to allow for multiple writers on the
 * traceStream.  Replaced old inst-pvm.C.  Changed addProcess to return type
 * int.
 *
 * Revision 1.2  1994/02/01  18:46:50  hollings
 * Changes for adding perfConsult thread.
 *
 * Revision 1.1  1994/01/27  20:31:17  hollings
 * Iinital version of paradynd speaking dynRPC igend protocol.
 *
 * Revision 1.5  1994/01/26  06:57:07  hollings
 * new header location
 *
 * Revision 1.4  1993/12/13  20:02:23  hollings
 * added applicationDefined
 *
 * Revision 1.3  1993/08/26  18:22:24  hollings
 * added cost model
 *
 * Revision 1.2  1993/07/01  17:03:17  hollings
 * added debugger calls and process control calls
 *
 * Revision 1.1  1993/03/19  22:51:05  hollings
 * Initial revision
 *
 *
 */

#ifndef INSTRUMENTATION_H
#define INSTRUMENTATION_H

#include "rtinst/h/trace.h"

/* time */
typedef double timeStamp;		

/* something that data can be collected for */
typedef struct _resourceRec *resource;		

/* descriptive information about a resource */
struct _resourceInfo {
    char *name;			/* name of actual resource */
    char *fullName;		/* full path name of resource */
    char *abstraction;          /* abstraction name */
    timeStamp creation;		/* when did it get created */
};		
typedef struct _resourceInfo resourceInfo;

/* list of resources */
typedef struct _resourceListRec *resourceList;		

/* a metric */
typedef struct _metricRec *metric;			

/* a list of metrics */
typedef struct _metricListRec *metricList;		

/* a metric/resourceList pair */
typedef class metricDefinitionNode *metricInstance;		

typedef enum { Trace, Sample } dataType;

/*
 * error handler call back.
 *
 */
typedef (*errorHandler)(int errno, char *message);

/*
 * Define a program to run (this is very tentative!)
 *
 *   argv - arguments to command
 *   envp - environment args, for pvm
 */
int addProcess(int argc, char*argv[], int nenv=0, char *envp[]=0);

/*
 * Find out if an application has been defined yet.
 *
 */
Boolean applicationDefined();

/*
 * Start an application running (This starts the actual execution).
 *  app - an application context from createPerformanceConext.
 */
Boolean startApplication();

/*
 *   Stop all processes associted with the application.
 *	app - an application context from createPerformanceConext.
 *
 * Pause an application (I am not sure about this but I think we want it).
 *      - Does this force buffered data to be delivered?
 *	- Does a paused application respond to enable/disable commands?
 */
Boolean pauseAllProcesses();

/*
 * Continue a paused application.
 *    app - an application context from createPerformanceConext.
 */
Boolean continueAllProcesses();


/*
 * Disconnect the tool from the process.
 *    pause - leave the process in a stopped state.
 *
 */
Boolean detachProcess(int pid, Boolean pause);

/*
 * Routines to control data collection.
 *
 * resourceList		- a list of resources
 * metric		- what metric to collect data for
 *
 */
int startCollecting(resourceList, metric);


/*
 * Return the expected cost of collecting performance data for a single
 *    metric at a given focus.  The value returned is the fraction of
 *    perturbation expected (i.e. 0.10 == 10% slow down expected).
 */
float guessCost(resourceList, metric);

/*
 * Control information arriving about a resource Classes
 *
 * resource		- enable notification of children of this resource
 */
Boolean enableResourceCreationNotification(resource);

/*
 * Resource utility functions.
 *
 */
resourceList getRootResources();

extern resource rootResource;

char *getResourceName(resource);

resource getResourceParent(resource);

resourceList getResourceChildren(resource);

Boolean isResourceDescendent(resource parent, resource child);

resource findChildResource(resource parent, char *name);

int getResourceCount(resourceList);

resource getNthResource(resourceList, int n);

resourceInfo *getResourceInfo(resource);

resourceList createResourceList();

Boolean addResourceList(resourceList, resource);

resource newResource(resource parent,
		     void *handle,
		     char *abstraction,
		     char *name,
		     timeStamp creation,
		     Boolean unique);

/*
 * manipulate user handle (a single void * to permit mapping between low level
 *   resource's and the resource consumer.
 *
 */
void *getResourceHandle(resource);

void setResourceHandle(resource, void*);

resourceList findFocus(int count, char **foucsString);

/*
 * Get the static configuration information.
 *
 */
metricList getMetricList();

/*
 * looks for a specifc metric instance in an application context.
 *
 */
metric findMetric(char *name);

/*
 * Metric utility functions.
 *
 */
char *getMetricName(metric);

/*
 * Get metric out of a metric instance.
 *
 */
metric getMetric(metricInstance);

#endif
