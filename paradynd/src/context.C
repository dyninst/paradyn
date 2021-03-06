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

/* $Id: context.C,v 1.131 2008/03/12 20:09:38 legendre Exp $ */

#include "rtinst/h/rtinst.h"
#include "rtinst/h/trace.h"
#include "paradynd/src/pd_process.h"
#include "paradynd/src/pd_thread.h"
#include "paradynd/src/metricFocusNode.h"
#include "paradynd/src/perfStream.h"
#include "paradynd/src/costmetrics.h"
#include "paradynd/src/init.h"
#include "paradynd/src/context.h"
#include "paradynd/src/processMgr.h"
#include "paradynd/src/pd_image.h"
#include "paradynd/src/pd_module.h"
#include "paradynd/src/comm.h"
#include "paradynd/src/debug.h"

#if !defined(i386_unknown_nt4_0)
extern int termWin_port; //defined in main.C
extern pdstring pd_machine;
#endif

extern pdRPC *tp;
extern void CallGraphSetEntryFuncCallback(pdstring exe_name, pdstring r, int tid);

void doDelayedReport()
{
	/*
	for(unsigned i = 0 ; i < delayedReport.size() ; i++)
		{
			g_node * gn = delayedReport[i];
			CallGraphSetEntryFuncCallback(gn->file, gn->full_name, gn->tid);
		}
	delayedReport.resize(0);
	*/
}

#if 0
void deleteThread(traceThread *fr)
{
    pd_process *pdproc = NULL;

    assert(fr);
    pdproc = getProcMgr().find_pd_process(fr->ppid);
    
    if (!pdproc) return;

    assert(pdproc && pdproc->get_rid());

    pd_thread *thr = pdproc->thrMgr().find_pd_thread(fr->tid);
    tp->retiredResource(thr->get_rid()->full_name());

    // take a final sample when thread is noticed as exited, but before it's
    // meta-data is deleted;
    pdproc->doMajorShmSample();  // take a final sample
    metricFocusNode::handleExitedThread(pdproc, thr);
    pdproc->getVariableMgr().deleteThread(thr);

    // deleting thread
    pdproc->removeThread(fr->tid);
    pdproc->get_dyn_process()->lowlevel_process()->deleteThread(fr->tid);
    // deleting resource id
    // how do we delete a resource id? - naim
}
#endif

unsigned miniTrampHandlePtrHash(miniTrampHandle * const &ptr) {
   // would be a static fn but for now aix.C needs it.
   unsigned addr = (unsigned)(Address)ptr;
   return addrHash16(addr); // util.h
}

void pd_execCallback(pd_process *pd_proc) {
   if (!pd_proc) {
      pdlogLine("Error in pd_execCallback: could not find pd_process\n");
      return;
   }

   metricFocusNode::handleExec(pd_proc);
}

#if !defined(i386_unknown_nt4_0)

PDSOCKET connect_Svr(pdstring machine,int port)
{
  PDSOCKET stdout_fd = INVALID_PDSOCKET;

  struct sockaddr_in serv_addr;
  struct hostent *hostptr = 0;
  struct in_addr *inadr = 0;
  if (!(hostptr = P_gethostbyname(machine.c_str())))
    {
      cerr << "CRITICAL: Failed to find information for host " << pd_machine.c_str() << "." << endl;
      assert(0);
    }

  inadr = (struct in_addr *) ((void*) hostptr->h_addr_list[0]);
  P_memset ((void*) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr = *inadr;
  serv_addr.sin_port = htons(port);

  if ( (stdout_fd = P_socket(AF_INET,SOCK_STREAM , 0)) == PDSOCKET_ERROR)
  {
    stdout_fd = INVALID_PDSOCKET;
    return stdout_fd;
  }

  //connect() may timeout if lots of Paradynd's are trying to connect to
  //  Paradyn at the same time, so we keep retrying the connect().
  errno = 0;
  while (P_connect(stdout_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == PDSOCKET_ERROR) {
/*#if defined(i386_unknown_nt4_0)
    if (PDSOCKET_ERRNO != WSAETIMEDOUT)
#else */
    if (errno != ETIMEDOUT)
//#endif
    {
      stdout_fd = INVALID_PDSOCKET;
      return stdout_fd;
    } 
    CLOSEPDSOCKET(stdout_fd);
    if ((stdout_fd = P_socket(AF_INET,SOCK_STREAM, 0)) == PDSOCKET_ERROR)
    {
      stdout_fd = INVALID_PDSOCKET;
      return stdout_fd;
    }
    errno = 0;
  }

  return stdout_fd;
}
#endif


#ifdef notdef
bool addDataSource(char *name, char *machine,
    char *login, char *command, int argc, char *argv[])
{
    P_abort();
    return(false);
}
#endif

// TODO use timers here
timeStamp startPause = timeStamp::ts1970();

// total processor time the application has been paused.
// so for a multi-processor system this should be processor * time.
timeLength elapsedPauseTime = timeLength::Zero();

static bool appPause = true;

bool markApplicationPaused()
{
  if (!appPause) {
    // get the time when we paused it.
    startPause = getWallTime();
    // sprintf(errorLine, "paused at %f\n", startPause);
    // pdlogLine(errorLine);
    appPause = true;
    return true;
  } else 
    return false;
}

timeStamp *pFirstRecordTime = NULL;

void setFirstRecordTime(const timeStamp &ts) {
  if(pFirstRecordTime != NULL) delete pFirstRecordTime;
  pFirstRecordTime = new timeStamp(ts);
}
bool isInitFirstRecordTime() {
  if(pFirstRecordTime == NULL) return false;
  else return true;
}
const timeStamp &getFirstRecordTime() {
  if(pFirstRecordTime == NULL) assert(0);
  return *pFirstRecordTime;
}

bool markApplicationRunning()
{
  if (!appPause) {
//    cerr << "WARNING: markApplicationRunning: the application IS running\n";
    return false;
  }
  appPause = false;

  if (!isInitFirstRecordTime()) {
    cerr << "WARNING: markApplicationRunning: !firstRecordTime\n";
    return false;
  }

  if (startPause > timeStamp::ts1970())
    elapsedPauseTime += (getWallTime() - startPause);

  return true;
}

bool isApplicationPaused()
{
  return appPause;
}

bool continueAllProcesses()
{
    for (processMgr::procIter itr = getProcMgr().begin();
         itr != getProcMgr().end();
         itr++) {
        
        pd_process *p = *itr;
        if(p != NULL && p->isStopped()) {
            if(p->isTerminated() || ! p->continueProc()) {
                sprintf(errorLine,"WARNING: cannot continue process %d\n",
                        p->getPid());
                cerr << errorLine << endl;
            }
        }
    }
    
    pdstatusLine("application running");
    if (!markApplicationRunning()) {
        return false;
    }
    
    return(false); // Is this correct?
}

bool pauseAllProcesses()
{
    bool changed = markApplicationPaused();
    for (processMgr::procIter itr = getProcMgr().begin();
         itr != getProcMgr().end();
         itr++) {
        pd_process *p = *itr;
        if (p != NULL && !p->isStopped() && !p->isTerminated()) {
            p->pauseProc();
        }
    }
    
    if (changed){
        pdstatusLine("application paused");
    }
    
    return(changed);
}

void processNewTSConnection(int tracesocket_fd) {
   // either a forked process or one created via attach is trying to get a
   // new tracestream connection.  accept() the new connection, then do some
   // processing.  There is no need to restrict this for forked and attached
   // processes --mjrg
   int fd = RPC_getConnect(tracesocket_fd); // accept()
      // will become traceLink of new process
   assert(fd >= 0);

   unsigned cookie;
   //cerr << "processNewTSConnection\n";
   unsigned rRet = 0;
   if (sizeof(cookie) != (rRet=P_recv(fd, &cookie, sizeof(cookie), 0))) {
      cerr << "error, read return: " << rRet << ", cookieSize: "
           << sizeof(cookie) << endl;      
      assert(false);
   }

   //const unsigned cookie_fork   = 0x11111111;
   const unsigned cookie_attach = 0x22222222;

   bool calledFromAttach = (cookie == cookie_attach);

   pdstring str;

   if (calledFromAttach)
   {
      str = "getting new connection from attached process";
   }
   else
   {
#if !defined(i386_unknown_nt4_0)
      str = "getting unexpected process connection";
      pdstatusLine(str.c_str());
#else
      // we expect a 0 cookie value on Windows where CreateProcess
      // is not the same semantics as fork.
#endif // !defined(i386_unknown_nt4_0)
   } 

   int pid;
   if (sizeof(pid) != P_recv(fd, &pid, sizeof(pid), 0))
      assert(false);

   int ppid;
   if (sizeof(ppid) != P_recv(fd, &ppid, sizeof(ppid), 0))
      assert(false);

   key_t theKey;
   if (sizeof(theKey) != P_recv(fd, &theKey, sizeof(theKey), 0))
      assert(false);

   int32_t ptr_size;
   if (sizeof(ptr_size) != P_recv(fd, &ptr_size, sizeof(ptr_size), 0))
      assert(false);

   void *applAttachedAtPtr = NULL;
   char *ptr_dst = (char *)&applAttachedAtPtr;
   if (sizeof(void *) > (uint32_t)ptr_size) {
      // adjust for pointer size mismatch
      ptr_dst += sizeof(void *) - sizeof(int32_t);
   }
   if (ptr_size != P_recv(fd, ptr_dst, ptr_size, 0))
      assert(false);

   pdstatusLine("ready");
}
