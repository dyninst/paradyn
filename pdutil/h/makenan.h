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

// $Id: makenan.h,v 1.9 1998/08/17 02:05:34 wylie Exp $

#ifndef makenan_H
#define makenan_H

#include <math.h>
#include <assert.h>

#if defined (i386_unknown_linux2_0)
   // provide suitable definitions if one is not available from the
   // platform's /usr/include/math.h or similar
   #define DOMAIN 1
   #define MATH_EXCEPTION_STRUCT __math_exception
   struct MATH_EXCEPTION_STRUCT {
        int type;       // exception type
        char *name;     // name of function where error occured
        double arg1;    // 1st argument to function
        double arg2;    // 2nd argument to function (if any)
        double retval;  // value to be returned by function
   };
   extern int matherr(struct MATH_EXCEPTION_STRUCT *x);
#endif

extern float f_paradyn_nan;
extern bool nan_created;
extern bool matherr_flag;

// There is no standard macro to create a NaN valued float
extern float make_Nan();
#define PARADYN_NaN make_Nan()

#endif
