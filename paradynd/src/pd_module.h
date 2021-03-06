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

// $Id: pd_module.h,v

#if !defined(__PD_MODULE__)
#define __PD_MODULE__

#include "common/h/String.h"
#include "common/h/Dictionary.h"

#include "dyninstAPI/h/BPatch_module.h"
#include "dyninstAPI/h/BPatch_basicBlockLoop.h"
#include "dyninstAPI/h/BPatch_loopTreeNode.h"
 
//class pdmodule;  // pdmodule is actually a dynisnt module, despite the name
//class process;
//class pd_process;

#include "paradynd/src/pd_process.h"
#include "paradynd/src/resource.h"

class pd_module {
   BPatch_module *dyn_module;
   BPatch_Vector< BPatch_function * > some_funcs; // included functions
   BPatch_Vector< BPatch_function * > *all_funcs; // inc + excluded functions

   pdstring _name;
   bool is_excluded;
   resource *mod_resource;

   static dictionary_hash<BPatch_function *, resource *> *func_resources;
   static dictionary_hash<BPatch_basicBlockLoop *, resource *> *loop_resources;

 protected:
   void createResources();
   void createLoopResources(BPatch_loopTreeNode *loop_tree, resource *parent);

   void FillInCallGraphNodeNested(pdstring exe_name, pdstring func_name,
                                  pdstring parent_name, 
                                  BPatch_loopTreeNode *node,
                                  BPatch_process *proc);

 public:

   //void FillInCallGraphStatic(pd_process *proc, bool init_graph , unsigned *checksum);

   pd_module(BPatch_module *dmod);
   ~pd_module();

   BPatch_module *get_dyn_module() const { return dyn_module; }
   resource *getResource();
   
   pdstring fileName() const {return _name;}

   void FillInCallGraphStatic(pd_process *proc, bool init_graph = false,unsigned *checksum = 0 );

   BPatch_Vector<BPatch_function *> *getIncludedFunctions() { return &some_funcs;}
   bool isExcluded() {return is_excluded;}

   resource *getModuleResource() { return mod_resource; }

   static resource *getFunctionResource(BPatch_function *f);
   static resource *getLoopResource(BPatch_basicBlockLoop *l);
};



#endif


