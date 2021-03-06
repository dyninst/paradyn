/*
 * Copyright (c) 1996-2003 Barton P. Miller
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
//----------------------------------------------------------------------------
//
// main-nt.C
//
// main-nt.C - main function for visis on Windows.
// After some necessary initialization, this routine simply calls 
// the platform-shared main() routine.
//
//----------------------------------------------------------------------------
// $Id: main-win.C,v 1.3 2006/05/04 01:41:58 legendre Exp $
//----------------------------------------------------------------------------
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "pdutil/h/winMain.h"

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
    MessageBox( NULL, "Press OK to continue", __argv[0], MB_OK );

    // provide a console for the visi so we can see error messages
    //InitConsole();
#endif // _DEBUG
    MessageBox( NULL, "Press OK to continue", __argv[0], MB_OK );
    DebugBreak();


    // initialize our use of the WinSock library
    InitSockets( __argv[0] );    

    // call the traditional main()
    return main( __argc, __argv );
}


