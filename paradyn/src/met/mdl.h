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

// $Id: mdl.h,v 1.24 1999/12/01 14:41:44 zhichen Exp $

#ifndef MDL_EXTRA_H
#define MDL_EXTRA_H

#include "util/h/String.h"
// trace data streams
#include "util/h/ByteArray.h"
#include "util/h/Vector.h"
#include "util/h/Dictionary.h"
#include <iostream.h>
#include <fstream.h>
#include "util/h/aggregation.h"

// Toplevel for code in resource heirarchy....
#define CODE_RH_NAME "Code"
// field separator for resource heirarchy....
const char RH_SEPARATOR = '/';

#define MDL_FOLD_SUM 0
#define MDL_FOLD_AVG 1

#define MDL_AGG_SUM aggSum
#define MDL_AGG_AVG aggAvg
#define MDL_AGG_MIN aggMin
#define MDL_AGG_MAX aggMax

// MDL_T_POINT_RETURN is a kludge to check that we do not have 
// postInsn in a function return point. Once this is checked,
// the type should be set to MDL_T_POINT
#define MDL_T_POINT_RETURN 29

#define MDL_T_SCALAR_BASE 30
#define MDL_T_INT 30
#define MDL_T_FLOAT 31
#define MDL_T_STRING 32
#define MDL_T_BOOL 33
#define MDL_T_POINT 34
#define MDL_T_PROCEDURE 35
#define MDL_T_MODULE 36
#define MDL_T_CALLSITE 37
#define MDL_T_COUNTER 38
#define MDL_T_PROC_TIMER 39
#define MDL_T_WALL_TIMER 40
#define MDL_T_PROCESS 41
#define MDL_T_DRN     42
#define MDL_T_COUNTER_PTR 43
#define MDL_T_PROCEDURE_NAME 44
#define MDL_T_CONSTRAINT   45
#define MDL_T_MEMORY       46
#define MDL_T_VARIABLE     47
#define MDL_T_RECORD       48 //for $constraint[x].XXX
#define MDL_T_SCALAR_END 48

#define MDL_T_LIST_BASE 52
#define MDL_T_LIST_PROCEDURE_NAME 52
#define MDL_T_LIST_INT 53
#define MDL_T_LIST_FLOAT 54
#define MDL_T_LIST_STRING 55
#define MDL_T_LIST_PROCEDURE 56
#define MDL_T_LIST_MODULE 57
#define MDL_T_LIST_POINT 58
#define MDL_T_LIST_END 58

#define MDL_T_NONE 59

#define MDL_PREPEND 60
#define MDL_APPEND  61

#define MDL_RETURN 110
#define MDL_ENTER 111
#define MDL_SELF 112
#define MDL_CALLSITE 113
#define MDL_ARG 114

#define MDL_PLUS 120
#define MDL_MINUS 121
#define MDL_DIV 122
#define MDL_MULT 123
#define MDL_LT 124
#define MDL_GT 125
#define MDL_LE 126
#define MDL_GE 127
#define MDL_EQ 128
#define MDL_NE 129
#define MDL_AND 130
#define MDL_OR 131
#define MDL_NOT 132
#define MDL_ADDRESS 133
#define MDL_PLUSPLUS 134

#define MDL_ASSIGN 140
#define MDL_PLUSASSIGN 141
#define MDL_MINUSASSIGN 142

#define MDL_SET_COUNTER 200
#define MDL_ADD_COUNTER 201
#define MDL_SUB_COUNTER 202
#define MDL_START_WALL_TIMER 203
#define MDL_STOP_WALL_TIMER 204
#define MDL_START_PROC_TIMER 205
#define MDL_STOP_PROC_TIMER 206
#define MDL_READ_ADDRESS 207
#define MDL_READ_SYMBOL 208
#define MDL_CALL_FUNC 209
#define MDL_CALL_FUNC_COUNTER 210
//#if defined(MT_THREAD)
#define MDL_START_PROC_TIMER_LWP 211
//#endif

#define MDL_PRE_INSN   300
#define MDL_POST_INSN  301

// mdl expression types
#define MDL_EXPR_INT 400
#define MDL_EXPR_STRING 401
#define MDL_EXPR_INDEX 402
#define MDL_EXPR_BINOP 403
#define MDL_EXPR_FUNC 404
#define MDL_EXPR_DOT 405
#define MDL_EXPR_PREUOP 406
#define MDL_EXPR_POSTUOP 407
#define MDL_EXPR_VAR 408
#define MDL_EXPR_ASSIGN 409

#define MDL_RES_SYNCOBJECT 500
#define MDL_RES_CODE       501
#define MDL_RES_PROCESS    502
#define MDL_RES_MACHINE    503
#define MDL_RES_MEMORY    504

typedef struct {
  unsigned type;
  string name;
  bool end_allowed; // seems like it's only set, not used. -chun.
} mdl_type_desc;

typedef struct {
  mdl_type_desc self;
  vector<mdl_type_desc> kids;
} mdl_focus_element;

#if 0
//
// instr_req_to_string(), op_to_string(), and agg_to_string() are not
// used anywhere, and they are out-of-date. comment them out. may remove
// them in the future. --chun.
//
inline string instr_req_to_string(unsigned type, string obj) {

  switch(type) {
  case MDL_SET_COUNTER:
    return (string("setCounter(") + obj + ", "); 
  case MDL_ADD_COUNTER:
    return (string("addCounter(") + obj + ", ");
  case MDL_SUB_COUNTER:
    return (string("subCounter(") + obj + ", ");
  case MDL_START_WALL_TIMER:
    return (string("startWallTimer(") + obj + "); ");
  case MDL_STOP_WALL_TIMER:
    return (string("stopWallTimer(") + obj + "); ");
//#if defined(MT_THREAD)
  case MDL_START_PROC_TIMER_LWP:
    return (string("startProcTimer_lwp(") + obj + "); ");
//#endif
  case MDL_START_PROC_TIMER:
    return (string("startProcTimer(") + obj + "); ");
  case MDL_STOP_PROC_TIMER:
    return (string("stopProcTimer(") + obj + "); ");
  }
  assert(0);
  return(NULL);
}

inline string op_to_string(unsigned op) {
  switch(op) {
  case MDL_PLUS:
    return " + ";
  case MDL_MINUS:
    return " - ";
  case MDL_DIV:
    return " / ";
  case MDL_MULT:
    return " * ";
  case MDL_LT:
    return " < n";
  case MDL_GT:
    return " > ";
  case MDL_LE:
    return " <= ";
  case MDL_GE:
    return " >= ";
  case MDL_EQ:
    return " == ";
  case MDL_NE:
    return " != ";
  case MDL_AND:
    return " && ";
  case MDL_OR:
    return " || ";
  case MDL_NOT:
    return " ! ";
  default:
    assert(false);
    return ""; // placate compiler
  }
}

inline string agg_to_string(unsigned f) {
  switch (f) {
  case MDL_AGG_AVG: return ("avg");
  case MDL_AGG_SUM: return ("sum");
  case MDL_AGG_MAX: return ("max");
  case MDL_AGG_MIN: return ("min");
  default: return ("error");
  }
}
#endif // if 0

#if defined(PARADYN)
class process { };
class pdThread { };
class function_base { };
class module { };
class instPoint { };
class dataReqNode { };
class metricDefinitionNode { };
class AstNode { };
#else
class process;
class pdThread;
class function_base;
class module;
class instPoint;
class dataReqNode;
class metricDefinitionNode;
class AstNode;
#endif

class functionName {
 public:
  functionName(const string &nm) { name = nm; }
  string get() const { return name; }
 private:
  string name;
};

class mdl_var {
public:

  inline mdl_var(bool is_remote=false);
  mdl_var(const string &, bool is_remote);
  inline ~mdl_var();
  void reset();

  mdl_var& operator= (const mdl_var& var) {
    if (&var != this) {
        name_ = var.name_;
        type_ = var.type_;
        vals_ = var.vals_;
        remote_ = var.remote_;
        string_val_ = var.string_val_;
    }
    return(*this);
  }

  void dump();

  mdl_var(const string& nm, int i, bool is_remote);
  mdl_var(const string& nm, float f, bool is_remote);
  mdl_var(const string& nm, instPoint *p, bool is_remote);
  mdl_var(const string& nm, function_base *pr, bool is_remote);
  mdl_var(const string& nm, functionName *pr, bool is_remote);
  mdl_var(const string& nm, module *mod, bool is_remote);
  mdl_var(const string& nm, const string& s, bool is_remote);
  mdl_var(const string& nm, process *pr, bool is_remote);
  mdl_var(const string& nm, vector<function_base*> *vp, bool is_remote);
  mdl_var(const string& nm, vector<functionName*> *vp, bool is_remote);
  mdl_var(const string& nm, vector<module*> *vm, bool is_remote);
  mdl_var(const string& nm, vector<int> *vi, bool is_remote);
  mdl_var(const string& nm, vector<float> *vf, bool is_remote);
  mdl_var(const string& nm, vector<string> *vs, bool is_remote);
  mdl_var(const string& nm, dataReqNode *drn, bool is_remote);
  mdl_var(const string& nm, vector<instPoint*> *ipl, bool is_remote);

  bool get(int &i);
  bool get(float &f);
  bool get(instPoint *&p);
  bool get(function_base *&pr);
  bool get(functionName *&fn);
  bool get(module *&mod);
  bool get(string& s);
  bool get(process *&pr);
  bool get(vector<function_base*> *&vp);
  bool get(vector<functionName*> *&vp);
  bool get(vector<module*> *&vm);
  bool get(vector<int> *&vi);
  bool get(vector<float> *&vf);
  bool get(vector<string> *&vs);
  bool get(dataReqNode *&drn);
  bool get(vector<instPoint*> *&vip);

  bool set(int i);
  bool set(float f);
  bool set(instPoint *p);
  bool set(function_base *pr);
  bool set(functionName *f);
  bool set(module *mod);
  bool set(const string& s);
  bool set(process *pr);
  bool set(vector<function_base*> *vp);
  bool set(vector<functionName*> *vp);
  bool set(vector<module*> *vm);
  bool set(vector<int> *vi);
  bool set(vector<float> *vf);
  bool set(vector<string> *vs);
  bool set(dataReqNode *drn);
  bool set(vector<instPoint*> *ipl);

  void set_type(unsigned type);
  bool is_list() const;
  unsigned element_type() const;
  unsigned list_size() const;
  bool get_ith_element(mdl_var &ret, unsigned ith) const;
  unsigned as_list();
  bool make_list(unsigned type);
  bool add_to_list(mdl_var& add_me);
  
  const string &get_name() const {return name_;}
  unsigned type() const {return type_;}
  unsigned get_type() const {return type_;}
  bool is_remote() const {return remote_;}

private:
  union mdl_types {
    int i;
    float f;
    instPoint *point_;
    function_base *pr;
    functionName *fn;
    process *the_process;
    module *mod;
    vector<function_base*>  *list_pr;
    vector<functionName*> *list_fn;
    vector<int>          *list_int;
    vector<float>        *list_float;
    vector<string>       *list_string;
    vector<module*>      *list_module;
    vector<instPoint*>   *list_pts;
    dataReqNode          *drn;
    void *ptr;
  };

  string name_;
  unsigned type_;
  mdl_types vals_; // the union
  bool remote_;
  string string_val_; // put it here instead of inside union mdl_types
                      // because the compiler does not allow. --chun
};

class mdl_env {
public:
  static void dump();
  static void push();
  static bool pop();
  static bool add(mdl_var& value);

  static bool add(const string& var_name, bool is_remote, unsigned type=MDL_T_NONE);
  static bool get(mdl_var& ret, const string& var_name);
  static bool set(mdl_var& value, const string& var_name);

  static bool set(int i, const string& var_name);
  static bool set(float f, const string& var_name);
  static bool set(instPoint *p, const string& var_name);
  static bool set(function_base *pr, const string& var_name);
  static bool set(functionName *pr, const string& var_name);
  static bool set(module *mod, const string& var_name);
  static bool set(const string& s, const string& var_name);
  static bool set(process *pr, const string& var_name);
  static bool set(vector<function_base*> *vp, const string& var_name);
  static bool set(vector<functionName*> *vp, const string& var_name);
  static bool set(vector<module*> *vm, const string& var_name);
  static bool set(vector<int> *vi, const string& var_name);
  static bool set(vector<float> *vf, const string& var_name);
  static bool set(vector<string> *vs, const string& var_name);
  static bool set(vector<instPoint*> *vip, const string& var_name);
  static bool set(dataReqNode *drn, const string& var_name);

  //static unsigned get_type(string var_name) ;
  static bool set_type(unsigned type, const string& var_name);
  static bool get_type(unsigned &ret, const string& var_name);
  static bool is_remote(bool &is_rem, const string& var_name);

private:
  static bool find(unsigned &index, const string& var_name);
  static vector<unsigned> frames;
  static vector<mdl_var> all_vars;
};

inline void mdl_var::dump() {
  cout << name_ << " ";
  switch (type_) {
  case MDL_T_NONE:
    cout << " MDL_T_NONE\n";
    break;
  case MDL_T_INT:
    cout << " MDL_T_INT\n";
    break;
  case MDL_T_STRING:
    cout << "MDL_T_STRING\n";
    break;
  case MDL_T_FLOAT:
    cout << "MDL_T_FLOAT\n";
    break;
  case MDL_T_POINT:
    cout << "MDL_T_POINT\n";
    break;
  case MDL_T_PROCEDURE:
    cout << "MDL_T_PROCEDURE\n";
    break;
  case MDL_T_PROCEDURE_NAME:
    cout << "MDL_T_PROCEDURE_NAME\n";
    break;
  case MDL_T_MODULE:
    cout << "MDL_T_MODULE\n";
    break;
  case MDL_T_CALLSITE:
    cout << "MDL_T_CALLSITE\n";
    break;
  //case MDL_T_COUNTER:
  //  cout << "MDL_T_COUNTER\n";
  //  break;
  case MDL_T_WALL_TIMER:
    cout << "MDL_T_WALL_TIMER\n";
    break;
  case MDL_T_PROC_TIMER:
    cout << "MDL_T_PROC_TIMER\n";
    break;
  case MDL_T_DRN:
    cout << "MDL_T_DRN\n";
    break;
  case MDL_T_LIST_PROCEDURE:
    cout << "MDL_T_LIST_PROCEDURE\n";
    break;
  case MDL_T_LIST_PROCEDURE_NAME:
    cout << "MDL_T_LIST_PROCEDURE_NAME\n";
    break;
  case MDL_T_LIST_MODULE:
    cout << "MDL_T_LIST_MODULE\n";
    break;
  case MDL_T_LIST_POINT:
    cout << "MDL_T_LIST_POINT\n";
    break;
  case MDL_T_PROCESS:
    cout << "MDL_T_PROCESS\n";
    break;
  default:
    cout << " No type\n";
  }
}

// NB: this was previously somewhat curiously called "destroy"
inline void mdl_var::reset() { 
  type_ = MDL_T_NONE; vals_.ptr = NULL; 
}

inline mdl_var::~mdl_var() {
  reset();
}

#if PURE_BUILD
#define PURE_INIT(__p) memset(__p, '\0', sizeof(*(__p)))
#else
#define PURE_INIT(__p)
#endif

inline mdl_var::mdl_var(bool is_remote) 
: name_(""), type_(MDL_T_NONE), remote_(is_remote), string_val_("")
{ PURE_INIT(&vals_); vals_.ptr = NULL; }

inline mdl_var::mdl_var(const string& nm, bool is_remote)
: name_(nm), type_(MDL_T_NONE), remote_(is_remote), string_val_("")
{ PURE_INIT(&vals_); vals_.ptr = NULL; }

inline mdl_var::mdl_var(const string& nm, int i, bool is_remote)
: name_(nm), type_(MDL_T_INT), remote_(is_remote), string_val_("")
{ PURE_INIT(&vals_); vals_.i = i; }

inline mdl_var::mdl_var(const string& nm, float f, bool is_remote)
: name_(nm), type_(MDL_T_FLOAT), remote_(is_remote), string_val_("")
{ PURE_INIT(&vals_); vals_.f = f; }

inline mdl_var::mdl_var(const string& nm, instPoint *p, bool is_remote)
: name_(nm), type_(MDL_T_POINT), remote_(is_remote), string_val_("")
{ PURE_INIT(&vals_); vals_.point_ = p; }

inline mdl_var::mdl_var(const string& nm, function_base *pr, bool is_remote)
: name_(nm), type_(MDL_T_PROCEDURE), remote_(is_remote), string_val_("")
{ PURE_INIT(&vals_); vals_.pr = pr; }

inline mdl_var::mdl_var(const string& nm, functionName *fn, bool is_remote)
: name_(nm), type_(MDL_T_PROCEDURE_NAME), remote_(is_remote), string_val_("")
{ PURE_INIT(&vals_); vals_.fn = fn; }

inline mdl_var::mdl_var(const string& nm, module *md, bool is_remote)
: name_(nm), type_(MDL_T_MODULE), remote_(is_remote), string_val_("")
{ PURE_INIT(&vals_); vals_.mod = md; }

inline mdl_var::mdl_var(const string& nm, const string& s, bool is_remote)
: name_(nm), type_(MDL_T_STRING), remote_(is_remote), string_val_(s)
{ PURE_INIT(&vals_); vals_.ptr = NULL; }

inline mdl_var::mdl_var(const string& nm, process *p, bool is_remote) 
: name_(nm), type_(MDL_T_PROCESS), remote_(is_remote), string_val_("")
{ PURE_INIT(&vals_); vals_.the_process = p; }

inline mdl_var::mdl_var(const string& nm, vector<function_base*> *vp, bool is_remote) 
: name_(nm), type_(MDL_T_LIST_PROCEDURE), remote_(is_remote), string_val_("")
{ PURE_INIT(&vals_); vals_.list_pr = vp; }
     
inline mdl_var::mdl_var(const string& nm, vector<functionName*> *vf, bool is_remote) 
: name_(nm), type_(MDL_T_LIST_PROCEDURE_NAME), remote_(is_remote), string_val_("")
{ PURE_INIT(&vals_); vals_.list_fn = vf; }

inline mdl_var::mdl_var(const string& nm, vector<module*> *vm, bool is_remote) 
: name_(nm), type_(MDL_T_LIST_MODULE), remote_(is_remote), string_val_("")
{ PURE_INIT(&vals_); vals_.list_module = vm; }

inline mdl_var::mdl_var(const string& nm, vector<int> *vi, bool is_remote)
: name_(nm), type_(MDL_T_LIST_INT), remote_(is_remote), string_val_("")
{ PURE_INIT(&vals_); vals_.list_int = vi; }

inline mdl_var::mdl_var(const string& nm, vector<float> *vf, bool is_remote)
: name_(nm), type_(MDL_T_LIST_FLOAT), remote_(is_remote), string_val_("")
{ PURE_INIT(&vals_); vals_.list_float = vf; }

inline mdl_var::mdl_var(const string& nm, vector<string> *vs, bool is_remote)
: name_(nm), type_(MDL_T_LIST_STRING), remote_(is_remote), string_val_("")
{ PURE_INIT(&vals_); vals_.list_string = vs; }

inline mdl_var::mdl_var(const string& nm, vector<instPoint*> *vip, bool is_remote)
: name_(nm), type_(MDL_T_LIST_POINT), remote_(is_remote), string_val_("")
{ PURE_INIT(&vals_); vals_.list_pts = vip; }

inline mdl_var::mdl_var(const string& nm, dataReqNode *drn, bool is_remote) 
: name_(nm), type_(MDL_T_DRN), remote_(is_remote), string_val_("")
{ PURE_INIT(&vals_); vals_.drn = drn; }

inline bool mdl_var::get(int &i) {
  if (type_ != MDL_T_INT) return false;
  i = vals_.i;
  return true;
}
inline bool mdl_var::get(float &f) {
  if (type_ != MDL_T_FLOAT) return false;
  f = vals_.f;
  return true;
}

inline bool mdl_var::get(instPoint *&pt) {
  if (type_ != MDL_T_POINT) return false;
  pt = vals_.point_;
  return true;
}
inline bool mdl_var::get(function_base *&pr) {
  if (type_ != MDL_T_PROCEDURE) return false;
  pr = vals_.pr;
  return true;
}
inline bool mdl_var::get(functionName *&fn) {
  if (type_ != MDL_T_PROCEDURE_NAME) return false;
  fn = vals_.fn;
  return true;
}
inline bool mdl_var::get(module *&md) {
  if (type_ != MDL_T_MODULE) return false;
  md = vals_.mod;
  return true;
}
inline bool mdl_var::get(string& s) {
  if (type_ != MDL_T_STRING) return false;
  s = string_val_;
  return true;
}
inline bool mdl_var::get(process *&pr) {
  if (type_ != MDL_T_PROCESS) return false;
  pr = vals_.the_process;
  return true;
}
inline bool mdl_var::get(dataReqNode *&drn) {
  if (type_ != MDL_T_DRN) return false;
  drn = vals_.drn; 
  return true;
}

inline bool mdl_var::get(vector<function_base*> *&vp) {
  if (type_ != MDL_T_LIST_PROCEDURE) return false;
  vp = vals_.list_pr;
  return true;
}
inline bool mdl_var::get(vector<functionName*> *&vp) {
  if (type_ != MDL_T_LIST_PROCEDURE_NAME) return false;
  vp = vals_.list_fn;
  return true;
}
inline bool mdl_var::get(vector<module*> *&vm) {
  if (type_ != MDL_T_LIST_MODULE) return false;
  vm = vals_.list_module;
  return true;
}
inline bool mdl_var::get(vector<int> *&vi) {
  if (type_ != MDL_T_LIST_INT) return false;
  vi = vals_.list_int;
  return true;
}
inline bool mdl_var::get(vector<float> *&vf) {
  if (type_ != MDL_T_LIST_FLOAT) return false;
  vf = vals_.list_float;
  return true;
}
inline bool mdl_var::get(vector<string> *&vs) {
  if (type_ != MDL_T_LIST_STRING) return false;
  vs = vals_.list_string;
  return true;
}
inline bool mdl_var::get(vector<instPoint*> *&vip) {
  if (type_ != MDL_T_LIST_POINT) return false;
  vip = vals_.list_pts;
  return true;
}
inline bool mdl_var::set(int i) {
  reset();
  type_ = MDL_T_INT;
  vals_.i = i;
  return true;
}
inline bool mdl_var::set(float f) {
  reset();
  type_ = MDL_T_FLOAT;
  vals_.f = f;
  return true;
}

inline bool mdl_var::set(instPoint *pt) {
  reset();
  type_ = MDL_T_POINT;
  vals_.point_ = pt;
  return true;
}
inline bool mdl_var::set(function_base *pr) {
  reset();
  type_ = MDL_T_PROCEDURE;
  vals_.pr = pr;
  return true;
}
inline bool mdl_var::set(functionName *fn) {
  reset();
  type_ = MDL_T_PROCEDURE_NAME;
  vals_.fn = fn;
  return true;
}
inline bool mdl_var::set(module *md) {
  reset();
  type_ = MDL_T_MODULE;
  vals_.mod = md;
  return true;
}
inline bool mdl_var::set(const string& s) {
  reset();
  type_ = MDL_T_STRING;
  string_val_ = s;
  return true;
}
inline bool mdl_var::set(process *pr) {
  reset();
  type_ = MDL_T_PROCESS;
  vals_.the_process = pr;
  return true;
}
inline bool mdl_var::set(dataReqNode *drn) {
  reset();
  type_ = MDL_T_DRN;
  vals_.drn = drn;
  return true;
}

inline bool mdl_var::set(vector<function_base*> *vp) {
  reset();
  type_ = MDL_T_LIST_PROCEDURE;
  vals_.list_pr = vp;
  return true;
}
inline bool mdl_var::set(vector<functionName*> *vp) {
  reset();
  type_ = MDL_T_LIST_PROCEDURE_NAME;
  vals_.list_fn = vp;
  return true;
}
inline bool mdl_var::set(vector<module*> *vm) {
  reset();
  type_ = MDL_T_LIST_MODULE;
  vals_.list_module = vm;
  return true;
}
inline bool mdl_var::set(vector<int> *vi) {
  reset();
  type_ = MDL_T_LIST_INT;
  vals_.list_int = vi;
  return true;
}
inline bool mdl_var::set(vector<float> *vf) {
  reset();
  type_ = MDL_T_LIST_FLOAT;
  vals_.list_float = vf;
  return true;
}
inline bool mdl_var::set(vector<string> *vs) {
  reset();
  type_ = MDL_T_LIST_STRING;
  vals_.list_string = vs;
  return true;
}
inline bool mdl_var::set(vector<instPoint*> *vip) {
  reset();
  type_ = MDL_T_LIST_POINT;
  vals_.list_pts = vip;
  return true;
}

inline bool mdl_var::is_list() const {
  if ((type_ >= MDL_T_LIST_BASE) && (type_ <= MDL_T_LIST_END))
    return true;
  else
    return false;
}

inline unsigned mdl_var::list_size() const {
  switch (type_) {
  case MDL_T_LIST_INT:
    return (vals_.list_int->size());    
  case MDL_T_LIST_FLOAT:
    return (vals_.list_float->size());
  case MDL_T_LIST_STRING:
    return (vals_.list_string->size()); 
  case MDL_T_LIST_PROCEDURE:
    return (vals_.list_pr->size());
  case MDL_T_LIST_PROCEDURE_NAME:
    return (vals_.list_fn->size());
  case MDL_T_LIST_MODULE:
    return (vals_.list_module->size());
  default:
    return 0;
  }
}

inline bool mdl_var::get_ith_element(mdl_var &ret, unsigned ith) const {
  if (ith >= list_size()) return false;

  switch (type_) {
  case MDL_T_LIST_INT:
    return (ret.set((*vals_.list_int)[ith]));
  case MDL_T_LIST_FLOAT:
    return (ret.set((*vals_.list_float)[ith]));
  case MDL_T_LIST_STRING:
    return (ret.set((*vals_.list_string)[ith]));
  case MDL_T_LIST_PROCEDURE:
    assert(0); return false;
  case MDL_T_LIST_PROCEDURE_NAME:
    assert(0); return false;
  case MDL_T_LIST_MODULE:
    assert(0); return false;
  default:
    break;
  }
  return false;
}

inline bool mdl_var::add_to_list(mdl_var& add_me) {
  int i; float f; string s;
  if (!vals_.ptr) return false;
  switch (type_) {
  case MDL_T_LIST_INT:
    if (!add_me.get(i)) return false;
    (*vals_.list_int) += i;
    break;
  case MDL_T_LIST_FLOAT:
    if (!add_me.get(f)) return false;
    (*vals_.list_float) += f;
    break;
  case MDL_T_LIST_STRING: 
    if (!add_me.get(s)) return false;
    (*vals_.list_string) += s;
    break;
  default:
    return false;
  }
  return true;
}

inline bool mdl_var::make_list(unsigned element_type) {
  vector<string> *vs;
  vector<int> *vi;
  vector<float> *vf;
  vector<module*> *vm;
  vector<function_base*> *vp;
  vector<functionName*> *fn;

  switch (element_type) {
  case MDL_T_INT:
    vi = new vector<int>;
    if (!set(vi)) return false;
    break;
  case MDL_T_FLOAT:
    vf = new vector<float>;
    if (!set(vf)) return false;
    break;
  case MDL_T_STRING:
    vs = new vector<string>;
    if (!set(vs)) return false;
    break;
  case MDL_T_PROCEDURE:
    vp = new vector<function_base*>;
    if (!set(vp)) return false;
    break;
  case MDL_T_PROCEDURE_NAME:
    fn = new vector<functionName*>;
    if (!set(fn)) return false;
    break;
  case MDL_T_MODULE:
    vm = new vector<module*>;
    if (!set(vm)) return false;
    break;
  default:
    return false;
  }
  return true;
}

inline unsigned mdl_var::as_list() {
  switch (type_) {
  case MDL_T_INT:
    return MDL_T_LIST_INT;
  case MDL_T_FLOAT:
    return MDL_T_LIST_FLOAT;
  case MDL_T_STRING:
    return MDL_T_LIST_STRING;
  case MDL_T_PROCEDURE:
    return MDL_T_LIST_PROCEDURE;
  case MDL_T_PROCEDURE_NAME:
    return MDL_T_LIST_PROCEDURE_NAME;
  case MDL_T_MODULE:
    return MDL_T_LIST_MODULE;
  default:
    return MDL_T_NONE;
  }
}

inline void mdl_var::set_type(unsigned type) {
  reset();
  type_ = type;
}

inline unsigned mdl_var::element_type() const {
  switch (type_) {
  case MDL_T_LIST_INT:        return MDL_T_INT;
  case MDL_T_LIST_FLOAT:      return MDL_T_FLOAT;
  case MDL_T_LIST_STRING:     return MDL_T_STRING;
  case MDL_T_LIST_PROCEDURE:  return MDL_T_PROCEDURE;
  case MDL_T_LIST_PROCEDURE_NAME:  return MDL_T_PROCEDURE_NAME;
  case MDL_T_LIST_MODULE:     return MDL_T_MODULE;
  case MDL_T_LIST_POINT:      return MDL_T_POINT;
  default:                    return MDL_T_NONE;
  }
}

inline void mdl_env::dump() {
  for (unsigned u=0; u<mdl_env::all_vars.size(); u++)
    mdl_env::all_vars[u].dump();
}

inline void mdl_env::push() {
  mdl_env::frames += mdl_env::all_vars.size();
}

inline bool mdl_env::pop() {
  unsigned frame_size = mdl_env::frames.size();
  if (frame_size <= 0)
    return false;
  unsigned index = mdl_env::frames[frame_size - 1];
  mdl_env::frames.resize(frame_size-1);
  mdl_env::all_vars.resize(index);
  return true;
}

inline bool mdl_env::add(mdl_var& value) {
  mdl_env::all_vars += value;
  return true;
}

inline bool mdl_env::add(const string& var_name, bool is_remote, unsigned type) {
  mdl_var temp(var_name, is_remote);
  if (((type >= MDL_T_SCALAR_BASE) && (type <= MDL_T_SCALAR_END)) ||
      ((type >= MDL_T_LIST_BASE) && (type <= MDL_T_LIST_END)))
    temp.set_type(type);
  mdl_env::all_vars += temp;
  return true;
}

inline bool mdl_env::find(unsigned &index, const string& var_name) {
  unsigned size = mdl_env::all_vars.size();
  if (!size)
    return false;

  do {
    size--;
    if (mdl_env::all_vars[size].get_name() == var_name) {
      index = size;
      return true;
    }
  } while (size);
  return false;
}

inline bool mdl_env::get_type(unsigned &ret, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  ret = mdl_env::all_vars[index].get_type();
  return true;
}

inline bool mdl_env::is_remote(bool &is_rem, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  is_rem = mdl_env::all_vars[index].is_remote();
  return true;
}

inline bool mdl_env::set_type(unsigned type, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  mdl_env::all_vars[index].set_type(type);
  return true;
}

inline bool mdl_env::get(mdl_var &ret, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  
  ret = mdl_env::all_vars[index];
  return true;
}

inline bool mdl_env::set(int i, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  return mdl_env::all_vars[index].set(i);
}

inline bool mdl_env::set(float f, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  return mdl_env::all_vars[index].set(f);
}

inline bool mdl_env::set(instPoint *p, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  return mdl_env::all_vars[index].set(p);
}

inline bool mdl_env::set(function_base *pr, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  return mdl_env::all_vars[index].set(pr);
}

inline bool mdl_env::set(functionName *fn, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  return mdl_env::all_vars[index].set(fn);
}

inline bool mdl_env::set(module *mod, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  return mdl_env::all_vars[index].set(mod);
}

inline bool mdl_env::set(const string& s, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  return mdl_env::all_vars[index].set(s);
}

inline bool mdl_env::set(process *pr, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  return mdl_env::all_vars[index].set(pr);
}

inline bool mdl_env::set(dataReqNode *drn, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  return mdl_env::all_vars[index].set(drn);
}

inline bool mdl_env::set(vector<function_base*> *vp, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  return mdl_env::all_vars[index].set(vp);
}
inline bool mdl_env::set(vector<functionName*> *vp, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  return mdl_env::all_vars[index].set(vp);
}
inline bool mdl_env::set(vector<module*> *vm, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  return mdl_env::all_vars[index].set(vm);
}
inline bool mdl_env::set(vector<int> *vi, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  return mdl_env::all_vars[index].set(vi);
}
inline bool mdl_env::set(vector<float> *vf, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  return mdl_env::all_vars[index].set(vf);
}
inline bool mdl_env::set(vector<string> *vs, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  return mdl_env::all_vars[index].set(vs);
}
inline bool mdl_env::set(vector<instPoint*> *vip, const string& var_name) {
  unsigned index;
  if (!mdl_env::find(index, var_name))
    return false;
  return mdl_env::all_vars[index].set(vip);
}

#endif
