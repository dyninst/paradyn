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

// $Id: Dictionary.h,v 1.19 1998/08/16 23:27:53 wylie Exp $

#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

#ifdef external_templates
#pragma interface
#endif

#if defined(sparc_sun_sunos4_1_3)
#include <string.h>
#endif

#include "util/h/Vector.h"

/************************************************************************
 * template<class K, class V> class dictionary_hash
 * Ari Tamches
 *
 * See Stroustrup, The C++ Programming Language, 3d edition, which
 * provided inspiration for this hash table class via an extended
 * example in section 17.6
 *
 * This class tries very hard to ensure that there are enough bins s.t.
 * hash collisions are few (though if you provide a bad hash function,
 * all bets are off, as usual).  Toward that end, the following invariant
 * is maintained at all times:
 *         #bins * max_bin_load >= total # items
 * max_bin_load is a user-settable parameter (see ctor).
 *
 * When adding a new hash table item would break the invariant,
 * #bins is multiplied by bin_grow_factor (another user-settable parameter in ctor),
 * and everything is rehashed (expensive, but not needed often, since growth
 * is exponential).
 * 
************************************************************************/

template<class K, class V> class dictionary_hash_iter;

template<class K, class V>
class dictionary_hash {
   friend class dictionary_hash_iter<K,V>;
 public:
   dictionary_hash (unsigned (*hashfunc)(const K &),
                   unsigned nbins=101,
                   float max_bin_load=0.7,
                      // we keep #bins*max_bin_load >= total # items, to make
                      // sure that there are enough bins s.t. (assuming a good
                      // hash function) collisions are few.
                   float bin_grow_factor = 1.6
                      // when we need more bins, we grow by this factor
                      // i.e., new #bins = old #bins * bin_grow_factor
                  );
  ~dictionary_hash () {}

   // I'd rather not have provided these methods, but apparantly some code
   // in paradynd actually copies entire dictionaries (!)
   dictionary_hash (const dictionary_hash<K,V> &);
   dictionary_hash<K,V>&  operator= (const dictionary_hash<K,V> &);

   unsigned                    size ()                       const;
   V&                    operator[] (const K &);
   bool                        find (const K &, V &)         const;
   bool                     defines (const K &)              const;
   void                       undef (const K &);
   vector<K>                 keys ()                         const;
   vector<V>                 values ()                       const;
   void                      clear ();

 private:
   unsigned locate_addIfNotFound(const K &);
      // returns ndx within all_elems[]; never returns UINT_MAX

   unsigned locate(const K &, bool even_if_removed) const;
      // never adds a new entry if not found.  Okay to call from const member fns

   void grow_numbins(unsigned new_numbins);

   // We keep a ptr to the hash function:
   unsigned (*hasher)(const K &);

   // Important structure:
   struct entry {
      K key;
      unsigned key_hashval; // we could always rehash, since we have 'key', but this
                            // is faster.
      V val;
      bool removed; // since removal would require a lot of shifting w/in all_elems[],
                    // we use this flag instead, and only do the actual removal on
                    // resize.
      unsigned next; // an index into 'all_elems', for hash collisions.  UINT_MAX
                     // implies end of collision chain for this bin

      entry() {} // needed by vector class
      entry(const K &ikey, unsigned ikey_hashval, const V &ival, unsigned inext) :
                        key(ikey), key_hashval(ikey_hashval), val(ival), next(inext) {
         removed = false;
      }
      entry(const entry &src) : key(src.key), val(src.val) {
         key_hashval = src.key_hashval;
         removed = src.removed;
         next = src.next;
      }
      entry& operator=(const entry &src) {
         if (&src == this) return *this;
         
         key = src.key;
         key_hashval = src.key_hashval;
         val = src.val;
         removed = src.removed;
         next = src.next;
         return *this;
      }
   };

   // The actual elements, in one big vector (certain operations, such as rehashing
   // the entire dictionary) are made more efficient this way, and it's a clean
   // approach to storage).  Since we use vector<>::operator+= on this, we're glad that
   // class vector<> preallocates some extra stuff when growing.
   vector<entry> all_elems;

   // The bins.  Note: since the number of bins doesn't change often (only when
   // growing), we don't really want the extra-buffer storage that vector<>
   // preallocates when using resize().
   // Oh well.
   vector<unsigned> bins;
      // each entry in 'bins' gives the index (w/in 'all_elems') of the first element
      // in the bin.  In other words, the first element in bin X is all_elems[bin[X]].
      // If bin[X] is UINT_MAX, then the bin is empty.

   unsigned num_removed_elems;
   
   // Notes:
   // 1) At any given time, all_elems.size()-num_removed_items gives us the
   //    total # of items in the dictionary.
   // 2) At any given time, bins.size() gives us the total # of bins in the dictionary.
   // 3) We keep the invariant #bins * max_bin_load >= total # items,
   //    incrementing #bins (by a factor of bin_grow_factor) if needed to maintain it.

   float max_bin_load, bin_grow_factor;
};

// Typical way to use an iterator:
// for (dictionary_hash_iter iter=thedict; iter; iter++) {
//    ... make use of iter.currkey() and iter.currval() ...
// }
template<class K, class V>
class dictionary_hash_iter {
 private:
   bool done() const {return (curr == last);} // works if both are NULL (empty iter)
   void movetonext() {
      do {
         curr++;
      } while (curr != last && curr->removed);
   }
   
 public:
   dictionary_hash_iter(const dictionary_hash<K,V> &idict) : dict(idict) {
      reset();
   }

   bool next(K &k, V &v) {
      for (; curr != last; curr++) {
         if (!curr->removed) {
            k = curr->key;
            v = curr->val;
            curr++;
            return true;
         }
      }
      return false;
   }

   void reset() {
      // start the iterator off at the first non-removed item now:
      if (dict.all_elems.size() == 0) {
         curr = last = NULL;
      }
      else {
         curr = &dict.all_elems[0];
         last = curr + dict.all_elems.size(); // 1 past the last element

         while (curr < last && curr->removed)
            curr++;
      }
   }

   operator bool() const {return curr < last;} // correct if both NULL

   void operator++(int) { movetonext(); }

   const K &currkey() const {
      assert(!curr->removed);
      return curr->key;
   }
   const V &currval() const {
      assert(!curr->removed);
      return curr->val;
   }

   dictionary_hash_iter(const dictionary_hash_iter &src) : dict(src.dict) {
      curr = src.curr;
      last = src.last;
   }

 private:
   // private to make sure they're not used:
   dictionary_hash_iter &operator=(const dictionary_hash_iter &);

   const dictionary_hash<K,V> &dict;
   const dictionary_hash<K,V>::entry *curr;
   const dictionary_hash<K,V>::entry *last; // one past the last elem, a la STL style
};

#endif
