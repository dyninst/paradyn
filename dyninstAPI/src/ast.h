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

// $Id: ast.h,v 1.37 1999/07/07 16:03:28 zhichen Exp $

#ifndef AST_HDR
#define AST_HDR

//
// Define a AST class for use in generating primitive and pred calls
//

#include <stdio.h>
#include "util/h/Vector.h"
#include "util/h/Dictionary.h"
#include "util/h/String.h"
#include "util/h/Types.h"
#include "dyninstAPI/h/BPatch_type.h"

class process;
class instPoint;
class function_base;


// a register number, e.g. [0,31]
// typedef int reg; // see new Register type in "util/h/Types.h"

typedef enum { plusOp,
               minusOp,
               timesOp,
               divOp,
               lessOp,
               leOp,
               greaterOp,
               geOp,
               eqOp,
               neOp,
               loadOp,           
               loadConstOp,
	       loadFrameRelativeOp,
               storeOp,
               ifOp,
	       whileOp,  // Simple control structures will be useful
	       doOp,     // Zhichen
	       callOp,
	       trampPreamble,
	       trampTrailer,
	       noOp,
	       orOp,
	       andOp,
	       getRetValOp,
	       getSysRetValOp,
	       getParamOp,
	       getSysParamOp,	   
	       loadIndirOp,
	       storeIndirOp,
	       saveRegOp,
	       updateCostOp,
	       funcJumpOp,        // Jump to function without linkage
	       branchOp} opCode;

class registerSlot {
 public:
    Register number;    // what register is it
    bool inUse;      	// free or in use.
    bool needsSaving;	// been used since last rest
    bool mustRestore;   // need to restore it before we are done.		
    bool startsLive;	// starts life as a live register.
};

class registerSpace {
    public:
	registerSpace(const unsigned int dCount, Register *deads,
                      const unsigned int lCount, Register *lives);
	Register allocateRegister(char *insn, Address &base, bool noCost);
	void freeRegister(Register k);
	void resetSpace();
	bool isFreeRegister(Register k);
	u_int getRegisterCount() { return numRegisters; }
	registerSlot *getRegSlot(Register k) { return (&registers[k]); }
	bool readOnlyRegister(Register k);
        void keep_register(Register k);
        void unkeep_register(Register k);
        bool is_keep_register(Register k);
    private:
	u_int numRegisters;
	Register highWaterRegister;
	registerSlot *registers;
        vector<Register> keep_list;
};

class AstNode {
    public:
        enum nodeType { sequenceNode, opCodeNode, operandNode, callNode };
        enum operandType { Constant, ConstantPtr, ConstantString,
			   DataValue, DataPtr, 
                           DataId, DataIndir, DataReg,
			   Param, ReturnVal, DataAddr, FrameAddr,
			   SharedData};

        AstNode(); // mdl.C
	AstNode(const string &func, AstNode *l, AstNode *r);
        AstNode(const string &func, AstNode *l); // needed by inst.C
	AstNode(operandType ot, void *arg);
	AstNode(AstNode *l, AstNode *r);

        AstNode(opCode, AstNode *left); 
        // assumes "NULL" for right child ptr
        // needed by inst.C and stuff in ast.C

        AstNode(operandType ot, AstNode *l);
	AstNode(opCode ot, AstNode *l, AstNode *r, AstNode *e = NULL);
        AstNode(const string &func, vector<AstNode *> &ast_args);
        AstNode(function_base *func, vector<AstNode *> &ast_args);
	AstNode(function_base *func); // FuncJump (for replaceFunction)

        AstNode(AstNode *src);
        AstNode &operator=(const AstNode &src);

       ~AstNode();

        Address generateTramp(process *proc, char *i, Address &base,
			      int &trampCost, bool noCost);
	Address generateCode(process *proc, registerSpace *rs, char *i, 
			     Address &base, bool noCost, bool root);
	Address generateCode_phase2(process *proc, registerSpace *rs, char *i, 
				    Address &base, bool noCost);

	int cost() const;	// return the # of instruction times in the ast.
	void print() const;
        int referenceCount;     // Reference count for freeing memory
        int useCount;           // Reference count for generating code
        void setUseCount(void); // Set values for useCount
        void cleanUseCount(void);
        bool checkUseCount(registerSpace*, bool&);
        void printUseCount(void);
        Register kept_register; // Use when generating code for shared nodes
        void updateOperandsRC(bool flag); // Update operand's referenceCount
                                          // if "flag" is true, increments the
                                          // counter, otherwise it decrements 
                                          // the counter.
        void printRC(void);
	bool findFuncInAst(string func) ;
	void replaceFuncInAst(function_base *func1, function_base *func2);
	void replaceFuncInAst(function_base *func1, function_base *func2, vector<AstNode *> &ast_args, int index=0);

#if defined(sparc_sun_sunos4_1_3) || defined(sparc_sun_solaris2_4)  
	bool astFlag;  
	void sysFlag(instPoint *location);    

	void optRetVal(AstNode *opt);
#endif

    private:
        AstNode(opCode); // like AstNode(opCode, const AstNode &, 
                         //              const AstNode &)
                         // but assumes "NULL" for both child ptrs
	nodeType type;
	opCode op;		    // only for opCode nodes
	string callee;		    // only for call nodes
	function_base *calleefunc;  // only for call nodes
	vector<AstNode *> operands; // only for call nodes
	operandType oType;	    // for operand nodes
	void *oValue;	            // operand value for operand nodes
	const BPatch_type *bptype;  // type of corresponding BPatch_snippet
	bool doTypeCheck;	    // should operands be type checked

        // These 2 vrbles must be pointers; otherwise, we'd have a recursive
        // data structure with an infinite size.
        // The only other option is to go with references, which would have
        // to be initialized in the constructor and can't use NULL as a
        // sentinel value...
	AstNode *loperand;
	AstNode *roperand;
	AstNode *eoperand;

    public:
	// Functions for getting and setting type decoration used by the
	// dyninst API library
#ifdef BPATCH_LIBRARY
	const BPatch_type *getType() { return bptype; };
	void		  setType(const BPatch_type *t) { bptype = t; }
	void		  setTypeChecking(bool x) { doTypeCheck = x; }
	BPatch_type	  *checkType();
#endif
};

AstNode *assignAst(AstNode *src);
void removeAst(AstNode *&ast);
void terminateAst(AstNode *&ast);
AstNode *createIf(AstNode *expression, AstNode *action);
#if defined(MT_THREAD)
AstNode *createCounter(const string &func, void *, void *, AstNode *arg);
AstNode *createTimer(const string &func, void *, void *, 
                     vector<AstNode *> &arg_args);
AstNode *computeAddress(void *level, void *index, int type);
AstNode *addIndexToAddress(AstNode *addr, void *index, int type);
AstNode *computeTheAddress(void *level, void *index, int type);
#else
AstNode *createCounter(const string &func, void *, AstNode *arg);
AstNode *createTimer(const string &func, void *, 
                     vector<AstNode *> &arg_args);
#endif
Register emitFuncCall(opCode op, registerSpace *rs, char *i, Address &base, 
		      const vector<AstNode *> &operands, const string &func,
		      process *proc, bool noCost, const function_base *funcbase);
void emitFuncJump(opCode op, char *i, Address &base,
		  const function_base *func, process *proc);

#endif /* AST_HDR */
