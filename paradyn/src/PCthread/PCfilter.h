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

/*
 * PCfilter.h
 *
 * Data filter class performs initial processing of raw DM data arriving 
 * in the Performance Consultant.  
 *
 * $Log: PCfilter.h,v $
 * Revision 1.12  1997/10/28 20:34:29  tamches
 * dictionary_lite --> dictionary_hash to take advantage of the new
 * and improved dictionary_hash class
 *
 * Revision 1.11  1996/08/16 21:03:27  tamches
 * updated copyright for release 1.1
 *
 * Revision 1.10  1996/07/26 07:28:12  karavan
 * bug fix: eliminated race condition from data subscription code.  Changed
 * data structures used as indices in class filteredDataServer.  Obsoleted
 * class fmf.
 *
 * Revision 1.9  1996/07/23 20:28:02  karavan
 * second part of two-part commit.
 *
 * implements new search strategy which retests false nodes under certain
 * circumstances.
 *
 * change in handling of high-cost nodes blocking the ready queue.
 *
 * code cleanup.
 *
 * Revision 1.8  1996/07/22 18:55:41  karavan
 * part one of two-part commit for new PC functionality of restarting searches.
 *
 * Revision 1.7  1996/05/08 07:35:13  karavan
 * Changed enable data calls to be fully asynchronous within the performance consultant.
 *
 * some changes to cost handling, with additional limit on number of outstanding enable requests.
 *
 * Revision 1.6  1996/05/06 04:35:10  karavan
 * Bug fix for asynchronous predicted cost changes.
 *
 * added new function find() to template classes dictionary_hash and
 * dictionary_lite.
 *
 * changed filteredDataServer::DataFilters to dictionary_lite
 *
 * changed normalized hypotheses to use activeProcesses:cf rather than
 * activeProcesses:tlf
 *
 * code cleanup
 *
 * Revision 1.5  1996/05/02 19:46:35  karavan
 * changed predicted data cost to be fully asynchronous within the pc.
 *
 * added predicted cost server which caches predicted cost values, minimizing
 * the number of calls to the data manager.
 *
 * added new batch version of ui->DAGconfigNode
 *
 * added hysteresis factor to cost threshold
 *
 * eliminated calls to dm->enable wherever possible
 *
 * general cleanup
 *
 * Revision 1.4  1996/04/30 06:26:51  karavan
 * change PC pause function so cost-related metric instances aren't disabled
 * if another phase is running.
 *
 * fixed bug in search node activation code.
 *
 * added change to treat activeProcesses metric differently in all PCmetrics
 * in which it is used; checks for refinement along process hierarchy and
 * if there is one, uses value "1" instead of enabling activeProcesses metric.
 *
 * changed costTracker:  we now use min of active Processes and number of
 * cpus, instead of just number of cpus; also now we average only across
 * time intervals rather than cumulative average.
 *
 * Revision 1.3  1996/04/07 21:24:38  karavan
 * added phaseID parameter to dataMgr->enableDataCollection2.
 *
 * Revision 1.2  1996/02/22 18:30:05  karavan
 * bug fix to PC pause/resume so only filters active at time of pause
 * resubscribe to data
 *
 * Revision 1.1  1996/02/02 02:07:27  karavan
 * A baby Performance Consultant is born!
 *
 */

#ifndef pc_filter_h
#define pc_filter_h

#include <stream.h>
#include <assert.h>
//sys.h defines the following:
//  typedef double timeStamp;
//  typedef float sampleValue;
//  typedef struct {
//     timeStamp start;
//     timeStamp end;
//      sampleValue value;
//  } Interval;
#include "PCintern.h"
#include "PCdata.h"

//filter
//==========
//Receives and sends data in real-time

//#define MAX(a, b) ((a) > (b) ? (a):(b))

class filter;
class filteredDataServer;

class PCmetricInst;
typedef PCmetricInst* fdsSubscriber;
typedef metricInstanceHandle fdsDataID;
typedef resourceListHandle focus;

/* 
   The filter class serves as a base class for individual types of 
   filters, which differ in how data values are computed using the 
   raw  data which serves as input.

   Every filter receives data manager data as input, and sends output
   to its list of subscribers at defined time intervals.  The subscribers
   are managed by methods of the parent class, dataProvider, in data.C
   and data.h.
*/
class filter : public dataProvider
{
  friend ostream& operator <<(ostream &os, filter& f);
  friend class filteredDataServer;
  enum FilterStatus { Active, ActivationRequestPending, Inactive };
 public:
  filter(filteredDataServer *keeper,  
	 metricHandle met, focus focs,
	 bool costFlag);
  ~filter() { ; }  
  // all processing for a fresh hunk of raw data
  virtual void newData(sampleValue, timeStamp, timeStamp) = 0;
  //
  metricInstanceHandle getMI () {return mi;}
  metricHandle getMetric() {return metric;}
  focus getFocus() {return foc;}
  bool isActive() {return status == Active;}
  bool isPending() {return status == ActivationRequestPending;}
  // true means we want to disable this as part of a PC pause event
  bool pausable() {return (numConsumers > 0 && !costFlag);}
  void setcostFlag() {costFlag = true;}
 protected:  
  // activate filter by enabling met/focus pair
  void wakeUp();
  void inactivate() {status = Inactive;}
  // these used in newData() to figure out intervals 
  void updateNextSendTime(timeStamp startTime);
  void getInitialSendTime(timeStamp startTime);
  // (re)enable data for an existing filter
  void activate();
  // current length of a single interval
  timeStamp intervalLength;
  // when does the interval currently being collected end?
  timeStamp nextSendTime;
  // current time, according to this filter
  timeStamp lastDataSeen;   
  // when did the interval currently being collected start?
  timeStamp partialIntervalStartTime;
  // numerator of running average
  sampleValue workingValue; 
  // denominator of running average: sum of all time used in this 
  // average; time used may not start at 0; may not be contiguous
  timeStamp workingInterval;  
  // identifiers for this filter
  metricInstanceHandle mi;
  metricHandle metric;
  focus foc;
 private:
  FilterStatus status;
  // if set, this data is not disabled for a pause because it is used in 
  // cost observation by all phases
  bool costFlag;
  // collector for these filters
  filteredDataServer *server;          
};

ostream& operator <<(ostream &os, filter& f);

/* an avgFilter computes values based on the running average over all 
   data received from first enable of its metric instance.  The current
   running average is sent to each subscriber at the end of each time 
   interval. 
*/
class avgFilter : public filter
{
public:
  avgFilter(filteredDataServer *keeper,  
	    metricHandle met, focus focs,
	    bool costFlag):
	      filter(keeper, met, focs, costFlag) {;}
  ~avgFilter(){;}
  // all processing for a fresh hunk of raw data
  void newData(sampleValue newVal, timeStamp start, timeStamp end);
};

/* a valFilter computes values based on the average over each time 
   interval.  The average for each time interval is sent to each 
   subscriber at the end of that interval.
*/
class valFilter : public filter
{
public:
  valFilter(filteredDataServer *keeper,  
	 metricHandle met, focus focs,
	 bool costFlag):
	   filter(keeper, met, focs, costFlag) {;}
  ~valFilter() {;}
  // all processing for a fresh hunk of raw data
  void newData(sampleValue newVal, timeStamp start, timeStamp end);
};

class filteredDataServer;

// contains variable number of filters; maintains subscribers to each
// filter; new subscription -> dm->enable() request and add filter; 
// end subscription -> if numSubscribers == 0, dm->disable request.
// 
class filteredDataServer
{
  friend class filter;
public:
  filteredDataServer(unsigned phID);
  ~filteredDataServer();

  // interface to subscribers (provider role)
  void addSubscription(fdsSubscriber sub,
			    metricHandle mh,
			    focus f,
			    filterType ft,
			    bool costFlag);
  void endSubscription(fdsSubscriber sub, fdsDataID subID);
  void endSubscription(fdsSubscriber sub, metricHandle met, focus foc);
  // cancel pending enable request for this met/foc pair
  void cancelSubRequest (fdsSubscriber sub, metricHandle met, focus foc);
  void unsubscribeAllData();
  void resubscribeAllData();

  // interface to raw data source (consumer role)
  void newBinSize(timeStamp newSize);
  void newData(metricInstanceHandle mih, sampleValue value, int bin);
  void newDataEnabled(vector<metricInstInfo>* newlyEnabled);

  // miscellaneous  
  timeStamp getCurrentBinSize () {return currentBinSize;}
 private:
  void printPendings(); 
  filter *findFilter(metricHandle mh, focus f);
  static unsigned fdid_hash (fdsDataID& val) {return (unsigned)val % 19;} 
  void inActivateFilter (filter *fil);
  void makeEnableDataRequest (metricHandle met, focus foc);
  unsigned getPCphaseID () {
    if (phType == CurrentPhase) return dmPhaseID+1;
    else return 0;
  }
  // size of dm histogram bucket; used to convert data bin number into interval
  timeStamp currentBinSize;  
  // current size of each interval of output
  timeStamp intervalSize;
  // used by filters to align send times across filters
  timeStamp nextSendTime;
  phaseType phType;
  unsigned dmPhaseID;
  // starting interval size we never go below this
  timeStamp minGranularity;

  // DataFilters  contain all filters which are now successfully enabled.  
  dictionary_hash<fdsDataID, filter*>DataFilters;

  // miIndex and AllDataFilters contain every filter ever created for this server
  dictionary_hash<focus, filter*> **miIndex;
  vector<filter*> AllDataFilters;
};

#endif
