#ifndef _visiTypes_h
#define _visiTypes_h
/*
 * Copyright (c) 1993, 1994 Barton P. Miller, Jeff Hollingsworth,
 *     Bruce Irvin, Jon Cargille, Krishna Kunchithapadam, Karen
 *     Karavanic, Tia Newhall, Mark Callaghan.  All rights reserved.
 * 
 * This software is furnished under the condition that it may not be
 * provided or otherwise made available to, or used by, any other
 * person, except as provided for by the terms of applicable license
 * agreements.  No title to or ownership of the software is hereby
 * transferred.  The name of the principals may not be used in any
 * advertising or publicity related to this software without specific,
 * written prior authorization.  Any use of this software must include
 * the above copyright notice.
 *
 */
/* $Log: visiTypes.h,v $
/* Revision 1.9  1995/02/26 01:59:29  newhall
/* added phase interface functions
/*
 * Revision 1.8  1995/02/16  09:32:05  markc
 * Modified to support machines which do not have NaN(x).
 * This code has not been tested, but it compiles.
 *
 * Revision 1.7  1994/09/25  01:58:15  newhall
 * changed interface definitions to work for new version of igen
 * changed AddMetricsResources def. to take array of metric/focus pairs
 *
 * Revision 1.6  1994/08/03  20:57:03  newhall
 * *** empty log message ***
 *
 * Revision 1.5  1994/08/03  20:47:46  newhall
 * removed event type NEWMETRICSRESOURCES
 *
 * Revision 1.4  1994/07/30  03:26:56  newhall
 * added new msgTag type ENABLED
 *
 * Revision 1.3  1994/07/28  22:23:19  krisna
 * changed definition of ERROR to use NaN(X)
 *
 * Revision 1.2  1994/05/23  20:55:19  newhall
 * To visi_GridCellHisto class: added deleted flag, SumValue
 * method function, and fixed AggregateValue method function
 *
 * Revision 1.1  1994/05/11  17:11:08  newhall
 * changed data values from double to float
 * */

#include <stdio.h>
#include <math.h>
// TODO -- nan.h is a non-standard header file -- there has to be a portable
// way to do this -- mdc -- I am not sure if this works, but it compiles
#if !defined(i386_unknown_netbsd1_0) && !defined(hppa1_1_hp_hpux)
#include <nan.h>
#endif /* !defined(i386_unknown_netbsd1_0) */
#include "util/h/String.h"

#define INVALID            0
#define VALID              1
#define NOVALUE           -1
#define OK                 0
#define VISI_ERROR_BASE   -19
#define ERROR_REALLOC     -20
#define ERROR_CREATEGRID  -21
#define ERROR_SUBSCRIPT   -22
#define ERROR_AGGREGATE   -23
#define ERROR_NOELM       -24
#define ERROR_MALLOC      -25
#define ERROR_STRNCPY     -26
#define ERROR_INIT        -27
#define VISI_ERROR_MAX    -27

static double visi_nan = 0;
static float f_visi_nan = 0;
static nan_created = false;
// TODO -- NaN is a non standard macro -- it is not portable
// TODO -- should a float or a double be used here
#if !defined(i386_unknown_netbsd1_0) && !defined(hppa1_1_hp_hpux)
#define ERROR (NaN(visi_nan),visi_nan)
#else
#include <assert.h>
inline float make_ERROR() {
  if (!nan_created) {
    visi_nan = sqrt(-3);
    f_visi_nan = sqrt(-3);
    nan_created = true;
  }
  assert(isnan(f_visi_nan));
  assert(isnan(visi_nan));
  return f_visi_nan;
}
#define ERROR make_ERROR()
#endif

//
// event types associated with events from Paradyn to a visualization
// DATAVALUES:  a new set of data has arrived in the datagrid
// INVALIDMETRICSRESOURCES:  a metric resource combination has become invalid
// ADDMETRICSRESOURCES:  new metrics have become enabled for a resource
// PHASESTART:  a new phase has been defined
// PHASEEND:    a phase has ended
// PHASEDATA:  data about the current set of phases has arrived
// FOLD:  the histogram has folded; binWidth has doubled
//
typedef enum {DATAVALUES,INVALIDMETRICSRESOURCES,ADDMETRICSRESOURCES,
	      PHASESTART,PHASEEND,PHASEDATA,FOLD} msgTag;


typedef float sampleType;
typedef double timeType;

struct metricStruct {
     string units;    // how units are measured
     string name;     // metric name for graph labeling  
     int     Id;       // unique metric Id
     int     aggregate;  //either SUM or AVE
};
typedef struct metricStruct visi_metricType;


struct resourceStruct{
     string name;     // resource name for graph labeling
     int    Id;       // unique resource id
};
typedef struct resourceStruct visi_resourceType;


struct dataValueStruct{
     int   metricId;
     int   resourceId;
     int   bucketNum;
     sampleType data;
};
typedef struct dataValueStruct visi_dataValue;

extern void visi_ErrorHandler(int errno,const char *msg);

#endif
