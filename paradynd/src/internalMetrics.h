
#include "metric.h"

typedef float (*sampleValueFunc)();

//
// Metrics that are internal to a paradyn daemon.
//
class internalMetric {
  public:
    internalMetric(char *n, int style, int a, char *units, sampleValueFunc f) {
	allInternalMetrics.add(this, n);
	metRec.info.name = n;
	metRec.info.style = style;
	metRec.info.aggregate = a;
	metRec.info.units = units;
	metRec.definition.baseFunc = NULL;
	metRec.definition.predicates = NULL;
	func = f;
    }
    float getValue() {
	if (func) {
	    return((func)());
	} else {
	    return(value);
	}
    }
    void enable(metricDefinitionNode *n) {
	node = n;
	activeInternalMetrics.add(this, n);
    }
    void disable() {
	node = NULL;
    }
    Boolean enabled() {
	return(node != NULL);
    }
    struct _metricRec metRec;
    float value;
    sampleValueFunc func;
    static List<internalMetric*> allInternalMetrics;
    static List<internalMetric*> activeInternalMetrics;
    metricDefinitionNode *node;
};
