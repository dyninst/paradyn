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

// $Id: TermWinTkGUI.h,v 1.4 2005/12/19 19:44:12 pack Exp $
#ifndef TERMWINTKGUI_H
#define TERMWINTKGUI_H

#include "common/h/String.h"
#include "visiClients/termWin/src/TermWin.h"

class TermWinTkGUI : public TermWin
{
private:
    // behaviors we can exhibit when Paradyn exits
    // (We explicitly specify numeric values because they must
    // match the values used in the termWin.tcl file.)
    enum CloseMode
    {
        CloseModeCloseOnParadynExit = 0,
        CloseModePersistent = 1
    };

    Tcl_Interp* interp;
    pdstring progName;
    CloseMode closeMode;

    static int CloseModeCmd( ClientData cd, Tcl_Interp* interp,
                                int argc, TCLCONST char** argv );

protected:
    thread_t tkTid;

    void SetCloseMode( CloseMode m )        { closeMode = m; }
    Tcl_Interp* GetInterp( void )           { return interp; }

public:
    TermWinTkGUI( pdstring progName, PDSOCKET lsock, PDSOCKET stdin_sock );
    virtual ~TermWinTkGUI( void );

    virtual bool Init( void );
    virtual bool IsDoneHandlingEvents( void ) const;
    virtual bool DispatchEvent( thread_t mtid, tag_t mtag );
    virtual void ShowError( const char* msg );
    virtual void Print( char* buf, int num, bool isFromDaemon );
    virtual bool DoPendingWork( void );

    // igen calls
    virtual void shutdown( void );
    
protected:    
    virtual void handle_error( void );
};

#endif // TERMWINTKGUI_H
