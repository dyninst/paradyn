/*
 * 
 * $Log: PCwhere.h,v $
 * Revision 1.11  1995/07/17 04:29:04  tamches
 * Changed whereAxis to pcWhereAxis, avoiding a naming conflict with the
 * new UI where axis.
 *
 * Revision 1.10  1995/06/02  20:50:19  newhall
 * made code compatable with new DM interface
 *
 * Revision 1.9  1995/02/16  08:19:26  markc
 * Changed Boolean to bool
 *
 * Revision 1.8  1995/01/26  17:58:47  jcargill
 * Changed igen-generated include files to new naming convention; fixed
 * some bugs compiling with gcc-2.6.3.
 *
 * Revision 1.7  1994/10/25  22:08:21  hollings
 * changed print member functions to ostream operators.
 *
 * Fixed lots of small issues related to the cost model for the
 * Cost Model paper.
 *
 * Revision 1.6  1994/09/22  01:09:45  markc
 * Commented out operator = declaration, it wasn't being used, and doesn't
 * look correct.
 *
 * Revision 1.5  1994/07/25  04:47:13  hollings
 * Added histogram to PCmetric so we only use data for minimum interval
 * that all metrics for a current batch of requests has been enabled.
 *
 * added hypothsis to deal with the procedure level data correctly in
 * CPU bound programs.
 *
 * changed inst hypothesis to use observed cost metric not old procedure
 * call based one.
 *
 * Revision 1.4  1994/06/14  15:34:48  markc
 * Modified parameters for moreSpecific.
 * Added MsgTags as a resource.
 *
 * Revision 1.3  1994/02/24  04:36:51  markc
 * Added an upcall to dyninstRPC.I to allow paradynd's to report information at
 * startup.  Added a data member to the class that igen generates.
 * Make depend differences due to new header files that igen produces.
 * Added support to allow asynchronous starts of paradynd's.  The dataManager has
 * an advertised port that new paradynd's can connect to.
 * Changed header files to reflect changes in igen.
 *
 * Revision 1.2  1994/02/03  23:27:06  hollings
 * Changes to work with g++ version 2.5.2.
 *
 * Revision 1.1  1994/02/02  00:38:23  hollings
 * First version of the Performance Consultant using threads.
 *
 * Revision 1.8  1993/08/05  19:00:59  hollings
 * new includes.
 *
 * Revision 1.7  1993/05/07  20:19:17  hollings
 * Upgrade to use dyninst interface.
 *
 * Revision 1.6  1993/03/23  17:17:40  hollings
 * changed testValue's friendship.
 *
 * Revision 1.5  1993/02/03  00:06:49  hollings
 * removed execesive friends of focus and focusCell.
 *
 * Revision 1.4  1992/12/14  19:58:27  hollings
 * added true enable/disable.
 *
 * Revision 1.3  1992/10/23  20:13:37  hollings
 * Working version right after prelim.
 *
 * Revision 1.2  1992/08/24  15:12:20  hollings
 * first cut at automated refinement.
 *
 * Revision 1.1  1992/08/03  20:45:54  hollings
 * Initial revision
 *
 *
 */

#ifndef FOCUS_H
#define FOCUS_H

#include "util/h/list.h"
#include "util/h/stringPool.h"
#include "dataManager.thread.CLNT.h"
// TODO: this should be removed
#include "paradyn/src/DMthread/DMmetric.h"
#include "paradyn/src/DMthread/DMresource.h"
// ******************************

class PCmetric;
class testValue;
class focus;
class searchHistoryNode;
class focusList;

//
// A point along the where axis.
//
class focus {
	friend class testValue;
	friend ostream& operator <<(ostream &os, focus& f);
    public:
	// focus();		
	focus(resourceList*);
	focusList magnify(resource *param);
	focusList enumerateRefinements();
	focus *constrain(resource *param);

	bool operator ==(focus*);	// comparison
	focus *moreSpecific(resource *parm, bool &conflicts);
	/* int operator = (focus); */
	void updateName();
	stringHandle getName() 	{ return(name); }
	bool getSuppressed()	{ return(suppress); }

	resourceList *data;
    private:
	// internal helper methods.
	focusList *findMagnifyCache(resource*);
	void addMagnifyCache(resource*, focusList*);

	focus *findConstrainCache(resource*);
	void addConstrainCache(resource*, focus*);

	stringHandle name;		// for fast compare.
	List<focusList*>		magnifyCache;
	List<focus*>			constrainCache;
	bool suppress;		// don't magnify on me.
};

ostream& operator <<(ostream &os, focus& f);

class focusList: public HTable<focus*> {
    public:
	focusList(focus *f) { focusList(); add(f,f->getName()); }
	focusList() {  HTable<focus*>(); }
	focus *find(focus *f) { 
	    return(HTable<focus*>::find((char *) f->getName())); 
	}
	bool addUnique(focus *f) { 
	    return(HTable<focus*>::addUnique(f, f->getName())); 
	}
};

extern stringPool strSpace;

// current global focus
extern focus *pcWhereAxis;

// current set of foci.
extern focusList globalFocus;

// 
extern resource *rootResource;

// current focus
extern focus *currentFocus;

// all knwon focii.
extern focusList allFoci;

extern resource *Locks;
extern resource *Barriers;
extern resource *Semaphores;
extern resource *MsgTags;

extern resource *SyncObject;
extern resource *Procedures;
extern resource *Processes;
extern resource *Machines;

#endif
