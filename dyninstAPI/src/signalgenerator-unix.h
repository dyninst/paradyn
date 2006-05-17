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

/* $Id: signalgenerator-unix.h,v 1.9 2006/05/17 04:13:05 legendre Exp $
 */


#ifndef _SIGNAL_GENERATOR_UNIX_H_
#define _SIGNAL_GENERATOR_UNIX_H_

#include "dyninstAPI/src/os.h"

class SignalGenerator;

typedef struct waitpid_ret_pair {
   int pid;
   int status;
} waitpid_ret_pair_t;

typedef struct pid_generator_pair {
   int pid;
   SignalGenerator *sg;
} pid_generator_pair_t;

class SignalGenerator : public SignalGeneratorCommon
{
  friend class SignalHandler;
  friend class SignalGeneratorCommon;
  friend class process;

  public:
   virtual ~SignalGenerator() {}

   bool checkForExit(EventRecord &ev, bool block =false);

   bool waitingForStop() {return waiting_for_stop;}
   void setWaitingForStop(bool flag) {waiting_for_stop = flag;}
   void expectFakeSignal() {expect_fake_signal = true;}
  private:
  //  SignalGenerator should only be constructed by process
  SignalGenerator(char *idstr, pdstring file, pdstring dir,
                  pdvector<pdstring> *argv,
                  pdvector<pdstring> *envp,
                  pdstring inputFile,
                  pdstring outputFile,
                  int stdin_fd, int stdout_fd,
                  int stderr_fd);

  SignalGenerator(char *idstr, pdstring file, int pid);

  virtual SignalHandler *newSignalHandler(char *name, int id);

  virtual bool forkNewProcess();
  virtual bool attachProcess();
  virtual bool waitForStopInline();
  virtual bool decodeEvents(pdvector<EventRecord> &events);
  virtual bool waitForEventsInternal(pdvector<EventRecord> &events);

  //  decodeSignal_NP is called by decodeSignal before decodeSignal does any
  //  decoding.  It allows platform specific actions to be taken for signal
  //  decoding.  If it returns true, decodeSignal assumes that a valid decode 
  //  has taken place and it  will not do any further decoding.
  bool decodeSignal_NP(EventRecord &ev);
  bool decodeSignal(EventRecord &ev);
  bool decodeRTSignal(EventRecord &ev);
  bool decodeSigTrap(EventRecord &ev);
  bool decodeSigStopNInt(EventRecord &ev);
  bool decodeSigIll(EventRecord &ev);

  //  decodeSyscall changes the field ev.what from a platform specific
  //  syscall representation, eg, SYS_fork, to a platform indep. one,
  //  eg. procSysFork.  returns false if there is no available mapping.
  bool decodeSyscall(EventRecord &ev);

#if !defined (os_linux) 
   bool decodeProcStatus(procProcStatus_t status, EventRecord &ev);
#endif

#if 0 // PDSEP
#if !defined (os_linux) 
   bool updateEvents(pdvector<EventRecord> &events, process *p, int lwp_to_use);
   bool updateEventsWithLwpStatus(process *curProc, dyn_lwp *lwp,
                                  pdvector<EventRecord> &events);
#endif
#endif

   bool waiting_for_stop;
   /* Cached for speed */
   Address sync_event_id_addr;
   Address sync_event_arg1_addr;
   Address sync_event_breakpoint_addr;

   void clearCachedLocations();


   bool expect_fake_signal;

#if defined (os_linux)
   public:
   bool add_lwp_to_poll_list(dyn_lwp *lwp);
   bool remove_lwp_from_poll_list(int lwp_id);
   bool resendSuppressedSignals();
   bool exists_dead_lwp();
   private:
   pdvector<int> suppressed_sigs;
   pdvector<dyn_lwp *> suppressed_lwps;
   //  SignalHandler::suppressSignalWhenStopping
   //  needed on linux platforms.  Allows the signal handler function
   //  to ignore most non SIGSTOP signals when waiting for a process to stop
   //  Returns true if signal is to be suppressed.
   bool suppressSignalWhenStopping(EventRecord &ev);
   //  SignalHandler::resendSuppressedSignals
   //  called upon receipt of a SIGSTOP.  Sends all deferred signals to the stopped process.
   int find_dead_lwp();
   pid_t waitpid_kludge(pid_t, int *, int, int *);
   pdvector<int> attached_lwp_ids;

 public:
   static int waitpid_demultiplex(SignalGenerator *me, int *status);

   pdvector<waitpid_ret_pair> event_queue;
   bool isInWaitpid;
   bool isInWaitLock;
   bool forcedExit;
   static pdvector<waitpid_ret_pair> unassigned_events;
   static pdvector<SignalGenerator *> first_timers;

   static volatile int waiter_exists;
   static pthread_cond_t waiter_condvar;
   static pthread_mutex_t waiter_mutex;
   static pdvector<pid_generator_pair_t> pidgens;
   void addPidGen(int pid);
   void removePidGen(int pid);
   void removePidGen();
   bool hasFirstTimer();
   void forceWaitpidReturn();

#endif

};

#endif
