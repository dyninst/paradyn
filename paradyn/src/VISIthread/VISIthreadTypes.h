#ifndef VISI_thread_h
#define VISI_thread_h 
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
/* $Log: VISIthreadTypes.h,v $
/* Revision 1.7  1994/08/11 02:19:22  newhall
/* added call to dataManager routine destroyPerformanceStream
/*
 * Revision 1.6  1994/06/07  18:16:29  newhall
 * support for adding metrics/resources to an existing set
 *
 * Revision 1.5  1994/06/03  18:22:50  markc
 * Changes to support igen error handling.
 *
 * Revision 1.4  1994/05/11  17:21:29  newhall
 * Changes to handle multiple curves on one visualization
 * and multiple visualizations.  Fixed problems with folding
 * and resource name string passed to visualization.  Changed
 * data type from double to float.
 *
 * Revision 1.3  1994/04/29  18:57:35  newhall
 * changed typedefs of structs to deal with g++/gdb bug
 *
 * Revision 1.2  1994/04/28  22:08:07  newhall
 * test version 2
 *
 * Revision 1.1  1994/04/09  21:23:04  newhall
 * test version
 * */
#include "thread/h/thread.h"
#include "VM.CLNT.h"
#include "UI.CLNT.h"
#include "dataManager.CLNT.h"
#include "visi.CLNT.h"
#include "../pdMain/paradyn.h"


#define BUFFERSIZE 64 
#define SUM     0
#define AVE     1
#define  VISI_DEFAULT_FOCUS "Whole Program"

////////////////////////////////////////
//  for VISIthread local data  
///////////////////////////////////////
struct VISIGlobalsStruct {

  UIMUser *ump;
  VMUser *vmp;
  dataManagerUser *dmp;
  visualizationUser *visip;
  performanceStream *perStream;
  dataValue buffer[BUFFERSIZE];
  int bufferSize;
  int fd;
  int pid;
  int quit;
  double bucketWidth;
  List<metricInstance *> *mrlist;  // data and key are metricInstance *

};
typedef struct VISIGlobalsStruct VISIthreadGlobals;

class visiUser : public visualizationUser
{
  public:
    visiUser(int fd, xdrIOFunc r, xdrIOFunc w, int nblock=0) :
	visualizationUser(fd, r, w, 0) {;};
    virtual void handle_error();
};
#endif
