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

//
// UI and tunable constant templates
//

#include "common/h/List.h"
#include "common/h/String.h"
#include "common/h/Vector.h"
#include "common/src/Dictionary.C"

template class pdvector< pdvector<unsigned> >;

/* ******************************
 * TCthread stuff
 */
#include "paradyn/src/TCthread/tunableConst.h"

template class pdvector<tunableBooleanConstant>;
template class pdvector<tunableFloatConstant>;

template class dictionary_hash<pdstring, tunableBooleanConstant>;
template class pdvector<dictionary_hash<pdstring, tunableBooleanConstant>::entry>;

template class dictionary_hash<pdstring, tunableFloatConstant>;
template class pdvector<dictionary_hash<pdstring, tunableFloatConstant>::entry>;

/* *************************************
 * UIthread stuff
 */
#include "VM.thread.h"
#include "../src/UIthread/UIglobals.h"
#include "common/src/List.C"
template class ListWithKey<metricInstInfo *, metricInstanceHandle>;
template class ListBase<metricInstInfo *, metricInstanceHandle>;
template class pdvector<VM_activeVisiInfo>;

template class dictionary_hash<unsigned, pdstring>;
template class pdvector<dictionary_hash<unsigned, pdstring>::entry>;

/* *************************************
 * UIthread Logo Stuff
 */

#include "paradyn/src/UIthread/pdLogo.h"
template class pdvector<pdLogo *>;
template class dictionary_hash<pdstring, pdLogo *>;
template class pdvector<dictionary_hash<pdstring, pdLogo *>::entry>;

template class dictionary_hash<pdstring, pdLogo::logoStruct>;
template class pdvector<dictionary_hash<pdstring, pdLogo::logoStruct>::entry>;
template class pdvector<pdLogo::logoStruct>;

/* *************************************
 * UIthread Misc Stuff
 */

#include "paradyn/src/UIthread/minmax.C"
template int min(const int, const int);
template int max(const int, const int);
template unsigned min(const unsigned, const unsigned);
template unsigned max(const unsigned, const unsigned);
template float min(const float, const float);
template float max(const float, const float);
template void ipmin(int &, const int);
template void ipmax(int &, const int);
