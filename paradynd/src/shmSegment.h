/*
 * Copyright (c) 1996-2000 Barton P. Miller
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
// $Id: shmSegment.h,v 1.1 2000/02/22 23:12:14 pcroth Exp $
//----------------------------------------------------------------------------
//
// Declaration of ShmSegment class.
// A ShmSegment object represents a shared memory segment, providing an
// OS-independent interface to management of shared memory segments to 
// paradynd.
//
//----------------------------------------------------------------------------
#ifndef SHMSEGMENT_H
#define SHMSEGMENT_H

#if defined(i386_unknown_nt4_0)
typedef HANDLE  shmid_t;
#else // defined(i386_unknown_nt4_0)
typedef int     shmid_t;   
#endif // defined(i386_unknown_nt4_0)


class ShmSegment
{
private:
    shmid_t seg_id;     // "id" for the segment
    key_t   seg_key;    // key that "names" segment
    unsigned int seg_size;  // size of segment
    void*   seg_addr;   // address at which segment is
                        // attached to current process
    

    ShmSegment( shmid_t id, key_t key, unsigned int size, void* addr )
        : seg_id( id ),
          seg_key( key ),
          seg_size( size ),
          seg_addr( addr )
    {}

public:
    static  ShmSegment* Create( key_t& key, unsigned int size, void* addr = NULL );
    static  ShmSegment* Open( key_t key, unsigned int size, void* addr = NULL );

    ~ShmSegment( void );

    shmid_t GetId( void ) const             { return seg_id; }
    key_t   GetKey( void ) const            { return seg_key; }
    unsigned int    GetSize( void ) const   { return seg_size; }
    void*   GetMappedAddress( void ) const  { return seg_addr; }
};

#endif // SHMSEGMENT_H
