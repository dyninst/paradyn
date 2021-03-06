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

#ifndef __libthread_thr_mailbox_h__
#define __libthread_thr_mailbox_h__

#include "pdthread/h/thread.h"
#include "message.h"
#include "mailbox.h"
#include "refarray.C"
#include "dllist.C"
#include "predicate.h"
#include "xplat/Monitor.h"


namespace pdthr
{

class WaitSet;

class thr_mailbox : public mailbox
{
	friend class SocketWaitSetPopulator;
	friend class FileWaitSetPopulator;
	friend class SocketReadySetPopulator;
	friend class FileReadySetPopulator;

private:
    // sets of bound sockets, ready-to-read sockets, and a mutex 
    // for access control
    dllist<PdSocket,XPlat::Monitor>* bound_socks;
    dllist<PdSocket,XPlat::Monitor>* ready_socks;
    XPlat::Mutex sockq_mutex;

    // sets of bound files, ready-to-read files, and a mutex 
    // for access control
    dllist<PdFile,XPlat::Monitor>* bound_files;
    dllist<PdFile,XPlat::Monitor>* ready_files;
    XPlat::Mutex fileq_mutex;
    
    dllist<message*,XPlat::Monitor>* messages;
    dllist<message*,XPlat::Monitor>* sock_messages;
    dllist<message*,XPlat::Monitor>* file_messages;
    
    bool check_for(thread_t* sender, tag_t* type,
                          bool do_block = false, bool do_yank = false,
                          message** m = NULL, unsigned io_first = 1,
		          pollcallback_t pcb=NULL);
	void wait_for_input( pollcallback_t pcb );

	bool is_buffered_special_ready( thread_t* sender, tag_t* type );

protected:
    virtual void populate_wait_set( WaitSet* wset );
    virtual void handle_wait_set_input( WaitSet* wset );

	virtual void raise_msg_avail( void ) = 0;
	virtual void clear_msg_avail( void ) = 0;
    
public:
    thr_mailbox(thread_t owner);
    virtual ~thr_mailbox();

    /* put() delivers message m into this mailbox */
    virtual int put(message* m);

    /* put_sock() delivers socket message m into this mailbox */
    virtual int put_sock(message* m);

    /* put_file() delivers file message m into this mailbox */
    virtual int put_file(message* m);

    /* recv() receives mail from this mailbox into buf;
       see the libthread documentation for the full
       meanings of the args */
    virtual int recv(thread_t* sender, tag_t* tagp, void* buf, unsigned* countp);

    // same as the above recv, but transfers ownership of
    // the buffer to the receiver
    virtual int recv(thread_t* sender, tag_t* tagp, void** buf);

    /* poll() checks for suitable available messages */
    virtual int poll(thread_t* from,
		     tag_t* tagp,
		     unsigned block,
		     unsigned fd_first=0,
		     pollcallback_t pcb=NULL);

    void bind( PdFile fd,
                    unsigned special,
                    int (*wb)(void*), void* arg,
                    thread_t* ptid);
    void unbind( PdFile fd, bool getlock = true );
    bool is_bound( PdFile fd );

    void bind( PdSocket s,
                    unsigned special,
                    int (*wb)(void*),
                    void* arg,
                    thread_t* ptid);
    void unbind( PdSocket s, bool getlock = true );
    bool is_bound( PdSocket s);

    void clear_ready( PdSocket sock );
    void clear_ready( PdFile fd );
    
    virtual void dump_state();
};

} // namespace pdthr

#endif


