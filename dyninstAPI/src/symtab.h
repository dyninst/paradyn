
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

#ifndef SYMTAB_HDR
#define SYMTAB_HDR

extern "C" {
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
}

#include "util/h/Pair.h"
#include "util/h/Vector.h"
#include "util/h/Dictionary.h"
#include "util/h/Object.h"
#include "util/h/list.h"
#include "dyninstAPI/src/dyninst.h"
#include "dyninstAPI/src/arch.h"
#include "dyninstAPI/src/util.h"
#include "util/h/String.h"
#ifndef BPATCH_LIBRARY
#include "paradynd/src/resource.h"
#endif
#include "util/h/Types.h"
#include "util/h/Symbol.h"
#include "dyninstAPI/src/inst.h"

#define RH_SEPERATOR '/'

/*
 * List of supported languages.
 *
 */
typedef enum { langUnknown,
	       langAssembly,
	       langC,
	       langCPlusPlus,
	       langGnuCPlusPlus,
	       langFortran,
	       langCMFortran
	       } supportedLanguages;

 
/* contents of line number field if line is unknown */
#define UNKNOWN_LINE	0

#define TAG_LIB_FUNC	0x1
#define TAG_IO_OUT	0x2
#define TAG_IO_IN       0x4
#define TAG_MSG_SEND	0x8
#define TAG_MSG_RECV    0x10
#define TAG_SYNC_FUNC	0x20
#define TAG_CPU_STATE	0x40	/* does the func block waiting for ext. event */
#define TAG_MSG_FILT    0x80

#define DYN_MODULE "DYN_MODULE"
#define EXTRA_MODULE "EXTRA_MODULE"
#define USER_MODULE "USER_MODULE"
#define LIBRARY_MODULE	"LIBRARY_MODULE"

class image;
class lineTable;
class process;

// if a function needs to be relocated when it's instrumented then we need
// to keep track of new instrumentation points for this function on a per
// process basis (there is no guarentee that two processes are going to
// relocated this function to the same location in the heap)
class relocatedFuncInfo {
public:
    relocatedFuncInfo(process *p,Address na):proc_(p),
	   	      addr_(na),funcEntry_(0),installed_(false){}
    ~relocatedFuncInfo(){proc_ = 0;}
    Address address(){ return addr_;}
    const process *getProcess(){ return proc_;}
    const vector<instPoint*> &funcReturns(){ return funcReturns_;}
    const vector<instPoint*> &funcCallSites(){ return calls_;}
    const instPoint *funcEntry(){ return funcEntry_;}
    void addFuncEntry(instPoint *e){ if(e) funcEntry_ = e; }
    void addFuncReturn(instPoint *r){ if(r) funcReturns_ += r; }
    void addFuncCall(instPoint *c){ if(c) calls_ += c; }
    bool isInstalled(){ return installed_; }
    void setInstalled() { installed_ = true; }
private:
    const process *proc_;		// process assoc. with the relocation
    Address addr_;			// function's relocated address
    instPoint *funcEntry_;		// function entry point
    bool installed_;			// if true, function has been relocated
    vector<instPoint*> funcReturns_;    // return point(s)
    vector<instPoint*> calls_;          // pointer to the calls
};

class pdmodule;
class module;

class function_base {
public:
    function_base(const string symbol, const string &pretty,
		Address adr, const unsigned size,const unsigned tg):
		symTabName_(symbol),prettyName_(pretty),line_(0),
		addr_(adr),size_(size),tag_(tg) { }
    virtual ~function_base() { /* TODO */ }
    const string &symTabName() const { return symTabName_;}
    const string &prettyName() const { return prettyName_;}
    unsigned size() const {return size_;}
    unsigned addr() const {return addr_;}
    unsigned tag() const { return tag_;}
    void setTag(unsigned tg){ tag_ = tg; }

    virtual Address getAddress(const process *p) = 0;
    virtual const instPoint *funcEntry(process *p) const = 0;
    virtual const vector<instPoint*> &funcExits(process *p) const = 0;
    virtual const vector<instPoint*> &funcCalls(process *p) const = 0; 
    virtual bool hasNoStackFrame() const = 0;
       // formerly "isLeafFunc()" but that led to confusion, since people assign two
       // different meanings to "leaf" fns: (1) has no stack frame, (2) makes no calls.
       // By renaming this fn, we make clear that we're returning (1), not (2).

    // extra debuggering info....
    ostream & operator<<(ostream &s) const;
    friend ostream &operator<<(ostream &os, function_base &f);
private:
    string symTabName_;		/* name as it appears in the symbol table */
    string prettyName_;		/* user's view of name (i.e. de-mangled) */
    int line_;			/* first line of function */
    Address addr_;		/* address of the start of the func */
    unsigned size_;             /* the function size, in bytes, used to
				   define the function boundaries. This may not
				   be exact, and may not be used on all 
				   platforms. */
    
    unsigned tag_;
};

class pd_Function : public function_base {
 public:
    pd_Function(const string &symbol, const string &pretty, pdmodule *f, 
		Address adr, const unsigned size, const unsigned tg, 
		const image *owner, bool &err);
    ~pd_Function() { /* TODO */ }

    bool findInstPoints(const image *owner);
    void checkCallPoints();
    bool defineInstPoint();
    pdmodule *file() const { return file_;}
    Address newCallPoint(Address adr, const instruction code, 
			 const image *owner, bool &err);
    // passing in a value of 0 for p will return the original address
    // otherwise, if the process is relocated it will return the new address
    Address getAddress(const process *p){
        if(p && relocatable_) { 
	  for(u_int i=0; i < relocatedByProcess.size(); i++){
	    if((relocatedByProcess[i])->getProcess() == p) 
		return (relocatedByProcess[i])->address();
	} }
	return addr();
    }
    const instPoint *funcEntry(process *p) const {
        if(relocatable_) { 
	  for(u_int i=0; i < relocatedByProcess.size(); i++){
	    if((relocatedByProcess[i])->getProcess() == p) 
		return (relocatedByProcess[i])->funcEntry();
	} }
	return funcEntry_;
    }
    const vector<instPoint*> &funcExits(process *p) const {
        if(relocatable_) {
	  for(u_int i=0; i < relocatedByProcess.size(); i++){
	    if((relocatedByProcess[i])->getProcess() == p) 
		return (relocatedByProcess[i])->funcReturns();
	} }
	return funcReturns;
    }
    const vector<instPoint*> &funcCalls(process *p) const {
        if(relocatable_) {
	  for(u_int i=0; i < relocatedByProcess.size(); i++){
	    if((relocatedByProcess[i])->getProcess() == p) 
		return (relocatedByProcess[i])->funcCallSites();
	} }
	return calls;
    }
    bool isInstalled(process *p){
        if(relocatable_) {
	  for(u_int i=0; i < relocatedByProcess.size(); i++){
	    if((relocatedByProcess[i])->getProcess() == p) 
		return (relocatedByProcess[i])->isInstalled();
	} }
	return false;
    }
    void setInstalled(process *p){
        if(relocatable_) {
	  for(u_int i=0; i < relocatedByProcess.size(); i++){
	    if((relocatedByProcess[i])->getProcess() == p) 
	        (relocatedByProcess[i])->setInstalled();
	} }
    }
    inline void tagAsLib(){ unsigned tg=tag(); tg |= TAG_LIB_FUNC; setTag(tg);}
    inline void untagAsLib() { 
	unsigned t = tag(); 
	t &= ~TAG_LIB_FUNC; 
	setTag(t);
    }
    inline bool isTagSimilar(const unsigned comp) const { 
	unsigned tg = tag();
    	return(tg & comp);
    }
    bool isLibTag() const { return (tag() & TAG_LIB_FUNC);}

    bool hasNoStackFrame() const {return noStackFrame;}
       // formerly "isLeafFunc()" but that led to confusion, since people assign two
       // different meanings to "leaf" fns: (1) has no stack frame, (2) makes no calls.
       // By renaming this fn, we make clear that we're returning (1), not (2).

    bool isTrapFunc() {return isTrap;}

#if defined(sparc_sun_sunos4_1_3) || defined(sparc_sun_solaris2_4)   

    bool checkInstPoints(const image *owner);
    bool findInstPoints(const image *owner, Address adr, process *proc);
    bool findNewInstPoints(const image *owner, 
			  const instPoint *&location, Address adr, 
			  process *proc,
			  vector<instruction> &extra_instrs, 
			  relocatedFuncInfo *reloc_info);
    bool relocateFunction(process *proc, const instPoint *&location,
			  vector<instruction> &extra_instrs);
    // Add a new call point to a function that will be, or currently is
    // being relocated (if location != 0 && reloc_info != 0,  then this is
    // called when the the function is actually being relocated
    Address newCallPoint(Address &adr, const instruction code, const 
                         image *owner, bool &err, int &id, Address &addr,
			 relocatedFuncInfo *reloc_info,
			 const instPoint *&location);
    // modifyInstPoint: change the value of the instPoint if it is wrong: 
    // if this instPoint is from a function that was just relocated, then
    // it may not have the correct address.  This routine finds the correct
    // address for the instPoint
    void modifyInstPoint(const instPoint *&location,process *proc);
#endif

  private:
    pdmodule *file_;		/* pointer to file that defines func. */
    instPoint *funcEntry_;	/* place to instrument entry (often not addr) */
    vector<instPoint*> funcReturns;	/* return point(s). */
    vector<instPoint*> calls;		/* pointer to the calls */

    // these are for relocated functions
    bool relocatable_;		// true if func will be relocated when instr

    bool noStackFrame; // formerly "leaf".  True iff this fn has no stack frame.

    bool isTrap; 		// true if function contains a trap instruct
    vector<relocatedFuncInfo *> relocatedByProcess; // one element per process
};

/* Stores source code to address in text association for modules */
class lineDict {
public:
  lineDict() : lineMap(uiHash) { }
  ~lineDict() { /* TODO */ }
  void setLineAddr (unsigned line, Address addr) { lineMap[line] = addr; }
  inline bool getLineAddr (const unsigned line, Address &adr);

private:
  dictionary_hash<unsigned, Address> lineMap;
};


class module {
public:
    module(){}
    module(supportedLanguages lang, Address adr, string &fullNm,
	   string &fileNm): fileName_(fileNm), fullName_(fullNm), 
		language_(lang), addr_(adr){}
    virtual ~module(){}

    string fileName() const { return fileName_; }
    string fullName() const { return fullName_; }
    supportedLanguages language() const { return language_;}
    Address addr() const { return addr_; }

    virtual function_base *findFunction (const string &name) = 0;
    virtual void define() = 0;    // defines module to paradyn
    virtual vector<function_base *> *getFunctions() = 0;
    virtual vector<function_base *> *getIncludedFunctions() = 0;

private:
    string fileName_;                   // short file 
    string fullName_;                   // full path to file 
    supportedLanguages language_;
    Address addr_;                      // starting address of module
};

class pdmodule: public module {
friend class image;
public:
  pdmodule(supportedLanguages lang, Address adr, string &fullNm,
	   string &fileNm, image *e): module(lang,adr,fullNm,fileNm),
	   exec_(e){
    some_funcs_inited = FALSE;
  }
  ~pdmodule() { /* TODO */ }

  void setLineAddr(unsigned line, Address addr) {
	lines_.setLineAddr(line, addr);}
  bool getLineAddr(unsigned line, Address &addr) { 
       return (lines_.getLineAddr(line, addr)); }

  image *exec() const { return exec_; }
  void mapLines() { }           // line number info is not used now
  void checkAllCallPoints();
  inline void changeLibFlag(const bool setSuppress);
  void define();    // defines module to paradyn
  vector<function_base *> *getFunctions() { return (vector<function_base *>*)&funcs;} 
  vector<function_base *> *getIncludedFunctions();
  function_base *findFunction (const string &name);

private:
  image *exec_;                      // what executable it came from 
  lineDict lines_;
  //  list of all found functions in module....
  vector<pd_Function*> funcs;
  // added as part of exclude support for statically linked objects.
  //  mcheyny, 970928
  //  list of non-excluded found functions in module....
  vector<pd_Function*> some_funcs;
  bool some_funcs_inited;
};


extern bool mdl_get_lib_constraints(vector<string> &);

void print_func_vector_by_pretty_name(string prefix, \
				      vector<function_base *>*funcs);
void print_module_vector_by_short_name(string prefix, \
                                      vector<pdmodule*> *mods);
string getModuleName(string constraint);
string getFunctionName(string constraint);
//used by both sharedobject and pdmodule class....
bool filter_excluded_functions(vector<pd_Function*> all_funcs, \
    vector<pd_Function*>& some_funcs, string module_name);
bool function_is_excluded(pd_Function *func, string module_name);
bool module_is_excluded(pdmodule *module);

/*
 * symbols we need to find from our RTinst library.  This is how we know
 *   were our inst primatives got loaded as well as the data space variables
 *   we use to put counters/timers and inst trampolines.  An array of these
 *   is placed in the image structure.
 *
 */
class internalSym {
public:
  internalSym() { }
  internalSym(const Address adr, const string &nm) : name(nm), addr(adr) { }
  Address getAddr() const { return addr;}

private:
  string name;            /* name as it appears in the symbol table. */
  Address addr;      /* absolute address of the symbol */
};


// modsByFileName
// modsbyFullName
// includedMods
// excludedMods
// allMods
// includedFunctions
// excludedFunctions
// instrumentableFunctions
// notInstruFunctions
// funcsByAddr
// funcsByPretty
// file_
// name_
// codeOffset_
// codeLen_
// dataOffset_
// dataLen_
// linkedFile
// iSymsMap
// allImages
// varsByPretty
// knownJumpTargets
class image {
   friend class process;

   //
   // ****  PUBLIC MEMBER FUBCTIONS  ****
   //
public:
  static image *parseImage(const string file);
  static image *parseImage(const string file,u_int baseAddr);
#ifndef BPATCH_LIBRARY
  static void changeLibFlag(resource*, const bool);
#endif

  image(const string &file, bool &err);
  image(const string &file, u_int baseAddr, bool &err);
  ~image() { /* TODO */ }

  // True if symbol can be found, regardless of whether it is
  // excluded!!!! 
  bool findInternalSymbol(const string &name, const bool warn, internalSym &iSym);
  // Non-NULL if symbol can be found, even if it it is excluded!!!!
  Address findInternalAddress(const string &name, const bool warn, bool &err);

  // find the named module - 
  // mcheyey - Should this work if function is excluded?? 
  pdmodule *findModule(const string &name, bool find_if_excluded = FALSE);

  // find the function by name, address, or the first by name
  // find_if_excluded specifies whether to include "excluded" 
  // functions in search.
  bool findFunction(const string &name, vector<pd_Function*> &flist, \
    bool find_if_excluded = FALSE);
  // find_if_excluded specifies whether to include "excluded" 
  // functions in search.
  pd_Function *findFunction(const Address &addr, \
    bool find_if_excluded = FALSE);

  // return NULL if function is excluded!!!!
  pd_Function *findOneFunction(const string &name);
  // not NULL if function can be found, even if excluded!!!!
  pd_Function *findOneFunctionFromAll(const string &name);

  // non-NULL even if function is excluded!!!!
  pd_Function *findFunctionIn(const Address &addr,const process *p) const;
  // as per findFunctionIn....
  pd_Function *findFunctionInInstAndUnInst(const Address &addr, const process *p) const;

  // report modules to paradyn
  void defineModules();

  bool symbolExists(const string &); /* Does the symbol exist in the image? */
  void postProcess(const string);          /* Load .pif file */


  void addJumpTarget(Address addr) {
    if (!knownJumpTargets.defines(addr)) knownJumpTargets[addr] = addr; 
  }

  bool isJumpTarget(Address addr) { 
    return knownJumpTargets.defines(addr); 
  }


  // data member access
  inline const Word get_instruction(Address adr) const;

  inline const unsigned char *getPtrToInstruction(Address adr) const;

  string file() const {return file_;}
  string name() const { return name_;}
  Address codeOffset() { return codeOffset_;}
  Address dataOffset() { return dataOffset_;}
  Address dataLength() { return (dataLen_ << 2);} 
  Address codeLength() { return (codeLen_ << 2);} 
  inline bool isCode(const Address &where) const;
  inline bool isData(const Address &where) const;
  const Object &getObject() const { return linkedFile; }

  inline bool isValidAddress(const Address &where) const;

  // Return symbol table information
  inline bool symbol_info(const string& symbol_name, Symbol& ret);

  // Called from the mdl -- lists of functions to look for
  static void watch_functions(string& name, vector<string> *vs, bool is_lib,
			      vector<pd_Function*> *updateDict);

  // origionally return mdlNormal;....
  // Note that (unlike name), this returns ONLY functions for which
  // necessary instrumentation info could be found)!!!!
  const vector<pd_Function*> &getAllFunctions();

  const vector<pd_Function*> &getIncludedFunctions();

  // get all modules, including excluded ones....
  const vector<pdmodule *> &getAllModules();
  const vector<pdmodule *> &getIncludedModules();

  //
  //  ****  PUBLIC DATA MEMBERS  ****
  //

 
  // 
  //  **** PRIVATE DATA MEMBERS ****
  //
  private:

  // TODO -- get rid of one of these
  // Note : as of 971001 (mcheyney), these hash tables only 
  //  hold entries in includedMods --> this implies that
  //  it may sometimes be necessary to do a linear sort
  //  through excludedMods if searching for a module which
  //  was excluded....
  dictionary_hash <string, pdmodule *> modsByFileName;
  dictionary_hash <string, pdmodule*> modsByFullName;

  // list of modules which have not been excluded.
  vector<pdmodule *> includedMods;
  // list of excluded module.  includedMods && excludedMods
  //  should be disjoint!!!!
  vector<pdmodule *> excludedMods;
  // list of all modules, should = includedMods + excludedMods;
  // Not actually created until getAllModules called....
  vector<pdmodule *> allMods;

  //
  // Lists of Functions....
  //
  //  ALERT ALERT!!!!  class image used to contain :
  //vector<pd_Function*> mdlLib;       // functions NOT linked between
  //                                   //   DYNINSTstart && DYNINSTend.
  //vector<pd_Function*> mdlNormal;    // functions linked between 
  //                                   //   DYNINSTstart && DYNINSTend.
  //  changed to :
  // list of all functions for which necessary instrumentation data
  //  could be found which are NOT excluded....
  vector<pd_Function*> includedFunctions;
  // hash table of all functions for which necessary instrumentation data
  //  could be found which ARE excluded....
  dictionary_hash <string, pd_Function*> excludedFunctions;
  // Note that notInstruFunctions holds list of functions for which
  //  necessary instrumentation data could NOT be found....

  // includedFunctions + excludedFunctions (but not \
  //  notInstruFunctions)....
  vector<pd_Function*> instrumentableFunctions;

  dictionary_hash <string, pd_Function*> notInstruFunctions;
  // The functions that we are not going to instrument

  //
  // Hash Tables of Functions....
  //
  // functions by address for all modules.  Only contains instrumented
  //  funtions.
  dictionary_hash <Address, pd_Function*> funcsByAddr;
  dictionary_hash <string, vector<pd_Function*>*> funcsByPretty;
  // note, a prettyName is not unique, it may map to a function appearing
  // in several modules.  Also only contains instrumented functions....


  //
  // Private Member Functions for placing pd_Functions in includedFunctions,
  //  excludedFunctions, notInstruFunctions, instrumentableFunctions, 
  //  and the assorted hash tables associated with them....
  //
  // Add a function which was successfully instrumented, and specify 
  //  if excluded (e.g. via mdl "exclude" command).  Sticks it in 
  //  instrumentableFunctions, mod->funcs, and for:
  // excluded:
  //   excludedFunctions, excludedFunctionsByName
  // not excluded:
  //   includedFunctions, funcsByPretty, funcsByAddr
  void addInstruFunction(pd_Function *func, pdmodule *mod, \
        const Address addr, bool excluded);
  // Add a function which could not be instrumented.  Sticks it in
  // notInstruFuncs (list), and notInstruFunctionsByName (hash table).
  void addNotInstruFunc(pd_Function *func);

  //
  //  ****  PRIVATE MEMBERS FUNCTIONS  ****
  //

  // private methods for findind an excluded function by name or
  //  address....
  bool find_excluded_function(const string &name, \
      vector<pd_Function*> &retList);
  pd_Function *find_excluded_function(const Address &addr);

  // merged code from shared_object and static executable constructor
  //  versions....
  void initialize(const string &fileName, bool &err, \
	bool shared_library, u_int baseAddr = 0);

  bool newFunc(pdmodule *, const string &name, const Address addr, 
	       const unsigned size, const unsigned tags, pd_Function *&retFunc);

  void checkAllCallPoints();

  bool addInternalSymbol(const string &str, const Address symValue);

  // creates the module if it does not exist
  pdmodule *getOrCreateModule (const string &modName, const Address modAddr);
  pdmodule *newModule(const string &name, const Address addr);

  bool addOneFunction(vector<Symbol> &mods, pdmodule *lib, pdmodule *dyn,
		      const Symbol &lookUp, pd_Function *&retFunc);

  bool addAllFunctions(vector<Symbol> &mods,
		       pdmodule *lib, pdmodule *dyn,
		       const bool startB, const Address startAddr,
		       const bool endB, const Address endAddr);

  bool addAllSharedObjFunctions(vector<Symbol> &mods,
		       pdmodule *lib, pdmodule *dyn);

  bool addAllVariables();

  // if useLib = true or the functions' tags signify a library function
  // the function is put in the library module
  bool defineFunction(pdmodule *use, const Symbol &sym, const unsigned tags,
		      pd_Function *&retFunc);
  bool defineFunction(pdmodule *lib, const Symbol &sym,
		      const string &modName, const Address modAdr,
		      pd_Function *&retFunc);


  void insert_function_internal_static(vector<Symbol> &mods, \
        Symbol &lookUp, \
        const Address boundary_start,  const Address boundary_end, \
	const Address startAddr, bool startB, const Address endAddr, \
        bool endB, pdmodule *dyn, pdmodule *lib);
  void image::insert_function_internal_dynamic(vector<Symbol>& mods, \
      Symbol &lookUp, \
      pdmodule *dyn, pdmodule *lib, bool is_libdyninstRT);

  //
  //  ****  PRIVATE DATA MEMBERS  ****
  //

  string file_;		/* image file name */
  string name_;		/* filename part of file, no slashes */

  Address codeOffset_;
  unsigned codeLen_;
  Address dataOffset_;
  unsigned dataLen_;

  // data from the symbol table 
  Object linkedFile;

  dictionary_hash <string, internalSym*> iSymsMap;   // internal RTinst symbols

  static vector<image*> allImages;

  dictionary_hash <string, vector<string>*> varsByPretty;
 
  // knownJumpTargets: the addresses in this image that are known to 
  // be targets of jumps. It is used to check points with multiple 
  // instructions.
  // This is a subset of the addresses that are actually targets of jumps.
  dictionary_hash<Address, Address> knownJumpTargets;

};


/*
 * a definition of a library function that we may wish to identify.  This is
 *   how we describe it to the symbol table parser, not how it appears in
 *   the symbol table.  Library functions are placed in a pseudo module 
 *   named LIBRARY_MODULE. 
 *
 */


class libraryFunc {
public:
  libraryFunc(const string n, unsigned t) : name(n), tags(t) { }
  unsigned getTags() const { return tags;}

private:
  string name;
  unsigned tags;
};

#ifndef BPATCH_LIBRARY
// TODO -- remove this
extern resource *moduleRoot;
#endif

inline bool lineDict::getLineAddr (const unsigned line, Address &adr) {
  if (!lineMap.defines(line)) {
    return false;
  } else {
    adr = lineMap[line];
    return true;
  }
}

inline void pdmodule::changeLibFlag(const bool setSuppress) {
  unsigned fsize = funcs.size();
  for (unsigned f=0; f<fsize; f++) {
    if (setSuppress)
      funcs[f]->tagAsLib();
    else
      funcs[f]->untagAsLib();    
  }
}

inline function_base *pdmodule::findFunction (const string &name) {
  unsigned fsize = funcs.size();
  for (unsigned f=0; f<fsize; f++) {
    if (funcs[f]->prettyName() == name)
      return funcs[f];
  }
  return NULL;
}

inline const Word image::get_instruction(Address adr) const{
  // TODO remove assert
  // assert(isValidAddress(adr));
  if(!isValidAddress(adr)){
      // logLine("address not valid in get_instruction\n");
      return 0;
  }

  if (isCode(adr)) {
    adr -= codeOffset_;
    adr >>= 2;
    const Word *inst = linkedFile.code_ptr();
    return (inst[adr]);
  } else if (isData(adr)) {
    adr -= dataOffset_;
    adr >>= 2;
    const Word *inst = linkedFile.data_ptr();
    return (inst[adr]);
  } else {
    abort();
    return 0;
  }
}

// return a pointer to the instruction at address adr
inline const unsigned char *image::getPtrToInstruction(Address adr) const {
  assert(isValidAddress(adr));
  if (isCode(adr)) {
    adr -= codeOffset_;
    const unsigned char *inst = (const unsigned char *)linkedFile.code_ptr();
    return (&inst[adr]);
  } else if (isData(adr)) {
    adr -= dataOffset_;
    const unsigned char *inst = (const unsigned char *)linkedFile.data_ptr();
    return (&inst[adr]);
  } else {
    abort();
    return 0;
  }
}


// Address must be in code or data range since some code may end up
// in the data segment
inline bool image::isValidAddress(const Address &where) const{
  return (isAligned(where) && (isCode(where) || isData(where)));
}

inline bool image::isCode(const Address &where)  const{
  return (linkedFile.code_ptr() && 
	  (where >= codeOffset_) && (where < (codeOffset_+(codeLen_<<2))));
}

inline bool image::isData(const Address &where)  const{
   return (linkedFile.data_ptr() && 
	  (where >= dataOffset_) && (where < (dataOffset_+(dataLen_<<2))));
}

inline bool image::symbol_info(const string& symbol_name, Symbol &ret_sym) {
  if (linkedFile.get_symbol(symbol_name, ret_sym))
    return true;

  if (varsByPretty.defines(symbol_name)) {
    vector<string> *mangledNames = varsByPretty[symbol_name];
    assert(mangledNames && mangledNames->size() == 1);
    if (linkedFile.get_symbol((*mangledNames)[0], ret_sym))
      return true;
  }

  return false;
}

#endif

