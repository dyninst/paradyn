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

// $Id: sharedobject.h,v 1.23 2001/08/30 21:31:23 bernat Exp $

#if !defined(_shared_object_h)
#define _shared_object_h

#include "common/h/String.h"
#include "common/h/Types.h"
#include "dyninstAPI/src/symtab.h"
#include "dyninstAPI/src/symtab.h"
#include "dyninstAPI/src/Object.h"
#ifndef BPATCH_LIBRARY
#include "paradynd/src/mdld.h"
#endif

/*
 * A class for link map information about a shared object that is mmapped 
 * by the dynamic linker into the applicaitons address space at runtime. 
 */
#define 	SHAREDOBJECT_NOCHANGE	0
#define 	SHAREDOBJECT_ADDED	1
#define 	SHAREDOBJECT_REMOVED	2

class shared_object {

public:
    shared_object():desc(0),name(0),
      short_name(0),
      base_addr(0),
      processed(false),
      mapped(false),include_funcs(true), 
      objs_image(0),included_funcs(0){}
    shared_object(string &n, Address b, bool p,bool m, bool i, image *d):
      name(n), base_addr(b),
      processed(p),mapped(m),
      include_funcs(i), objs_image(d),included_funcs(0){ 
      desc = new fileDescriptor(n, b);
      set_short_name();
    }
    shared_object(fileDescriptor *f,
		  bool p, bool m, bool i, image *d):
      desc(f),
      name(f->file()), base_addr(0),
      processed(p),mapped(m),
      include_funcs(i), objs_image(d),included_funcs(0){ 
      set_short_name();
    }


    shared_object(const shared_object &s_obj){
      desc = s_obj.desc;
      base_addr = s_obj.base_addr;
      short_name = s_obj.short_name;
      processed = s_obj.processed;
      mapped = s_obj.mapped;
      include_funcs = s_obj.include_funcs;
      objs_image = s_obj.objs_image;
      included_funcs = s_obj.included_funcs;
    }
    ~shared_object(){ objs_image = 0;}

    fileDescriptor *getFileDesc() { return desc; }
    const string &getName(){ return name; }
    const string &getShortName() { return short_name; }
    Address getBaseAddress() { return base_addr; }
    bool  isProcessed() { return(processed); }
    bool  isMapped() { return(mapped); }
    const image  *getImage() { return(objs_image); }
    bool includeFunctions(){ return(include_funcs); }
    void changeIncludeFuncs(bool flag){ include_funcs = flag; } 

    void  unMapped(){ mapped = false; }
    void  setBaseAddress(Address new_ba){ base_addr = new_ba; }

    bool  getSymbolInfo(const string &n,Symbol &info) {
        if(objs_image) {
	    return (objs_image->symbol_info(n,info));
	}
	return false;
    }

    const vector<pd_Function *> *getAllFunctions(){
        if(objs_image) {
	    // previously objs_image->mdlNormal....
	    return (&(objs_image->getAllFunctions()));
	}
	return 0;
    }

    // from a string that is a complete path name to a function in a module
    // (ie. "/usr/lib/libc.so.1/write") return a string with the function
    // part removed.  return 0 on error
    char *getModulePart(string &full_path_name) ;

#ifndef BPATCH_LIBRARY
    // get only the functions not excluded by the mdl options exclude_lib
    // or exclude_funcs
    //    vector<pd_Function *> *getSomeFunctions();
    vector<pd_Function *> *getIncludedFunctions();
#endif

    // Get list of ALL modules, not just included ones.
    const vector<pdmodule *> *getModules() {
        if(objs_image) {
	  return (&(objs_image->getAllModules()));
	}
	return 0;
    }

    bool  addImage(image *i){ 
	if(!processed && (objs_image == 0)) {
	    objs_image = i;
	    processed = true;
            return true;
	}
	else {
            return false;
	}
    }
    bool removeImage(){ return true;}

    pd_Function *findOneFunction(string f_name, 
				 bool check_excluded = false)
      {
	pd_Function *pdf;
	if (f_name.string_of() == 0) return NULL;
        if(objs_image) {
	  if ( (pdf = objs_image->findFuncByName(f_name) ))
	    return pdf;
	  if (check_excluded)
	    if ( (pdf = objs_image->findExcludedFunc(f_name) ))
	      return pdf;
	}
	return NULL;
    }

    pd_Function *findFuncByName(const string &name)
      {
	if (name.string_of() == 0) return NULL;
        if(objs_image) {
	  return (objs_image->findFuncByName(name));
	}
	return NULL;
      }
    

    pdmodule *findModule(string m_name,bool check_excluded)
      {
        if(objs_image) {
	    if(check_excluded && !include_funcs){
		return 0;
	    }
            return (objs_image->findModule(m_name));
	}
	return 0;
    }

    pd_Function *findFuncByAddr(Address adr, const process *p) {
      return(objs_image->findFuncByAddr(adr, p));
    }
      
    pd_Function *findFuncByEntryAddr(Address adr, const process *p) {
      return(objs_image->findFuncByEntryAddr(adr, p));
    }

    pd_Function *findFuncByRelocAddr(Address adr, const process *p) {
      return(objs_image->findFuncByRelocAddr(adr, p));
    }

    pd_Function *findFuncByOrigAddr(Address adr, const process *p) {
      return(objs_image->findFuncByOrigAddr(adr, p));
    }


    //
    //     PRIVATE DATA MEMBERS
    //				
private:
    fileDescriptor *desc; // full file descriptor

    string  name;	// full file name of the shared object
    string  short_name; // name of shared object as it should be identified
			//  in mdl, e.g. as used for "exclude"....
    Address   base_addr;// base address of where the shared object is mapped

    bool    processed;  // if true, daemon has processed the shared obj. file
    bool    mapped;     // if true, the application has the shared obj. mapped
			// shared objects can be unmapped as the appl. runs
    bool include_funcs; // if true include the the functions from this shared
			// object in the set of all instrumentable functions
			// (this is for foci not refined on the Code heirarchy)
			// - Conceptually assumes that shared object has 1
			// and only 1 module.
    image  *objs_image; // pointer to image if processed is true 
    vector<pd_Function *> *included_funcs; // all functions not excluded by 
				       // exclude_func option

    void set_short_name();
};

#endif
