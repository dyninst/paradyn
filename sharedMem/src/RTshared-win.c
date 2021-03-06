/*
 * Copyright (c) 1996-2004 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as "Paradyn") on an AS IS basis, and do not warrant its
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

/************************************************************************
 *
 * $Id: RTshared-win.c,v 1.2 2006/04/14 02:08:09 legendre Exp $
 * RTshared.c: shared memory implementation
 *
 ************************************************************************/

#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <errno.h>
#include <limits.h>

/* Unfortunately, we still have platform problems... here it's unix vs NT */

void *RTsharedAttach(unsigned key, unsigned size, void *addr) {
    HANDLE hMap;
    TCHAR strName[128];
    void *result;
    
    /* Attach to pre-created shared memory segment */
    sprintf(strName, "ParadynD_%d_%d", key, size);
    hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS,
                           TRUE,
                           strName);
    if( hMap == NULL )
    {
        fprintf( stderr, "OpenFileMapping failed: %d\n", GetLastError() );
    }

    result = MapViewOfFile( hMap,
			    FILE_MAP_ALL_ACCESS,
			    0, 0,
			    0 );
    if( result == NULL )
    {
        fprintf( stderr, "MapViewOfFile failed: %d\n", GetLastError() );
    }
    return result;
}


void *RTsharedDetach(unsigned key, unsigned size, void *addr) {
    UnmapViewOfFile( addr );
    /*CloseHandle( hMapping );*/
    return NULL;
}

