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

#ifndef _BPatch_snippet_h_
#define _BPatch_snippet_h_

#include "BPatch_Vector.h"
#include "BPatch_point.h"

class AstNode;
class function_base;
class process;

class BPatch_function;
class BPatch_point;
class BPatch_type;

typedef enum {
    BPatch_lt,
    BPatch_eq,
    BPatch_gt,
    BPatch_le,
    BPatch_ne,
    BPatch_ge,
    BPatch_and,
    BPatch_or
} BPatch_relOp;

typedef enum {
    BPatch_assign,
    BPatch_plus,
    BPatch_minus,
    BPatch_divide,
    BPatch_times,
    BPatch_mod,
    BPatch_ref,
    BPatch_seq
} BPatch_binOp;

typedef enum {
    BPatch_negate,
    BPatch_address
} BPatch_unOp;


class BPatch_function {
    process *proc;
public:
// The following are for  internal use by the library only:
    function_base *func;
    BPatch_function(process *_proc, function_base *_func) :
	proc(_proc), func(_func) {};

// For users of the library:
    char	*getName(char *s, int len);

    BPatch_Vector<BPatch_point *>
	*findPoint(const BPatch_procedureLocation loc);
};


class BPatch_snippet {
public:
// The following members are for  internal use by the library only:
    AstNode	*ast; /* XXX It would be better if this was protected */
// End members for internal use only.

    BPatch_snippet() : ast(NULL) {};
    BPatch_snippet(const BPatch_snippet &);
    BPatch_snippet &operator=(const BPatch_snippet &);

    virtual	~BPatch_snippet();

    float	getCost();
};

class BPatch_arithExpr: public BPatch_snippet {
public:
    BPatch_arithExpr(BPatch_binOp op,
		     const BPatch_snippet &lOperand,
		     const BPatch_snippet &rOperand);
};

class BPatch_boolExpr : public BPatch_snippet {
public:
    BPatch_boolExpr(BPatch_relOp op, const BPatch_snippet &lOperand,
		    const BPatch_snippet &rOperand);
};

class BPatch_constExpr : public BPatch_snippet {
public:
    BPatch_constExpr(int value);
#ifdef BPATCH_NOT_YET
    BPatch_constExpr(float value);
#endif
    BPatch_constExpr(const char *value);
};

class BPatch_funcCallExpr : public BPatch_snippet {
public:
    BPatch_funcCallExpr(const BPatch_function& func,
			const BPatch_Vector<BPatch_snippet *> &args);
};

class BPatch_ifExpr : public BPatch_snippet {
public:
    BPatch_ifExpr(const BPatch_boolExpr &conditional,
		  const BPatch_snippet &tClase);
    BPatch_ifExpr(const BPatch_boolExpr &conditional,
		  const BPatch_snippet &tClase,
		  const BPatch_snippet &fClause);
};

class BPatch_nullExpr : public BPatch_snippet {
public:
    BPatch_nullExpr();
};

class BPatch_paramExpr : public BPatch_snippet {
public:
    BPatch_paramExpr(int n);
};

class BPatch_retExpr : public BPatch_snippet {
public:
    BPatch_retExpr();
};

class BPatch_sequence : public BPatch_snippet {
public:
    BPatch_sequence(const BPatch_Vector<BPatch_snippet *> &items);
};

class BPatch_variableExpr : public BPatch_snippet {
    process	*proc;
    void	*address;
    int		size;
public:
// The following functions are for internal use by the library only:
    BPatch_variableExpr(process *in_process, void *in_address,
			const BPatch_type *type);
    BPatch_variableExpr(process *in_process, void *in_address,
			int in_size);

// Public functions for use by users of the library:
    void readValue(void *dst);
    void readValue(void *dst, int len);
    void writeValue(const void *src);
    void writeValue(const void *src, int len);

    void *getBaseAddr() const { return address; }
};

class BPatch_breakPointExpr : public BPatch_snippet {
public:
    BPatch_breakPointExpr();
};

#endif /* _BPatch_snippet_h_ */
