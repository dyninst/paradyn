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
/* Revision 1.1  1994/05/11 17:11:08  newhall
/* changed data values from double to float
/* */

#include <stdio.h>
#include <math.h>
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
#define ERROR              quiet_nan() 

//
// event types associated with events from Paradyn to a visualization
//
typedef enum {DATAVALUES,INVALIDMETRICSRESOURCES,ADDMETRICSRESOURCES,
	      NEWMETRICSRESOURCES,PHASENAME,FOLD} msgTag;


typedef float sampleType;
typedef double timeType;

struct metricStruct {
     char *units;    // how units are measured  i.e. "ms" 
     char *name;     // for y-axis labeling  
     int     Id;       // unique metric Id
     int     aggregate;  //either SUM or AVE
};
typedef struct metricStruct visi_metricType;


struct resourceStruct{
     char *name;     // obj. name for graph labeling
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

extern void visi_ErrorHandler(int errno,char *msg);

#endif
