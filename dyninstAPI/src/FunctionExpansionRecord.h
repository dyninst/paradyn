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

#ifndef __FUNCTION_EXPANSION_RECORD_H__
#define __FUNCTION_EXPANSION_RECORD_H__

#include "util/h/headers.h"
#include "util/h/Vector.h"

// small class for keeping track of information about shifts in code offsets
//  (offset from beginning of function) caused when functions are expanded
//  e.g. when entry or exit instrumentation is inserted....
class FERNode {
    //  the offset (number of instructions from beginning of the function) at
    //   which the extra instructions are inserted.  This offset is called
    //   "origional" to exphasze that it is relative to the ORIGIONAL function,
    //   not the REWRITTEN function.
    int origional_offset;
    //  the number of instructions by which offsets in F() between
    //   origional_offset and the origional_offset of the next FERNode are
    //   shifted....
    int shift;
  public:
    int OrigionalOffset() {return origional_offset;}
    int Shift() {return shift;}
    void SetShift(int val) {shift = val;}
    FERNode(int toffset = 0, int tshift = 0);

    ostream &operator<<(ostream &os);
};

class FunctionExpansionRecord {
    // record of info on how instrumenting or otherwise rewriting a function
    //  has expanded logical blocks of it.  This information is needed to patch
    //  jump/branch targets to locations inside the function (e.g. from other
    //  locations inside the function).

    // vector holding FERNodes added with AddExpansion()....
    //  assumes sorted in order of origional_offset....
    vector<FERNode> expansions;
    // vector holding FERNodes representing TOTAL displacements 
    //  derived from individual displacements specified via AddExpansion()....
    vector<FERNode> total_expansions;
    // index into expansions which satisfied last GetShift() request.... 
    int index;

    int collapsed;
  public:
    // add an additional expansion record....
    void AddExpansion(int origional_offset, int shift);
    // compute total expansions for given regions from added expansion
    //  records - use BEFORE GetShift()....
    void Collapse();

    int GetShift(int origional_offset);
    FunctionExpansionRecord();

    // dump state info....
    ostream& operator<<(ostream &os);
};


/*  __FUNCTION_EXPANSION_RECORD_H__  */
#endif
