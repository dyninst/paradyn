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

/* $Id: UIglobals.h,v 1.25 1999/04/27 16:03:43 nash Exp $ */

/* UIglobals.h 
     definitions used by UI thread */

#ifndef _ui_globals_h
#define _ui_globals_h

#define UIM_DEBUG 0

#include "dataManager.thread.CLNT.h"
#include "performanceConsultant.thread.CLNT.h"
#include "UI.thread.SRVR.h"
#include "thread/h/thread.h"
#include "paradyn/src/DMthread/DMinclude.h"

#include "tk.h"

struct cmdTabEntry 
{
  const char *cmdname;
  int (*func)(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);
};

typedef struct UIMReplyRec {
  void *cb;
  thread_t tid;
} UIMReplyRec;

extern List<metricInstInfo *>    uim_enabled;
extern perfStreamHandle          uim_ps_handle;
extern UIM                       *uim_server;

// callback pointers stored here for lookup after return from tcl/tk routines
//  int tokens are used within tcl/tk code since we can't use pointers
extern Tcl_HashTable UIMMsgReplyTbl;
extern int UIMMsgTokenID;

// this tcl interpreter used for entire UI
extern Tcl_Interp *interp;   

// set for batch mode; clear for normal
extern int UIM_BatchMode;    

#include "Status.h"
extern status_line *ui_status;

// value of highest valid error index
extern int uim_maxError;     

// metric-resource selection 
extern vector<metric_focus_pair> uim_VisiSelections;

int TclTunableCommand(ClientData cd, Tcl_Interp *interp,
                      int argc, char **argv);

#endif
