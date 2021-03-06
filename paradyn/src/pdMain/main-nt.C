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
//----------------------------------------------------------------------------
//
// main-nt.C
//
// main-nt.C - main function for Paradyn on Windows.
// After some necessary initialization, this routine simply calls 
// the platform-shared main() routine.
//
//----------------------------------------------------------------------------
// $Id: main-nt.C,v 1.7 2006/04/13 23:05:35 legendre Exp $
//----------------------------------------------------------------------------
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include "pdutil/h/winMain.h"

const "C" char V_paradyn[] = "$Paradyn: v5.0 paradyn #0 " __DATE__ __TIME__ "paradyn@cs.wisc.edu$";

int main( int argc, char* argv[] );

int
WINAPI
WinMain( HINSTANCE hInstance,
         HINSTANCE hPrevInstance,
         LPSTR lpCmdLine,
         int nCmdShow )
{
#ifdef _DEBUG
    // pause for attaching a debugger if desired
    MessageBox( NULL,
                "Pausing for debugger attach.  Press OK to continue",
                __argv[0],
                MB_OK );
#endif // _DEBUG

    // provide a console so we can see error messages
    InitConsole();

    // initialize our use of the WinSock library
    InitSockets( "Paradyn" );    

    // call the traditional main()
    int ret = main( __argc, __argv );
    
    // if we reached here, we've exited gracefully and don't 
    // need to keep the console around
    ClearWaitForConsoleKeypress();

    return ret;
}
