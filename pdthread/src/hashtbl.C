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

#ifndef __libthread_hashtbl_C__
#define __libthread_hashtbl_C__

#define HASHTBL_SIZE 512

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "hash.h"
#include "xplat/Mutex.h"

namespace pdthr
{

/* Basic fixed-size hash table class, protected by a mutex.  Some important
   restrictions: 
    * if key_t is a pointer type, then keys will be hashed based on
      their address, not the contents of said address.  To hash on contents, 
      use the put(key_t,len, value) and get(key_t,len) methods.
    * value_t must be a pointer type.
 */

template<class key_t, class value_t>
struct pair {
    key_t key;
    value_t value;
    pair<key_t,value_t>* next;
    pair<key_t,value_t>* cleanup_next;
};


template<class key_t, class value_t, class mutex_t=XPlat::Mutex>
class hashtbl {
  private:
    static const char* hashtbl_name(const char* kt, const char* vt, const char* c) {
        const char* format = "monitor_for_hashtbl_key_t-%s_value_t-%s_%s";
        int len = strlen(format) + strlen(kt) + strlen(vt) + strlen(c) + 1;
        char* retval = new char[len+1];
        sprintf(retval, format, kt, vt, c);
        return (const char *)retval;
    }


    pair<key_t,value_t>** entries;
    pair<key_t,value_t>* cleanup_list;
    const char* h_name;
    mutex_t mutex;
    int (*compare_func)(const key_t*,const key_t*);


    inline unsigned long 
    hashval(key_t key) { return hash((ub1*)&key, sizeof(key)) % HASHTBL_SIZE; }

    inline unsigned long 
    hashval(key_t *keyp, unsigned len) { return hash((ub1*)keyp, len) %
                                             HASHTBL_SIZE; }

    inline pair<key_t,value_t>* 
    find(pair<key_t,value_t>* start_here, key_t key) {
        while(start_here) {
            if(!start_here || start_here->key == key) break;
            start_here = start_here->next;
        }
        return start_here;
    }

  public:
    hashtbl(const char* key_name="key_t", const char* value_name="value_t", const char* comment="") 
            : h_name(hashtbl_name(key_name, value_name, comment)) {

        entries = new pair<key_t,value_t>* [HASHTBL_SIZE];
        for(int i = 0; i < HASHTBL_SIZE; i++)
            entries[i] = NULL;
        compare_func = NULL;
        cleanup_list = NULL;
        
    }

    hashtbl(int (*comparator)(const key_t*,const key_t*)) {
        entries = new pair<key_t,value_t>* [HASHTBL_SIZE];
        for(int i = 0; i < HASHTBL_SIZE; i++)
            entries[i] = NULL;
        compare_func = comparator;
        cleanup_list = NULL;
    }
    
    ~hashtbl() {
        pair<key_t,value_t> *bucket_to_delete = cleanup_list,
            *next_bucket = NULL;
        
        while(bucket_to_delete) {
            next_bucket = bucket_to_delete->cleanup_next;
            delete bucket_to_delete;
            bucket_to_delete = next_bucket;
        }
        
        delete [] entries;
    }

    void put(key_t key, value_t value) {
        mutex.Lock();
        unsigned long index = hashval(key);
        pair<key_t,value_t> *start_bucket, *result_bucket;
        start_bucket = entries[index];
        result_bucket = NULL;
#ifdef HASH_DEBUG
        fprintf(stderr, "key %d hashes to %d\n", (unsigned)key, index);
#endif
        result_bucket = find(start_bucket, key);
            
        if(!result_bucket) {
            result_bucket = new pair<key_t,value_t>;
            result_bucket->cleanup_next = cleanup_list;
            result_bucket->next = entries[index];
            result_bucket->key = key;
            result_bucket->value = value;
            cleanup_list = result_bucket;
            entries[index] = result_bucket;
        } else {
            result_bucket->value = value;
        }
        mutex.Unlock();
    }

    void put(key_t key, value_t value, unsigned len) {
        mutex.Lock();
        unsigned long index = hashval(key, len);
        pair<key_t,value_t> *start_bucket, *result_bucket;
        start_bucket = entries[index];
        result_bucket = NULL;
#ifdef HASH_DEBUG
        fprintf(stderr, "key %d hashes to %d\n", (unsigned)key, index);
#endif
        result_bucket = find(start_bucket, key);
            
        if(!result_bucket) {
            result_bucket = new pair<key_t,value_t>;
            result_bucket->cleanup_next = cleanup_list;
            result_bucket->next = entries[index];
            result_bucket->key = key;
            result_bucket->value = value;
            cleanup_list = result_bucket;
            entries[index] = result_bucket;
        } else {
            result_bucket->value = value;
        }
        mutex.Unlock();
    }    

    value_t get(key_t key) {
        mutex.Lock();
        
        unsigned long index = hashval(key);
        value_t retval;
        pair<key_t,value_t> *start_bucket, *result_bucket;
        start_bucket = entries[index];
        result_bucket = NULL;
#ifdef HASH_DEBUG
        fprintf(stderr, "key %d hashes to %d\n", (unsigned)key, index);
#endif
        result_bucket = find(start_bucket, key);

        if(result_bucket)
            retval = result_bucket->value;
        else
            retval = (value_t)NULL;
        
        mutex.Unlock();
        return retval;
    }

    value_t get(key_t *key, unsigned len) {
        mutex.Lock();
        
        unsigned long index = hashval(key, len);
        value_t retval;
        pair<key_t,value_t> *start_bucket, *result_bucket;
        start_bucket = entries[index];
        result_bucket = NULL;
#ifdef HASH_DEBUG
        fprintf(stderr, "key %d hashes to %d\n", (unsigned)key, index);
#endif
        result_bucket = find(start_bucket, key);

        if(result_bucket)
            retval = result_bucket->value;
        else
            retval = NULL;
        
        mutex.Unlock();
        return retval;
    }

    template<class Command>
    void map_keys(Command* cmd) {
        mutex.Lock();
        for (int i = 0; i < HASHTBL_SIZE; i++)
            if(entries[i] != NULL) cmd->exec((entries[i])->key);
        mutex.Unlock();
    }

    template<class Command>
    void map_vals(Command* cmd) {
        mutex.Lock();
        for (int i = 0; i < HASHTBL_SIZE; i++)
            if(entries[i] != NULL) cmd->exec((entries[i])->value);        
        mutex.Unlock();
    }    
};

} // namespace pdthr

#endif

