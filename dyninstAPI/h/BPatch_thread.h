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

#ifndef _BPatch_thread_h_
#define _BPatch_thread_h_


/*
 * The following is a kludge so that the functions that refer to signals (such
 * as BPatch_thread::stopSignal) can emulate the Unix behavior on Windows NT.
 */
#include <signal.h>
#ifndef SIGTRAP
#define SIGTRAP		5
#endif

#include "BPatch_dll.h"
#include "BPatch_Vector.h"
#include "BPatch_image.h"
#include "BPatch_snippet.h"

class process;
class miniTrampHandle;
class BPatch;
class BPatch_thread;

// BPatch_callWhen is defined in BPatch_point.h

/*
 * Used to specify whether a snippet should be installed before other snippets
 * that have previously been inserted at the same point, or after.
 */
typedef enum {
    BPatch_firstSnippet,
    BPatch_lastSnippet
} BPatch_snippetOrder;


/*
 * Contains information about the code that was inserted by an earlier call to
 * Bpatch_thread::insertSnippet.
 */
class BPATCH_DLL_EXPORT BPatchSnippetHandle {
    friend class BPatch_thread;
private:
    BPatch_Vector<miniTrampHandle *> mtHandles;
      
public:
    process *proc;

    BPatchSnippetHandle(process *_proc) : proc(_proc) {};
    ~BPatchSnippetHandle();

    void add(miniTrampHandle *pointInstance);
};

/*
 * Represents a thread of execution.
 */
class BPATCH_DLL_EXPORT BPatch_thread {
    friend class BPatch;
    friend class BPatch_image;
    friend class BPatch_function;
    friend class process;
    friend bool pollForStatusChange();

    process		*proc;
    BPatch_image	*image;
    int			lastSignal;
    int                 exitCode;
    bool		mutationsActive;
    bool		createdViaAttach;
    bool		detached;

    bool		waitingForOneTimeCode;
    void		*lastOneTimeCodeReturnValue;

    bool		unreportedStop;
    bool		unreportedTermination;

    void		setUnreportedStop(bool new_value)
				{ unreportedStop = new_value; }
    void		setUnreportedTermination(bool new_value)
				{ unreportedTermination = new_value; }

    bool		pendingUnreportedStop()
				{ return unreportedStop; }
    bool		pendingUnreportedTermination()
				{ return unreportedTermination; }

    bool		statusIsStopped();
    bool		statusIsTerminated();

    static void		oneTimeCodeCallbackDispatch(process *theProc,
						    void *userData,
						    void *returnValue);

    void		oneTimeCodeCallback(void *userData,
					    void *returnValue);

    void		*oneTimeCodeInternal(const BPatch_snippet &expr);

protected:
    // for creating a process
    BPatch_thread(char *path, char *argv[], char *envp[] = NULL, int stdin_fd = 0,
	int stdout_fd = 1, int stderr_fd = 2);
    // for attaching
    BPatch_thread(char *path, int pid);	

    // for forking
    BPatch_thread(int childPid, process *proc);

public:
    ~BPatch_thread();

    BPatch_image *getImage() { return image; }

    int		getPid();

    bool	stopExecution();
    bool	continueExecution();
    bool	terminateExecution();

    bool	isStopped();
    int		stopSignal();
    bool	isTerminated();
    int         terminationStatus();

    void	detach(bool cont);

    bool	dumpCore(const char *file, bool terminate);
    bool	dumpImage(const char *file);
    char*	dumpPatchedImage(const char* file);//ccw 28 oct 2001

    BPatch_variableExpr	*malloc(int n);
    BPatch_variableExpr	*malloc(const BPatch_type &type);
    void	free(const BPatch_variableExpr &ptr);
    BPatch_variableExpr *getInheritedVariable(const BPatch_variableExpr &pVar);

    // to provide backward compatiblity 
    BPatchSnippetHandle *insertSnippet(
			    const BPatch_snippet &expr,
			    BPatch_point &point,
			    BPatch_snippetOrder order = BPatch_firstSnippet);

    BPatchSnippetHandle *insertSnippet(
			    const BPatch_snippet &expr,
			    BPatch_point &point,
			    BPatch_callWhen when,
			    BPatch_snippetOrder order = BPatch_firstSnippet);

    BPatchSnippetHandle *insertSnippet(
			    const BPatch_snippet &expr,
			    const BPatch_Vector<BPatch_point *> &points,
			    BPatch_snippetOrder order = BPatch_firstSnippet);

    BPatchSnippetHandle *insertSnippet(
			    const BPatch_snippet &expr,
			    const BPatch_Vector<BPatch_point *> &points,
			    BPatch_callWhen when,
			    BPatch_snippetOrder order = BPatch_firstSnippet);

    bool	deleteSnippet(BPatchSnippetHandle *handle);

    void	setMutationsActive(bool activate);

    bool	replaceFunctionCall(BPatch_point &point,
				    BPatch_function &newFunc);
    bool	removeFunctionCall(BPatch_point &point);
    bool        replaceFunction(BPatch_function &oldFunc,
				BPatch_function &newFunc);

    void	oneTimeCode(const BPatch_snippet &expr) {
			oneTimeCodeInternal(expr);
		    };

	//the reload argument is used by save the world to determine
	//if this library should be reloaded by the mutated binary
	//when it starts up. this is up to the user because loading
	//an extra shared library could hide access to the 'correct'
	//function by redefining a function  
    bool	loadLibrary(char *libname, bool reload = false); 

    //method that retrieves the line number and file name corresponding 
    //to an address
    bool getLineAndFile(unsigned long addr,unsigned short& lineNo,
			char* fileName,int length);

	void startSaveWorld();//ccw 23 jan 2002
#ifdef IBM_BPATCH_COMPAT
    bool isThreaded() { return false; }
    bool addSharedObject(const char *name, const unsigned long loadaddr);
#endif

};

#endif /* BPatch_thread_h_ */
