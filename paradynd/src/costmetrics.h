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

#ifndef COST_METRICS_HDR 
#define  COST_METRICS_HDR

#include "paradynd/src/metric.h"
#include "paradynd/src/process.h"
#include "paradynd/src/im_preds.h"
#include "paradynd/src/internalMetrics.h"
#include "paradynd/src/resource.h"

#define PAST_LIMIT 20

struct pd_past_Values{
    float     value;
    timeStamp len;
};
typedef pd_past_Values pd_pastValues;

class costMetric {
  friend sampleInterval costMetricValueUpdate(costMetric *met, 
					      process *proc,
					      sampleValue value,
					      timeStamp endTime,
					      timeStamp processTime);
 public:
  costMetric(const string n,
	     metricStyle style, 
	     int a,   // how paradyn combines values from daemons
	     const string units,
	     im_pred_struct& preds,
	     bool developerMode,
	     daemon_MetUnitsType unitstype,
	     int combiner_op); // how daemons combine values from processes
  ~costMetric(){}
  void enable(metricDefinitionNode *n) { node = n; }
  void disable() { node = NULL; }
  bool enabled() { return(node != NULL); }
  metricStyle style() { return style_; }
  string name() const { return name_;}
  const char *getName() const { return name_.string_of();}
  int aggregate() const { return agg_; }
  //sampleValue getValue() { return (sample.value); }
  sampleValue getValue() { return cumulativeValue; }
  static costMetric *newCostMetric(const string n, 
			       metricStyle style, 
			       int a,   // how paradyn combines values
			       const string units,
			       im_pred_struct& preds,
			       bool developerMode,
			       daemon_MetUnitsType unitstype,
			       int combiner_op); // how daemons combine values
  
  T_dyninstRPC::metricInfo getInfo() {
    T_dyninstRPC::metricInfo ret;
    ret.name = name_; ret.style = style_;
    ret.aggregate = agg_; ret.units = units_;
    ret.developerMode = developermode_;
    ret.unitstype = 0;
    if(unitstype_ == UnNormalized) ret.unitstype = 0;
    else if (unitstype_ == Normalized) ret.unitstype = 1; 
    else if (unitstype_ == Sampled) ret.unitstype = 2; 
    return ret;
  }

  bool legalToInst(vector< vector<string> >& focus);
  bool isDeveloperMode() { return developermode_; }


  // add new entries to components, lastProcessTime, and parts
  bool addProcess(process *p);
  static bool addProcessToAll(process *newproc);
  // remove new entries to components, lastProcessTime, and parts
  bool removeProcess(process *p);
  static bool removeProcessFromAll(process *proc);

  timeStamp getLastSampleTime(process *proc){
      for(unsigned i=0; i < components.size(); i++){
	  if(proc == components[i]){
	        //return(parts[i]->lastSampleEnd);
                return(parts[i]->lastSampleTime());
      } }
      return 0.0;
  }
  timeStamp getLastSampleProcessTime(process *proc){
      for(unsigned i = 0; i < components.size(); i++){
	  if(proc == components[i]){
              return(lastProcessTime[i]);
      } }
      return 0.0;
  }
  sampleValue getCumulativeValue(process *proc){
      for(unsigned i = 0; i < components.size(); i++){
	  if(proc == components[i]){
	      return(cumulative_values[i]);
      } }
      return 0.0;
  }

  // updates the value of the cost metric 
  void updateValue(process *proc,        // process sending cost data
		   sampleValue newValue,  // new value 
		   timeStamp endTime,    // wall time
		   timeStamp processTime);  // CPU time
  // updates the smoothed value of the cost metric
  void updateSmoothValue(process *, sampleValue ,timeStamp, timeStamp ); 

  static vector<costMetric*> allCostMetrics;
  metricDefinitionNode *node;

private:
  // list of processes and values contributing to metric value
  vector<process *> components;
  vector<sampleInfo *> parts;
  aggregateSample aggSample;

  vector<sampleValue> cumulative_values;

  // process times associated with each component wall times are contained
  // in the parts vector
  vector<timeStamp> lastProcessTime;

  // circular list of past values for the metric (for computing smoothed costs)
  // these values are normalized by time
  pd_pastValues past[PAST_LIMIT];
  int  past_head;
  sampleValue smooth_sample;
  // sampleInfo sample;
  sampleValue cumulativeValue;

  string name_;
  int agg_;
  metricStyle style_;
  string units_;
  im_pred_struct pred;
  bool developermode_;
  daemon_MetUnitsType unitstype_;
  int combinerop_;
};

#endif
