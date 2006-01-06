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

// $Id: DMmetric.h,v 1.49 2006/01/06 23:11:13 legendre Exp $ 

#ifndef dmmetric_H
#define dmmetric_H
#include "pdutil/h/hist.h"
#include "dataManager.thread.h"
#include "dataManager.thread.SRVR.h"
#include <string.h>
#include "paradyn/src/UIthread/Status.h"
#include <stdlib.h>
#include "common/h/Vector.h"
#include "common/h/Dictionary.h"
#include "common/h/String.h"
#include "common/h/Time.h"
#include "pdutil/h/pdSample.h"
#include "pdutil/h/sampleAggregator.h"
#include "DMinclude.h"
#include "DMdaemon.h"
#include "paradyn/src/TCthread/tunableConst.h"
#include "pdutil/h/metricStyle.h"

// trace data streams
typedef void (*dataCallBack2)(const void *data,
                             int length,
                             void *userData);

#define UNUSED_METRIC_HANDLE	(unsigned) -1

class metricInstance;

// a part of an mi.
//
class component {
    friend class metricInstance;
    friend class paradynDaemon;
    public:
	component(paradynDaemon *d, int i, metricInstance *mi) {
	    daemon = d;
	    id = i;
            // Is this add unique?
            assert(i >= 0);
	    d->activeMids[(unsigned)id] = mi;
	    sample = 0;
	}
	~component() {
	    // don't delete aggComponent, since sampleAggregator will
	    // keep around to aggregate it's data
        MRN::Network * network = daemon->getNetwork() ;
        MRN::Communicator * comm = network->new_Communicator( );
        comm->add_EndPoint( daemon->getEndPoint() );

        MRN::Stream *stream = network->new_Stream(comm);

	    daemon->disableDataCollection(stream, id);
        assert(id>=0);
        daemon->disabledMids += (unsigned) id;
        daemon->activeMids.undef((unsigned)id);
	}
	int getId(){return(id);}
        paradynDaemon *getDaemon() { return(daemon); }

    private:
	aggComponent *sample;
	paradynDaemon *daemon;
	int id;
};


//
//  info about a metric in the system
//
class metric {
    friend class dataManager;
    friend class metricInstance;
    friend class paradynDaemon;
    friend void addMetric(T_dyninstRPC::metricInfo &info);
 public:
	metric(T_dyninstRPC::metricInfo i); 
	const T_dyninstRPC::metricInfo *getInfo() { return(&info); }
	const char *getName() { return((info.name.c_str()));}
	const char *getUnits() { return((info.units.c_str()));}
	dm_MetUnitsType  getUnitsType() { 
            if(info.unitstype == 0) return(UnNormalized);
	    else if(info.unitstype == 1) return(Normalized);
	    else return(Sampled);}
	metricHandle getHandle( void ) const { return(info.handle);}
	metricStyle  getStyle() { return((metricStyle) info.style); }
        int   getAggregate() { return info.aggregate;}

	static unsigned  size(){return(metrics.size());}
	static const T_dyninstRPC::metricInfo  *getInfo(metricHandle handle);
	static const char *getName(metricHandle handle);
        static const char *getUnits(metricHandle handle);
        static dm_MetUnitsType getUnitsType(metricHandle handle);
        static const metricHandle  *find(const pdstring name); 
	static pdvector<pdstring> *allMetricNames(bool all);
	static pdvector<met_name_id> *allMetricNamesIds(bool all);
	bool isDeveloperMode() { return info.developerMode; }
        static metric  *getMetric(metricHandle iName); 

    private:
	static dictionary_hash<pdstring, metric*> allMetrics;
	static pdvector<metric*> metrics;  // indexed by metric id
	T_dyninstRPC::metricInfo info;
};

struct archive_type {
    Histogram *data;
    phaseHandle phaseId;
};

typedef struct archive_type ArchiveType;

// data of this type is passed when Histogram makes callback to data mgr
// with the filled bucket values
struct histCallbackData {
  metricInstance *miPtr;
  bool globalFlag;
};

struct perfStreamEntry {
  bool sentInitActualVal;    // true when initialActualVal is sent down
                             // the performanceStream
  perfStreamHandle psHandle;
};

class metricInstance {
    friend class dataManager;
    friend class metric;
    friend class paradynDaemon;
    friend class metricFocusReqBundle;
    friend void DMdoEnableData(perfStreamHandle, perfStreamHandle,
                               pdvector<metricRLType> *, u_int, phaseType,
                               phaseHandle, u_int, u_int, u_int);
    friend void DMdisableRoutine(perfStreamHandle, perfStreamHandle,
                                 metricInstanceHandle, phaseType);
    // trace data streams
    friend void traceDataCallBack(const void *data, int length, void *arg);
    public:
	metricInstance(resourceListHandle rl, metricHandle m, phaseHandle ph);
	~metricInstance(); 
	pdSample getValue() {
	    pdSample ret;
	    if (!data) abort();
	    ret = data->getValue();
	    return(ret);
	}
	pdSample getTotValue() {
	    pdSample ret;
	    if (!data) abort();
	    ret = data->getValue();
	    return(ret);
	}

	int currUsersCount(){return(users.size());}
	int globalUsersCount(){return(global_users.size());}
	int getSampleValues(pdSample*, int, int, phaseType);
	int getArchiveValues(pdSample*, int, int, phaseHandle);
	timeLength getBucketWidth(phaseType phase);
	static timeLength determineAggInterval();

        static unsigned  mhash(const metricInstanceHandle &val){
	    return((unsigned)val);
	}
	metricInstanceHandle getHandle( void ) const { return(id); }
	metricHandle getMetricHandle(){ return(met); }
	resourceListHandle getFocusHandle(){ return(focus); }
	const char *getMetricName(){ return(metric::getName(met));}
        const char *getMetricUnits(){ return(metric::getUnits(met));}
        dm_MetUnitsType getMetricUnitsType(){return(metric::getUnitsType(met));}
	const char *getFocusName(){return(resourceList::getName(focus));}
	bool convertToIDList(pdvector<u_int> &rl){
	    return resourceList::convertToIDList(focus,rl);
        }
	void addInterval(relTimeStamp s, relTimeStamp e, pdSample v){
	     if(data) 
                 data->addInterval(s,e,v);
             if(global_data)
		 global_data->addInterval(s,e,v);
	}
        
        // trace data streams
        void sendTraceData(const void *data, int length) {
            if(traceFunc)
                (traceFunc)(data,length,this);
        }
        void assignTraceFunc(dataCallBack2 dcb) {
            traceFunc = dcb;
        }

        bool isCurrHistogram(){if (data) return(true); else return(false);}
	bool isGlobalHistogram(){ 
		if(global_data) return(true); else return(false);
	}
	bool isEnabled(){return(enabled);}
	void setEnabled(){ enabled = true;}
	void clearEnabled(){ enabled = false;}
	bool isCurrentlyEnabling(){return(currently_enabling);}
	void setCurrentlyEnabling(){ currently_enabling = true;}
	void clearCurrentlyEnabling(){ currently_enabling = false;}

	bool isCurrEnableOutstanding(){ return(currEnablesWaiting != 0);}
	bool isGlobalEnableOutstanding(){return(globalEnablesWaiting != 0);}
	void incrCurrWaiting(){ currEnablesWaiting++; }
	void incrGlobalWaiting(){ globalEnablesWaiting++; }
	void decrGlobalWaiting(){
	  if(globalEnablesWaiting) globalEnablesWaiting--;
        }
	void decrCurrWaiting(){
	  if(currEnablesWaiting) currEnablesWaiting--;
	}
	void globalPhaseDataCallback(pdSample *buckets, int count, int first);
	void currPhaseDataCallback(pdSample *buckets, int count, int first);

	static void updateAllAggIntervals();
	void updateAggInterval(timeLength nw) {
	  aggregator.changeAggIntervalWidth(nw);
	}
	void setInitialActualValue(pdSample s) {
	  initActualVal = s;
	  if(global_data)
	    global_data->setInitialActualValue(s);
	}
	pdSample getInitialActualValue()    {  return initActualVal;  }

	void setPersistentData(){ persistent_data = true; } 
	void setPhasePersistentData(){ phase_persistent_data = true; } 
	void setPersistentCollection(){ persistent_collection = true; } 
	// returns true if the metric instance can be deleted 
	bool clearPersistentData();  
	void clearPhasePersistentData(){phase_persistent_data=false;}  
	void clearPersistentCollection(){ persistent_collection = false; } 
	bool isDataPersistent(){ return persistent_data;}
	bool isPhaseDataPersistent(){ return phase_persistent_data;}
	bool isCollectionPersistent(){ return persistent_collection;}
	bool allComponentStartTimesReceived();
	// returns false if componet was already on list (not added)
	bool addComponent(component *new_comp);
        void removeComponent(paradynDaemon *daemon);
	void removeComponent(component *comp);
	// bool addPart(aggComponent *new_part);

	void updateComponent(paradynDaemon *dmn, timeStamp newStartTime,
			     timeStamp newEndTime, pdSample value);
	void doAggregation();

   // writes header info plus all values in histogram into file
   bool saveAllData(std::ofstream&, int &findex, const char *dirname,
                    SaveRequestType oFlag);
   static metricInstance *getMI(metricInstanceHandle);
	static timeLength GetGlobalWidth(){return(global_bucket_width);}
	static timeLength GetCurrWidth(){return(curr_bucket_width);}
	static void SetGlobalWidth(timeLength nw){global_bucket_width = nw;}
	static void SetCurrWidth(timeLength nw){curr_bucket_width = nw;}
	static phaseHandle GetCurrPhaseId(){return(curr_phase_id);}
	static void setPhaseId(phaseHandle ph){curr_phase_id = ph;}
	static void stopAllCurrentDataCollection(phaseHandle);
	static u_int numGlobalHists(){ return(num_global_hists); }
	static u_int numCurrHists(){ return(num_curr_hists); }
	static void incrNumGlobalHists(){ num_global_hists++; }
	static void incrNumCurrHists(){ num_curr_hists++; }
	static void decrNumCurrHists(){ if(num_curr_hists) num_curr_hists--; }
	static void decrNumGlobalHists(){ if(num_global_hists) num_global_hists--; }
   bool hasData( phaseType phase ) const;

    private:
	metricHandle met;
	resourceListHandle focus;
	timeLength enabledTime;
	bool enabled;    // set if data for mi is currently enabled
	bool currently_enabling; // set if MI is curr. being enabled 
	// to keep track of outstanding enable requests for this MI 
	u_int currEnablesWaiting; 
	u_int globalEnablesWaiting;

	// one component for each daemon contributing to the metric value 
	// each new component must be added to aggSample, and when
	// all processes in that component exit, the component should be removed
	// from aggSample.
	// When all components have a value, aggSample will return an aggregated
	// sample, that is bucketed by a histogram.
	pdvector<component *> components; 
	sampleAggregator aggregator;

	pdvector<perfStreamEntry> users;  // subscribers to curr. phase data
	Histogram *data;		 // data corr. to curr. phase
	pdvector<perfStreamEntry> global_users;  // subscribers to global data
	Histogram *global_data;	    // data corr. to global phase
        pdvector<ArchiveType *> old_data;  // histograms of data from old phases 
        
	// if set, archive old data on disable and on new phase definition
	bool persistent_data;  
	// if set, don't disable on new phase definition
	bool persistent_collection; 
	// if set, don't delete data on disable, but on new phase clear flag 
	bool phase_persistent_data;

	pdSample initActualVal;

	unsigned id;
	static dictionary_hash<metricInstanceHandle,metricInstance *> 
	    allMetricInstances;

	static u_int next_id;
        // info. about phase data
	static phaseHandle curr_phase_id;  // TODO: set this on Startphase
	static timeLength global_bucket_width;  // updated on fold
	static timeLength curr_bucket_width;    // updated on fold

	// these values only change on enable, disable, and phase start events
	// they count the number of active histograms (active means that data
	// is currently being collected for the histogram)
	static u_int num_curr_hists; // num of curr. phase active histograms 
	static u_int num_global_hists; // num of global phase active histograms 
        
        // trace data streams
        pdvector<perfStreamHandle> trace_users;  // subscribers to trace data
        dataCallBack2 traceFunc;
	
	static metricInstance *find(metricHandle, resourceListHandle);
        static pdvector<metricInstance*> *query(metric_focus_pair); 
	void flushPerfStreams();
        void newCurrDataCollection(dataCallBack, foldCallBack);
        void newGlobalDataCollection(dataCallBack, foldCallBack);
	timeStamp getEarliestFirstTime();
	void addCurrentUser(perfStreamHandle p); 
	void addGlobalUser(perfStreamHandle p); 
	// clear enabled flag and remove comps and parts
	void dataDisable();  
        void removeGlobalUser(perfStreamHandle);
        void removeCurrUser(perfStreamHandle);
	bool deleteCurrHistogram();

        // trace data streams
        void newTraceDataCollection(dataCallBack2);
        void addTraceUser(perfStreamHandle p);
        void removeTraceUser(perfStreamHandle);

	resourceList *getresourceList(){return(resourceList::getFocus(focus));}
	// write contents of one histogram plus header info into a file
	void saveOneMI_Histo(std::ofstream& fptr, Histogram *hdata, int phaseid);
};
#endif


