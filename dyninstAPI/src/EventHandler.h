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

#ifndef __EVENT_HANDLER__H__
#define __EVENT_HANDLER__H__

#ifdef DYNINST_CLASS_NAME
#undef DYNINST_CLASS_NAME
#endif
#define DYNINST_CLASS_NAME EVENT_HANDLER
#include "common/h/Types.h" /*for Address */
#include "common/h/Pair.h" /*for pdpair */
#include "common/h/Vector.h" /*for pdvector */
#include "common/h/headers.h" /*for DEBUG_EVENT */
#include "BPatch_eventLock.h"
#include "mailbox.h"
class process;
class dyn_lwp;

typedef enum {
  evtUndefined,   /* undefined event -- usually indicates error */
  evtNullEvent,   /* null event, nothing (important) happened, but no error */
  evtAnyEvent,
  evtNewConnection,
  evtTimeout,
  evtSignalled,
  evtException,
  evtCritical,
  evtProcessCreate,
  evtProcessAttach,
  evtProcessExit, /* used to have exited normally, or via signal, now in status */
  evtProcessStop,
  evtProcessSelfTermination,
  evtThreadCreate,
  evtThreadExit,
  evtThreadContextStart,
  evtThreadContextStop,
  evtLoadLibrary,
  evtUnloadLibrary,
  evtSyscallEntry,
  evtSyscallExit,
  evtSuspended, 
  evtInstPointTrap,
  evtDebugStep,
  evtDynamicCall,
  evtRPCSignal,
  evtError,
  evtPreFork,
  evtPostFork,
  evtExec,
  evtOneTimeCode,
  evtUserEvent,
  evtShutDown,
  evtProcessInit, /*aka "initialized" */
  evtProcessLoadedRT, /* dyninst RTlib has been loaded */
  evtProcessInitDone, /* aka bootstrapped */
  evtThreadDetect, /* only for linux waitpid loop */
  //  If you are adding to this list, please augment the function eventType2str(),
  //  to reflect your changes.
  evtLastEvent /* placeholder for the end of the list*/
} eventType;

char *eventType2str(eventType x); 

typedef Address eventAddress_t;

typedef enum {
  statusUnknown,
  statusNormal,
  statusSignalled,
  statusRPCDone,
  statusRPCAtReturn,
  statusError
} eventStatusCode_t;

#define NULL_STATUS_INITIALIZER statusUnknown
#if defined (os_windows)

typedef DEBUG_EVENT eventInfo_t;
typedef DWORD eventWhat_t;
#define THREAD_RETURN void
#define DO_THREAD_RETURN return
#else // !windows

typedef unsigned long eventInfo_t;
typedef unsigned int eventWhat_t;
#define THREAD_RETURN void *
#define DO_THREAD_RETURN return NULL
#endif
typedef int eventFileDesc_t;
class EventRecord {
  public:
   EventRecord(); 
   process *proc;
   dyn_lwp *lwp;  // the lwps causing the event (not the representative lwp)
   eventType type;
   eventWhat_t what;
   eventStatusCode_t status;
   eventInfo_t info;
   eventAddress_t address;
   eventFileDesc_t fd; /* only used in async events -- wasteful for clarity*/

   char *sprint_event(char *buf); 
   bool isTemplateOf(EventRecord &src);
}; 

#if defined (os_windows)
typedef void (*thread_main_t)(void *);
#else
typedef void *(*thread_main_t)(void *);
#endif

//template <class S>
class InternalThread {
  public:
  //  InternalThread::getThreadID(), return thread id
  //  returns tid (unsigned) -1 if thread not created yet
  unsigned long getThreadID() { return tid;}
  bool isRunning() {return _isRunning;}
  const char *getName() {return idstr;}
  protected:
  InternalThread(const char *id);
  virtual ~InternalThread();
  bool createThread();
  bool killThread();

  virtual void main() = 0;

  bool _isRunning;
  unsigned long tid;
  char *idstr;
  eventLock *startupLock;
  
  bool init_ok;
  private:
  
#if defined (os_windows)
  unsigned long handler_thread;
#else
  pthread_t handler_thread;
#endif
};

#ifdef DYNINST_CLASS_NAME
#undef DYNINST_CLASS_NAME
#endif
#define DYNINST_CLASS_NAME EventHandler

template <class T>
class EventHandler : public InternalThread {
  friend THREAD_RETURN eventHandlerWrapper(void *);
  friend THREAD_RETURN asyncHandlerWrapper(void *);

  public:
  void stopThreadNextIter() {stop_request = true;}
  protected:

  //  initialize_event_handler is called before the main event handling
  //  loop begins on the event handling thread.  When overloaded, allows 
  //  derived classes to do some initialization on the created thread.
  //  Returns false on failure to initialize.  Upon failure, the event loop
  //  is not entered at all and the created thread exits.
  virtual bool initialize_event_handler() {return true;}

  EventHandler(eventLock *_lock, const char *id, bool create_thread);
  virtual ~EventHandler();

  virtual bool waitNextEvent(T &ev) = 0; 
  virtual bool handleEvent(T &ev) = 0;
  
  bool _Lock(const char *__file__, unsigned int __line__); 
  bool _Unlock(const char *__file__, unsigned int __line__); 
  bool _WaitForSignal(const char *__file__, unsigned int __line__); 
  bool _Broadcast(const char *__file__, unsigned int __line__); 

  eventLock *eventlock;
  bool stop_request;
  private:
  void main();

};

extern pdvector< pdpair<unsigned long, const char *> > threadmap;
const char *getThreadStr(unsigned long tid);
unsigned long getExecThreadID();
#endif
