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

// $Id: aix.h,v 1.13 2001/11/06 19:20:20 bernat Exp $

#if !defined(rs6000_ibm_aix3_2) && !defined(rs6000_ibm_aix4_1)
#error "invalid architecture-os inclusion"
#endif

#ifndef AIX_PD_HDR
#define AIX_PD_HDR

#include <sys/param.h>
#define EXIT_NAME "exit"

#define START_WALL_TIMER "DYNINSTstartWallTimer"
#define STOP_WALL_TIMER  "DYNINSTstopWallTimer"

#if defined(MT_THREAD)
#define START_PROC_TIMER           "DYNINSTstartThreadTimer"
#define STOP_PROC_TIMER            "DYNINSTstopThreadTimer" 
#define DESTROY_PROC_TIMER         "DYNINSTdestroyThreadTimer"
#define START_PROC_TIMER_LWP       "DYNINSTstartThreadTimer"
#define START_PROC_TIMER_LWP_IRPC  "DYNINSTstartThreadTimer_inferiorRPC"
#else
#define START_PROC_TIMER "DYNINSTstartProcessTimer"
#define STOP_PROC_TIMER  "DYNINSTstopProcessTimer" 
#endif

#define SIGNAL_HANDLER  0

/* How many bytes our insertion of "dlopen <libname>" needs */
/* Should be set dynamically */
/* Nice thing about AIX is that we don't have to overwrite main with
   the call, we just use the empty space after the program */
#define BYTES_TO_SAVE 256 // should be a multiple of sizeof(instruction)

typedef int handleT; // defined for compatibility with other platforms
                     // not currently used on the AIX platform
#endif
