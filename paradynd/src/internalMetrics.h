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

#ifndef INTERNAL_MET_HDR
#define INTERNAL_MET_HDR

/*
 * $Log: internalMetrics.h,v $
 * Revision 1.16  1996/10/31 08:57:50  tamches
 * moved a routine's code from .h to .C file
 *
 * Revision 1.15  1996/08/16 21:19:11  tamches
 * updated copyright for release 1.1
 *
 * Revision 1.14  1996/04/29 03:38:03  tamches
 * complete overhaul & cleanification of internalMetrics class
 *
 * Revision 1.13  1996/01/29 20:16:30  newhall
 * added enum type "daemon_MetUnitsType" for internal metric definition
 * changed bucketWidth internal metric to EventCounter
 *
 * Revision 1.12  1995/12/18  23:26:59  newhall
 * changed metric's units type to have one of three values (normalized,
 * unnormalized, or sampled)
 *
 * Revision 1.11  1995/11/29 18:45:22  krisna
 * added inlines for compiler. added templates
 *
 * Revision 1.10  1995/11/17 17:24:23  newhall
 * support for MDL "unitsType" option, added normalized member to metric class
 *
 * Revision 1.9  1995/11/13  14:52:21  naim
 * Adding "mode" option to the Metric Description Language to allow specificacion
 * of developer mode for metrics (default mode is "normal") - naim
 *
 * Revision 1.8  1995/08/24  15:04:11  hollings
 * AIX/SP-2 port (including option for split instruction/data heaps)
 * Tracing of rexec (correctly spawns a paradynd if needed)
 * Added rtinst function to read getrusage stats (can now be used in metrics)
 * Critical Path
 * Improved Error reporting in MDL sematic checks
 * Fixed MDL Function call statement
 * Fixed bugs in TK usage (strings passed where UID expected)
 *
 *
 */
#include "metric.h"
#include "im_preds.h"
#include "dyninstRPC.xdr.h" // T_dyninstRPC

typedef float (*sampleValueFunc)(const metricDefinitionNode *);

typedef enum {UnNormalized, Normalized, Sampled} daemon_MetUnitsType;

//
// Metrics that are internal to a paradyn daemon.
//

class internalMetric {
 private:
  string name_;
  int agg_;
  metricStyle style_;
  string units_;
  im_pred_struct pred;
  bool developermode_;
  daemon_MetUnitsType unitstype_;

  sampleValueFunc func; // a func taking in no params and returning float

 public:
  class eachInstance {
   private:
     // If func!=NULL, then it is used to indirectly obtain the value of
     // the internal metric (getValue()).  Otherwise, the vrble "value" is used.
     sampleValueFunc func; // a func taking in no params and returning float
     float value;
     float cumulativeValue;
     metricDefinitionNode *node;

   public:
     eachInstance() {} // needed by Vector class.
     eachInstance(sampleValueFunc f, metricDefinitionNode *n);
     eachInstance(const eachInstance &src);
    ~eachInstance() {}

     eachInstance &operator=(const eachInstance &src);
     
     bool matchMetricDefinitionNode(const metricDefinitionNode *match_me) const {
        return (node == match_me);
     }

     float getValue() const {
        if (func != NULL) return func(node);
	return value;
     }
     void setValue(float newValue) {
        assert(func == NULL);
	value = newValue;
     }

     float getCumulativeValue() const {
        return cumulativeValue;
     }
     void bumpCumulativeValueBy(float addme) {
        cumulativeValue += addme;
     }

     int getMId() const {
        assert(node);
        return node->getMId();
     }

     void report(timeStamp start, timeStamp end, sampleValue valueToForward);
  };

 private:
  // enabled instances of this internal metric are here:
  vector<eachInstance> instances;

 public:
  internalMetric(const string &n, metricStyle style, int a, const string &units,
		 sampleValueFunc f, im_pred_struct& im_preds,
		 bool developermode, daemon_MetUnitsType unitstype);

  unsigned num_enabled_instances() const {
     return instances.size();
  }
  eachInstance &getEnabledInstance(unsigned index) {
     return instances[index];
  }
  const eachInstance &getEnabledInstance(unsigned index) const {
     return instances[index];
  }

  void enableNewInstance(metricDefinitionNode *n) {
     eachInstance newGuy(func, n);
     instances += newGuy;
  }
  void disableInstance(unsigned index);
  bool disableByMetricDefinitionNode(metricDefinitionNode *diss_me);
  
  int getMId(unsigned index) const {
     // returns the mid from the metricDefinitionNode.  Used in metric.C
     return instances[index].getMId();
  }

  metricStyle style() const;
  const string &name() const;
  int aggregate() const;
  bool isDeveloperMode() const;

  T_dyninstRPC::metricInfo getInfo();

  bool legalToInst(const vector< vector<string> >& focus) const;

  static vector<internalMetric*> allInternalMetrics; // should be private!
  static internalMetric *newInternalMetric(const string &n, metricStyle style, 
					   int a,
					   const string &units,
					   sampleValueFunc f, 
					   im_pred_struct& preds,
					   bool developerMode,
					   daemon_MetUnitsType unitstype);
};

#endif
