// PriorityQueue.h

/*
 * $Log: PriorityQueue.h,v $
 * Revision 1.3  1995/12/10 01:03:54  tamches
 * added some comments
 *
 * Revision 1.2  1995/12/08 21:30:26  tamches
 * added printing (for debugging purposes)
 *
 * Revision 1.1  1995/12/08 04:55:29  tamches
 * first version of priority queue class
 *
 */

// Note: in this priority queue class, the item with the _smallest_ key
//       will be the first item.

// class KEY must have the following defined:
// -- operators < and >  (very important; determines the heap ordering)
// -- the usual copy constructors and assignment (single-=) operators
//    that any well-written class will have
// -- operator<< for writing to an ostream&
//
// class DATA must have the following defined:
// -- same as above except no < or > are needed

#ifndef _PRIORITY_QUEUE_H_
#define _PRIORITY_QUEUE_H_

#include <iostream.h>
#include "util/h/Vector.h"

template <class KEY, class DATA>
class PriorityQueue {
 private:
   struct pair {
      KEY theKey;
      DATA theData;
      pair(const KEY &k, const DATA &d) : theKey(k), theData(d) {}
      pair() {} // required by Vector.h
      bool operator<(const pair &other) const {return theKey < other.theKey;}
      bool operator>(const pair &other) const {return theKey > other.theKey;}
      ostream &operator<<(ostream &os) const {
         os << theKey << ',' << theData;
         return os;
      }
   };

   vector<pair> data;

 private:
   static int cmpfunc(const void *ptr1, const void *ptr2);
      // for sorting w/in operator<<()

   void swap_items(unsigned index1, unsigned index2);

   void reheapify_with_parent(unsigned index);
      // item w/ given index may not be in proper heap order; make it so
      // in time O(log(size())) by moving the item upwards as far as necessary

   void reheapify_with_children(unsigned index);
      // item w/ given index may not be in proper heap order; make it so
      // in time O(log(size())) by moving the item downwards as far as necessary

 public:
   PriorityQueue();
   PriorityQueue(const PriorityQueue &src);
  ~PriorityQueue();

   PriorityQueue &operator=(const PriorityQueue &src);

   unsigned size() const; // returns # items in the queue
   bool empty() const; // returns true iff empty queue

   void add(const KEY &, const DATA &);

   // Peek at the first elem in the heap (bomb if heap is empty):
   const KEY  &peek_first_key()  const;
   const DATA &peek_first_data() const;
   
   void delete_first(); // bombs if empty

   ostream &operator<<(ostream &os) const; // just for debugging, nach...

   friend ostream &operator<<(ostream &os, PriorityQueue<KEY, DATA> &q);
};

#endif
