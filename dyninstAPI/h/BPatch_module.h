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

#ifndef _BPatch_module_h_
#define _BPatch_module_h_
#include "BPatch_dll.h"
#include <BPatch_Vector.h>
#include <BPatch_sourceObj.h>

class pdmodule;
class process;
class BPatch_image;
class BPatch_function;
class BPatch_typeCollection;
class BPatch_builtInTypeCollection;
class LineInformation;

extern BPatch_builtInTypeCollection * builtInTypes;


class BPATCH_DLL_EXPORT BPatch_module: public BPatch_sourceObj {

    friend class process;
    //friend class BPatch_function;
    //friend class BPatch_image;
    //friend class BPatch_thread;
    friend class BPatch_flowGraph;
    friend class InstrucIter;

    process		*proc;
    pdmodule		*mod;
    BPatch_image	*img;
    BPatch_Vector<BPatch_function *> * BPfuncs;
    BPatch_Vector<BPatch_function *> * BPfuncs_uninstrumentable;

    bool nativeCompiler;
     
public:
    // these 3 are used by parseStab, current_func is also used by parseCoff
    //static char * current_func_name;
    //static char * current_mangled_func_name; 
    //static BPatch_function *current_func; 

    pdmodule * getModule() { return mod; }

// The following functions are for internal use by  the library only:
    BPatch_module(process *_proc, pdmodule *_mod, BPatch_image *img);
    BPatch_module() : mod(NULL), img(NULL), BPfuncs(NULL),nativeCompiler(false) {
	_srcType = BPatch_sourceModule;
    };

    virtual ~BPatch_module();

    bool getSourceObj(BPatch_Vector<BPatch_sourceObj *>&);
    BPatch_sourceObj *getObjParent();
    bool getVariables(BPatch_Vector<BPatch_variableExpr *> &);

    BPatch_typeCollection *moduleTypes;

// End functions for internal use only
  
    char *getName(char *buffer, int length);
    char *getFullName(char *buffer, int length);
    const char *libraryName();

    size_t getAddressWidth();

    BPatch_Vector<BPatch_function *> *getProcedures(bool incUninstrumentable = false);
    BPatch_Vector<BPatch_function *> *findFunction(const char *name,
						   BPatch_Vector<BPatch_function *> & funcs,
						   bool notify_on_failure=true,
						   bool regex_case_sensitive=true,
						   bool incUninstrumentable = false);
    // FIXME: This (undocumented) method only works for function entry addresses.
    BPatch_Vector<BPatch_function *> *findFunctionByAddress(void *addr,
							    BPatch_Vector<BPatch_function *> &funcs,
							    bool notify_on_failure = true,
							    bool incUninstrumentable = false);

    BPatch_function *findFunctionByMangled(const char * mangled_name, 
					   bool incUninstrumentable = false);

    void dumpMangled(char * prefix);
    void parseTypes();

    bool isSharedLib() const;

    inline bool isNativeCompiler() const { return nativeCompiler; }

    char *parseStabStringSymbol(int line, char *stabstr, void *stabptr);

    //function to get addresses for a line of the module
    bool getLineToAddr(unsigned short lineNo,
		       BPatch_Vector<unsigned long>& buffer,
		       bool exactMatch = true);

    LineInformation* getLineInformation();

#ifdef IBM_BPATCH_COMPAT
    bool getLineNumbers(unsigned int &start, unsigned int &end);
    bool getAddressRange(void * &start, void * &end) {return false;} // DPCL only uses for debug output.
    char *getUniqueString(char *buffer, int length);

    char *sharedLibraryName(char *buffer, int length) { getFullName(buffer, length); return buffer;}
    char *getSharedLibName(char *buffer, int length) { getFullName(buffer, length); return buffer;}
    int getSharedLibType();
    int getBindingType();
#endif

    void setDefaultNamespacePrefix(char *name);

private:

    // In particular, we understand the type information
    // in both DWARF and STABS format.
    void parseStabTypes();
    void parseDwarfTypes();

};

#ifdef IBM_BPATCH_COMPAT
#define	BPatch_sharedPublic	1
#define BPatch_sharedPrivate	2
#define BPatch_nonShared	3

#define BPatch_static		1
#define BPatch_dynamic		2

#endif

#endif /* _BPatch_module_h_ */
