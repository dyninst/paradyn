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

// $Id: frame.h,v 1.16 2004/03/31 20:37:18 tlmiller Exp $

#ifndef FRAME_H
#define FRAME_H

#include <iostream>

class dyn_thread;
class process;
class dyn_lwp;
class codeRange;

class Frame {
 public:
  
  // default ctor (zero frame)
  Frame() : uppermost_(false), pc_(0), fp_(0), sp_(0),
    pid_(0), thread_(NULL), lwp_(NULL), isLeaf_(false), range_(0), isSignalFrame_(false), isTrampoline_(false), unwindCursor_(NULL)
  {}

  // I'm keeping the frame class (relatively) stupid,
  // so the standard method of getting a frame is to
  // call a different function (getActiveFrame or the
  // dyn_thread method) which gives you a frame. You can
  // then trace it back. The thread and lwp are just there
  // for reference by the frame user

  // For all those times you just need to stick in values
  Frame(Address pc, Address fp, 
		  unsigned pid, dyn_thread *thread, dyn_lwp *lwp, 
	bool uppermost, bool isLeaf = false, bool isSignalFrame = false, bool isTrampoline = false, void * unwindCursor = NULL ) :
    uppermost_(uppermost),
    pc_(pc), fp_(fp), sp_(0),
  pid_(pid), thread_(thread), lwp_(lwp), isLeaf_(isLeaf),
  range_(0), isSignalFrame_(isSignalFrame), isTrampoline_( isTrampoline ), unwindCursor_( unwindCursor )
    {};
  // Identical, with sp definition
  Frame(Address pc, Address fp, Address sp,
		  unsigned pid, dyn_thread *thread, dyn_lwp *lwp, 
	bool uppermost, bool isLeaf=false, bool isSignalFrame = false, bool isTrampoline = false, void * unwindCursor = NULL ) :
    uppermost_(uppermost),
    pc_(pc), fp_(fp), sp_(sp),
  pid_(pid), thread_(thread), lwp_(lwp), isLeaf_(isLeaf),
  range_(0), isSignalFrame_(isSignalFrame), isTrampoline_(isTrampoline), unwindCursor_( unwindCursor )
    {};

  Frame &operator=(const Frame &F) {
    uppermost_ = F.uppermost_;
    pc_ = F.pc_;
    fp_ = F.fp_;
    sp_ = F.sp_;
    pid_ = F.pid_;
    thread_ = F.thread_;
    lwp_ = F.lwp_;
    saved_fp = F.saved_fp;
    isLeaf_ = F.isLeaf_;
    range_ = F.range_;
    isSignalFrame_ = F.isSignalFrame_;
    isTrampoline_ = F.isTrampoline_;

	/* Don't worry about shallowness; all of the frames in a stack walk
	   point to the same cursor anyway, so trying to getCallerFrame()
	   on a frame you've already walked through will break even
	   if you don't copy the frame away. */
    unwindCursor_ = F.unwindCursor_;
    return *this;
  }

  bool operator==(const Frame &F) {
    return ((uppermost_ == F.uppermost_) &&
	    (pc_      == F.pc_) &&
	    (fp_      == F.fp_) &&
	    (sp_      == F.sp_) &&	    
	    (pid_     == F.pid_) &&
	    (thread_  == F.thread_) &&
	    (lwp_     == F.lwp_) &&
	    (saved_fp == F.saved_fp) &&
	    (isLeaf_  == F.isLeaf_) &&
	    (isSignalFrame_ == F.isSignalFrame_) &&
	    (isTrampoline_ == F.isTrampoline_) &&
	    (unwindCursor_ == F.unwindCursor_) );
  }

  Address  getPC() const { return pc_; }
  Address  getFP() const { return fp_; }
  Address  getSP() const { return sp_; }
  unsigned getPID() const { return pid_; }
  dyn_thread *getThread() const { return thread_; }
  dyn_lwp  *getLWP() const { return lwp_;}
  bool     isUppermost() const { return uppermost_; }
  bool	   isSignalFrame() const { return isSignalFrame_; }
  bool 	   isTrampoline() const { return isTrampoline_; }
  void setLeaf(bool isLeaf) { isLeaf_ = isLeaf; }
  codeRange *getRange() const { return range_;}
  void setRange(codeRange *r) { range_ = r;}
  friend ostream& operator<<(ostream&s, const Frame &m);

  // check for zero frame
  bool isLastFrame(process *p) const;
  
  // get stack frame of caller
  // May need the process image for various reasons
  Frame getCallerFrame(process *p) const;
  
  
 private:
  bool      uppermost_;
  Address   pc_;
  Address   fp_;
  Address   sp_;     // NOTE: this is not always populated
  int       pid_;    // Process id 
  dyn_thread *thread_; // user-level thread
  dyn_lwp  *lwp_;    // kernel-level thread (LWP)
  Address   saved_fp;// IRIX
  bool      isLeaf_; // Linux/x86
  codeRange *range_; // If we've done a by-address lookup,
                    // keep it here
  bool      isSignalFrame_;
  bool		isTrampoline_;
  void *	unwindCursor_; // Linux/ia64.
};

ostream& operator<<(ostream&s, const Frame &m);

#endif
