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
 * $Log: metricExt.h,v $
 * Revision 1.9  1997/06/07 21:01:27  newhall
 * replaced exclude_func and exclude_lib with exclude_node
 *
 * Revision 1.8  1997/06/05 04:29:48  newhall
 * added exclude_func mdl option to exclude shared object functions
 *
 * Revision 1.7  1996/09/26 19:03:28  newhall
 * added "exclude_lib" mdl option
 *
 * Revision 1.6  1996/08/16 21:12:25  tamches
 * updated copyright for release 1.1
 *
 * Revision 1.5  1995/06/02 20:49:03  newhall
 * made code compatable with new DM interface
 * fixed problem with force option in visiDef
 * fixed hpux link errors
 *
 * Revision 1.4  1995/05/18  10:58:36  markc
 * mdl
 *
 * Revision 1.3  1994/08/22  15:53:33  markc
 * Config language version 2.
 *
 * Revision 1.2  1994/07/07  13:10:43  markc
 * Turned off debugging printfs.
 *
 * Revision 1.1  1994/07/07  03:25:32  markc
 * Configuration language parser.
 *
 */

#ifndef _METRIC_EXT_H
#define _METRIC_EXT_H

#include "util/h/String.h"
#include "paradyn/src/DMthread/DMinclude.h"
#include "dyninstRPC.xdr.CLNT.h"

extern bool metMain(string &file);
extern bool mdl_init();
extern bool mdl_send(dynRPCUser *du);
extern void mdl_destroy();
extern bool mdl_apply();
extern bool mdl_check_node_constraints();

extern bool mdl_get_lib_constraints(vector<string> &);

#endif
