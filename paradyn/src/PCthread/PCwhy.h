/*
 * 
 * $Log: PCwhy.h,v $
 * Revision 1.1  1994/02/02 00:38:25  hollings
 * First version of the Performance Consultant using threads.
 *
 * Revision 1.7  1993/08/11  18:52:50  hollings
 * added cost function
 *
 * Revision 1.6  1993/08/05  19:01:26  hollings
 * new includes.
 *
 * Revision 1.5  1993/05/07  20:19:17  hollings
 * Upgrade to use dyninst interface.
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

#ifndef OBJECTS_H
#define OBJECTS_H

#include "util/h/list.h"
#include "PCmetric.h"

// forward ref.
class testValue;

typedef Boolean (*changeCollectionFunc)(collectMode);
typedef void (*evalFunc)(testValue*); 
enum statusType { Enabled, Disabled };

class test {
    public:
	test(changeCollectionFunc, evalFunc, char *);
	changeCollectionFunc changeCollection;
	collect(PCmetricList);	// make sure data is current
	evalFunc evaluate;
	disable(PCmetricList);	// tell inst. we are done w data
	void print();
	char *name;		// name of the test.
};

class testList: public List<test*> {
    public:
	void addUnique(test *t) { 
	    addUnique(t, t->name); 
	}
	test *find(char *key) { 
	    return(List<test*>::find((void *)key)); 
	}
	test *find(test *key) { 
	    return(List<test*>::find((void *) key->name)); 
	}
};

extern class searchHistoryNode;
typedef void (*explanationFunction)(searchHistoryNode*);

class hypothesisList;		// forward decl.

class hypothesis {
	friend Boolean checkPreconditions(hypothesis*, focus*, timeInterval*);
	friend Boolean checkIfTrue(hypothesis*, focus*, timeInterval*);
    public:
	hypothesis(hypothesis*, test*, char *);
	hypothesis(hypothesis*, test*, char *, explanationFunction);
	void unLabel();
	float cost();
	void print(int);		// print a hypothesis

	char *name;
	hypothesisList *children;
	hypothesis *preCondition;
	test *testObject;
	Boolean labeled;		// used by buildTestResultForHypothesis
	explanationFunction explanation;
};

class hypothesisList: public List<hypothesis*> {
    public:
	void add(hypothesis *t) { 
	    List<hypothesis*>::add(t, t->name); 
	}
	hypothesis *find(char *key) { 
	    return(List<hypothesis*>::find(key)); 
	}
	hypothesis *find(hypothesis *key) { 
	    return(List<hypothesis*>::find(key->name)); 
	}
};

extern hypothesis whyAxis;
extern hypothesisList allHypotheses;

extern testList allTests;

#endif
