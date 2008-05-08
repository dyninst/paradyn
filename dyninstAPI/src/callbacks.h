#ifndef __CALLBACKS_H__
#define __CALLBACKS_H__
#include "dyninstAPI/h/BPatch_Vector.h"
#include "dyninstAPI/h/BPatch_Set.h"
#include "mailbox.h"
#include "rpcMgr.h"
#include "util.h"

inline unsigned eventTypeHash(const eventType &val) 
{
  return (unsigned int) val;
}   

///////////////////////////////////////////////////////////
/////  Callback Manager:  a container for callbacks
///////////////////////////////////////////////////////////

class CallbackManager {

  public:
    CallbackManager() : cbs(eventTypeHash) {}
    ~CallbackManager() {}

    bool registerCallback(eventType evt, CallbackBase *cb);
    bool removeCallbacks(eventType evt, pdvector<CallbackBase *> &_cbs);

    //  CallbackManager::dispenseCallbacksMatching
    //
    //  Makes a copy of any callbacks matching specified event type and
    //  returns them in a vector.  
    bool dispenseCallbacksMatching(eventType evt, pdvector<CallbackBase *> &cbs);

  private:
   dictionary_hash<eventType, pdvector<CallbackBase *> > cbs;
};

extern eventLock *global_mutex;

CallbackManager *getCBManager();
class SignalHandler;
class SyncCallback : public CallbackBase
{
  public:

  SyncCallback(bool is_synchronous = true, eventLock *l = global_mutex,
               unsigned long target_thread_id_ = TARGET_UI_THREAD,
               CallbackCompletionCallback completion_cb = signalCompletion) :
    CallbackBase(target_thread_id_, completion_cb), synchronous(is_synchronous),
    lock(l), completion_signalled(false), sh(NULL) {}
  SyncCallback(SyncCallback &src) :
    CallbackBase(src.targetThread(), src.getCleanupCallback()),  
    synchronous(src.synchronous), lock(global_mutex/*src.lock*/), completion_signalled(false), sh(NULL) {}
  virtual ~SyncCallback() {}

   void setSynchronous(bool flag = true) {synchronous = flag;}
   static void signalCompletion(CallbackBase *cb); 
   virtual bool execute(void); 
  protected:
   virtual bool execute_real(void) = 0;
   virtual bool waitForCompletion(); 
   bool do_it();
   bool synchronous;
   eventLock *lock;
   bool completion_signalled;
 public:
   SignalHandler *sh;
};

///////////////////////////////////////////////////////////
/////  Callback classes,  type-safe and know how to call themselves
///////////////////////////////////////////////////////////

// BPatchErrorCallback
#include "BPatch_callbacks.h"

class ErrorCallback : public SyncCallback 
{  
  public:
   ErrorCallback(BPatchErrorCallback callback): SyncCallback(false /*synchronous*/),
                                                cb(callback), str(NULL) {}
   ErrorCallback(ErrorCallback &src) : SyncCallback(false),
                                       cb(src.cb), num(-1), str(NULL) {}
   ~ErrorCallback();

   CallbackBase *copy() { return new ErrorCallback(*this);}
   bool execute_real(void); 
   bool operator()(BPatchErrorLevel severity,
                   int number,
                   const char *error_str);  
   BPatchErrorCallback getFunc() {return cb;}
  private:    
   BPatchErrorCallback cb;    
   BPatchErrorLevel sev;    
   int num;    
   char *str;
};

class ForkCallback : public SyncCallback 
{  
  public:
   ForkCallback(BPatchForkCallback callback) : SyncCallback(),
      cb(callback), par(NULL), chld(NULL) {}
   ForkCallback(ForkCallback &src) : SyncCallback(),
      cb(src.cb), par(NULL), chld(NULL) {}
   ~ForkCallback() {}

   CallbackBase *copy() { return new ForkCallback(*this);}
   bool execute_real(void); 
   bool operator()(BPatch_thread *parent,
                   BPatch_thread *child);
   BPatchForkCallback getFunc() {return cb;}
  private:    
   BPatchForkCallback cb;    
   BPatch_thread *par;
   BPatch_thread *chld;
};

class ExecCallback : public SyncCallback 
{  
  public:
   ExecCallback(BPatchExecCallback callback) : SyncCallback(),
      cb(callback), thread(NULL) {}
   ExecCallback(ExecCallback &src) : SyncCallback(),
      cb(src.cb), thread(NULL) {}
   ~ExecCallback() {}

   CallbackBase *copy() { return new ExecCallback(*this);}
   bool execute_real(void); 
   bool operator()(BPatch_thread *thread);
   BPatchExecCallback getFunc() {return cb;}
  private:    
   BPatchExecCallback cb;    
   BPatch_thread *thread;
};

class ExitCallback : public SyncCallback 
{  
  public:
   ExitCallback(BPatchExitCallback callback) : SyncCallback(),
      cb(callback), thread(NULL) {}
   ExitCallback(ExitCallback &src) : SyncCallback(),
      cb(src.cb), thread(NULL) {}
   ~ExitCallback() {}

   CallbackBase *copy() { return new ExitCallback(*this);}
   bool execute_real(void); 
   bool operator()(BPatch_thread *thread, BPatch_exitType exit_type);
   BPatchExitCallback getFunc() {return cb;}
  private:    
   BPatchExitCallback cb;    
   BPatch_thread *thread;
   BPatch_exitType type;
};

class SignalCallback : public SyncCallback 
{  
  public:
   SignalCallback(BPatchSignalCallback callback) : SyncCallback(),
       cb(callback), thread(NULL) {}
   SignalCallback(SignalCallback &src) : SyncCallback(),
      cb(src.cb), thread(NULL) {}
   ~SignalCallback() {}

   CallbackBase *copy() { return new SignalCallback(*this);}
   bool execute_real(void); 
   bool operator()(BPatch_thread *thread, int sigNum);
   BPatchSignalCallback getFunc() {return cb;}
  private:    
   BPatchSignalCallback cb;    
   BPatch_thread *thread;
   int num;
};

class OneTimeCodeCallback : public SyncCallback 
{  
  public:
   OneTimeCodeCallback(BPatchOneTimeCodeCallback callback) : SyncCallback(),
      cb(callback), thread(NULL) {}
   OneTimeCodeCallback(OneTimeCodeCallback &src) : SyncCallback(),
      cb(src.cb), thread(NULL), user_data(NULL), return_value(NULL) {}
   ~OneTimeCodeCallback() {}

   CallbackBase *copy() { return new OneTimeCodeCallback(*this);}
   bool execute_real(void); 
   bool operator()(BPatch_thread *thread, void *userData, void *returnValue);
   BPatchOneTimeCodeCallback getFunc() {return cb;}
  private:    
   BPatchOneTimeCodeCallback cb;    
   BPatch_thread *thread;
   void *user_data;
   void *return_value;
};

class DynLibraryCallback : public SyncCallback 
{  
  public:
   DynLibraryCallback(BPatchDynLibraryCallback callback) : SyncCallback(),
      cb(callback), thread(NULL) {}
   DynLibraryCallback(DynLibraryCallback &src) : SyncCallback(),
       cb(src.cb), thread(NULL), mod(NULL) {}
   ~DynLibraryCallback() {}

   CallbackBase *copy() { return new DynLibraryCallback(*this);}
   bool execute_real(void); 
   bool operator()(BPatch_thread *thread, BPatch_module *module, bool load);
   BPatchDynLibraryCallback getFunc() {return cb;}
  private:    
   BPatchDynLibraryCallback cb;    
   BPatch_thread *thread;
   BPatch_module *mod;
   bool load_param;
};

class DynamicCallsiteCallback : public SyncCallback 
{  
  public:
   DynamicCallsiteCallback(BPatchDynamicCallSiteCallback c) : SyncCallback(),
      cb(c), pt(NULL), func(NULL) {}
   DynamicCallsiteCallback(DynamicCallsiteCallback &src) : SyncCallback(),
      cb(src.cb), pt(NULL), func(NULL) {}
   ~DynamicCallsiteCallback() {}

   CallbackBase *copy() { return new DynamicCallsiteCallback(*this);}
   bool execute_real(void); 
   bool operator()(BPatch_point *at_point, BPatch_function *called_function);
   BPatchDynamicCallSiteCallback getFunc() {return cb;}
  private:    
   BPatchDynamicCallSiteCallback cb;    
   BPatch_point *pt;
   BPatch_function *func;
};

class UserEventCallback : public SyncCallback 
{  
  public:
   UserEventCallback(BPatchUserEventCallback callback) : SyncCallback(),
      cb(callback), proc(NULL), buf(NULL) {}
   UserEventCallback(UserEventCallback &src) : SyncCallback(),
       cb(src.cb), proc(NULL), buf(NULL), bufsize(-1) {}
   ~UserEventCallback();

   CallbackBase *copy() { return new UserEventCallback(*this);}
   bool execute_real(void); 
   bool operator()(BPatch_process *process, void *buffer, int buffersize);
   BPatchUserEventCallback getFunc() {return cb;}
  private:    
   BPatchUserEventCallback cb;    
   BPatch_process *proc;
   void *buf;
   int bufsize;
};

class StopThreadCallback : public SyncCallback
{
 public:
    StopThreadCallback(BPatchStopThreadCallback callback) : SyncCallback(),
        cb(callback), point(NULL), return_value(NULL) {}
    StopThreadCallback(StopThreadCallback &src) : SyncCallback(),
        cb(src.cb), point(NULL), return_value(NULL) {}
    ~StopThreadCallback() {}

    CallbackBase *copy() { return new StopThreadCallback(*this); }
    bool execute_real(void);
    bool operator()(BPatch_point *atPoint, void *returnValue);
    BPatchStopThreadCallback getFunc() {return cb;}
 private:
    BPatchStopThreadCallback cb;
    BPatch_point *point;
    void *return_value;
};

class SignalHandlerCallback : public SyncCallback
{
 public:
    SignalHandlerCallback(BPatchSignalHandlerCallback callback, 
                          BPatch_Set<long> *signums) :
        SyncCallback(), 
        cb(callback), 
        point(NULL), 
        signum(0), 
        handlers(NULL),
        signals(signums),
        lastSigAddr(0)
        {}
    SignalHandlerCallback(SignalHandlerCallback &src) : SyncCallback(),
        cb(src.cb), point(NULL), signum(0), handlers(NULL), signals(src.signals), 
        lastSigAddr(lastSigAddr) {}
    ~SignalHandlerCallback() {}
    CallbackBase *copy() { return new SignalHandlerCallback(*this); }
    bool execute_real(void);
    bool operator()(BPatch_point *at_point, long signum, 
                    BPatch_Vector<Dyninst::Address> *handlerVec);
    BPatchSignalHandlerCallback getFunc() {return cb;}
    Address getLastSigAddr() { return lastSigAddr; }
    bool handlesSignal(long signum);
 private:
    BPatchSignalHandlerCallback cb;
    BPatch_point *point;
    long signum;
    BPatch_Vector<Dyninst::Address> *handlers;
    BPatch_Set<long> *signals;
    Address lastSigAddr; // address of instruction that caused the previous signal 
};


class AsyncThreadEventCallback : public SyncCallback 
{  
  public:
   AsyncThreadEventCallback(BPatchAsyncThreadEventCallback callback) : 
      SyncCallback(), 
      cb(callback), proc(NULL), thr(NULL), override_to_sync(false) {}
   AsyncThreadEventCallback(AsyncThreadEventCallback &src) : SyncCallback(),
      cb(src.cb), proc(NULL), thr(NULL), override_to_sync(false) {}
   ~AsyncThreadEventCallback() {}

   CallbackBase *copy() { return new AsyncThreadEventCallback(*this);}
   bool execute_real(void); 
   bool operator()(BPatch_process *process, BPatch_thread *thread);
   BPatchAsyncThreadEventCallback getFunc() {return cb;}
   void set_synchronous(bool b) { override_to_sync = b; }
  private:    
   BPatchAsyncThreadEventCallback cb;    
   BPatch_process *proc;
   BPatch_thread *thr;
   bool override_to_sync;
};

typedef void (*internalThreadExitCallback)(BPatch_process *, BPatch_thread *,
                                           pdvector<AsyncThreadEventCallback *> *);

class InternalThreadExitCallback : public SyncCallback 
{  
  public:
   InternalThreadExitCallback(internalThreadExitCallback callback) : 
      SyncCallback(), 
      cb(callback), proc(NULL) {}
   InternalThreadExitCallback(InternalThreadExitCallback &src) : 
      SyncCallback(), 
      cb(src.cb), proc(NULL), thr(NULL), cbs(NULL) {}
   ~InternalThreadExitCallback();

   CallbackBase *copy() { return new InternalThreadExitCallback(*this);}
   bool execute_real(void); 
   bool operator()(BPatch_process *p, BPatch_thread *t, 
                   pdvector<AsyncThreadEventCallback *> *callbacks);
  private:    
   internalThreadExitCallback cb;
   BPatch_process *proc;
   BPatch_thread *thr;
   pdvector<AsyncThreadEventCallback *> *cbs;
};

#endif
