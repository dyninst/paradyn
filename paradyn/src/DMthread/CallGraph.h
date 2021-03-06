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

// $Id: CallGraph.h,v 1.14 2006/01/06 23:11:10 legendre Exp $

/**********************************************************
 *
 * CallGraph.h - the call graph class holds information about a process's
 *  call graph - the relationship defined by which functions (can ever)
 *  call eachother.
 * The call graph maintains information describing this relationship
 *  in paradyn (Note that this information is also stored in a 
 *  less centralized form in paradynd) - for performance reasons -
 *  to prevent the need for a query to paradynd on every code 
 *  hierarchy refinement.
 * The call graph is much like the resource hierarchies - it sits in
 *  the paradyn process, and services magnify requests (coming
 *  from the perf. con., and possibly other sources).  That leads to
 *  the obvious question - why not represent the call graph as a 
 *  resource hierarchy - unfortunately, the assumption that a focus
 *  is a single selection from ALL hierarchies, as opposed to a subset 
 *  of hierarchies, is widespread in the paradyn project and code.
 * The Code Hierarchy and the call graph hierarchy refer to the same underlying
 *  resources.  Thus, it doesn't make sense to have a focus with a selection
 *  from both the code hierarchy and the call graph.  
 * Given the estimated pain-in-the-ass factor of converting paradyn to
 *  understand a focus as a selection from a subset of the hierarchies,
 *  it seems easier to just hack in a CallGraph....
 *
 *********************************************************/

#include "common/h/Vector.h"
#include "common/h/Dictionary.h"
#include "DMinclude.h"
#include "DMresource.h"

class resource;
class CallGraph;

class CallGraph {
  // for each resource registered w/ call graph, list of children
    //  in call graph.... 
    dictionary_hash <const resource *, pdvector<const resource *> > children;
 
    // for each resource registered w/ call graph, list of parents
    //  in call graph....
    dictionary_hash <const resource *, pdvector<const resource *> > parents;
    
    //used to avoid revisiting nodes that we have already visited when
    //traversing the call graph in displayCallGraph()
    dictionary_hash <const resource *, int> visited;

    //Keeps track of those functions (that contain dynamic call sites)
    //for which we have already issued requests to the daemon to
    //instrument those dynamic call sites
    dictionary_hash<const resource *, int> instrumented_call_sites;
    
    //Vector holding all of those functions that contain dynamic call sites
    pdvector<const resource *> dynamic_call_sites;

    dictionary_hash<unsigned, resourceHandle> tid_to_start_func;

    // pointer to root resource for code hierarchy....  This is the resource 
    //  with which searches on a call graph should start.
    // Currently, "/Code" resource
    static resource *rootResource;

    // resource describing the place in the code hierarchy to actually start
    //  a call graph search.
    // Typically, "/Code/some_module/main" function....
    resource *entryFunction;

    //Name of executable as it appears in call graph display window
    pdstring executableName;
    
    //Unique name of executable used to construct the call graph
    pdstring executableAndPathName;

    //program_id- identifies call graph to user interface
    int program_id;
    
    // holds known call graphs, indexed by program id....
    static dictionary_hash<int, CallGraph *> directory;

    void addChildrenToDisplay(const resource *parent, 
			      dictionary_hash <const resource *, int>);

    //Used for issuing new program ids to new call graphs
    static int last_id_issued;

 public:
    bool callGraphInitialized;
    bool callGraphDisplayed;
    CallGraph(pdstring exe_name);
    CallGraph(pdstring exe_name, resource *nroot);

    // Destructor for call graph.  DO NOT DELETE COMPONENT RESOURCES!!!!
    ~CallGraph();

    static void AddProgram(pdstring exe_name);

    // Specify the entry function for a given call graph - the place at
    //  which searches of the call graph should start - generally  
    //  either the code entry point (the place where the code starts 
    //  executing), or the first program specific function (usually "main")
    //  depending on where searches should start.... 
    void SetEntryFunc(resource *r, unsigned tid);  // tid = 0 : initial thread

    // Return boolean value indicating whether was added (not added if resource
    //  was previously seen)....
    bool AddResource(const resource *r);

    // Return integer value indicating how many of the children were
    //  added....
    // assert(r previously seen by call graph).....
    // registers resources in <children> not previously seen....
    int SetChildren(resource *r, const pdvector <resource *>children);

    int AddChild(const resource *parent, const resource *child);

    bool isStartFunction(resourceHandle rh);

    // Add DYNAMICALLY DETERMINED child resource.
    // assert(r previously known to call graph).
    // assert(c previously known to call graph).
    // returns false if c was previously known as child (and c not added),
    //  true otherwise (and c added)....
    bool AddDynamicallyDeterminedChild(const resource *r, const resource *c);

    //Used to notify the call graph of the functions that contain
    //dynamic call sites.
    void AddDynamicCallSite(const resource *parent);

    //Used to tell the call graph that we are done adding nodes,
    //so the call graph can create the display if requested.
    void CallGraphFillDone();

    // Equivalent to resource::getChildren - called by the magnify manager
    //  to magnify a resource - but magnify leading to different children
    //  than in the resource hierarchies - thus leading to different views
    //  of resource hierarchies for things like specialized searches
    //  (e.g. Performance Consultant searches).
    // See above for discussion of why alternate resource hierarchies
    //  were not used....
    pdvector<const resource*> getChildren(const resource *rh);

    // Call Graph class also holds static directory of call graphs indexed
    //  by program id.  This function should return a pointer to the call
    //  graph representing program <program>, or create a new one if
    //  one has not previously been registered for <program>....
    static CallGraph *GetCallGraph(pdstring exe_name);

    // as GetCallGraph, but does not create new one if matching item not found.
    static CallGraph *FindCallGraph(pdstring exe_name);

    //Temporary. This call is used when the PC wants to find the children
    // of a particular call graph node. This call should be replaced
    //with the other "FindCallGraph" call above, but the PC will have to 
    //be aware of which call graph it is requesting the children of.
    static CallGraph *FindCallGraph();

    //Builds the call graph display once all of the static functions have been
    //registered, and the user selects the callGraph menu item from the
    //main paradyn display
    void displayCallGraph();

    resource *getThreadStartFunc(unsigned tid) {
       assert(tid_to_start_func.defines(tid));
       resourceHandle rh = tid_to_start_func[tid];
       return resource::handle_to_resource(rh);
    }
    
    //Calls displayCallGraph() for each of the known call graphs.
    static void displayAllCallGraphs();

    //Saves call graph to a file
    static void saveAllCallGraphsToFile(std::ofstream&);
    void saveCallGraphToFile(std::ofstream&);
    void saveChildrenToFile(const resource *parent,
                              dictionary_hash <const resource *, int>,
                              dictionary_hash <const resource *, int>&,
                              dictionary_hash <const resource *, int>&,
                              std::ofstream&,
                              int);

    //requires an exe_name WITH the full path name of the executable
    //attached.
    static int name2id(pdstring exe_name);

    pdstring getName(){ return executableName;} 
    
    pdstring getExeAndPathName(){ return executableAndPathName;}

    int getId() const {return program_id;}

    static void determineAllDynamicCallees();
    void determineDynamicCallees();

    //Calls isDescendant() for each call graph that we know about
    //(There may be more than one call graph, however this is 
    //currently unimplemented).
    static bool isDescendantOfAny(const resource *child, const resource *parent);

    //Returns true if resource child is a descendent of resource
    //parent in the call graph.
    bool isDescendant(const resource *child, const resource *parent, 
		      dictionary_hash<const resource*, int>& have_visited);
};






