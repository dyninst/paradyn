#ifndef HIST
#define HIST

#include "util/h/Vector.h"
#include "util/h/sys.h"
#include "util/h/makenan.h"

typedef enum { HistInterval, HistBucket } histType;
typedef enum { HistNewValue, HistNewTimeBase } callType;
typedef sampleValue Bin;

typedef void (*foldCallBack)(timeStamp, void* userData, bool globalFlag);

typedef void (*dataCallBack)(sampleValue *buckets,
			     timeStamp start_time,
			     int numberOfBuckets,
			     int first,
			     void *userData,
			     bool globalFlag);

class Histogram;

typedef enum { histActive, histInactive } histStatus;
typedef enum { histSum, histAverage } histCompact;
typedef enum { histSplit, histSet } histAddMode;
typedef enum { EventCounter, SampledFunction } metricStyle;

class Histogram {
	void newDataFunc(callType type, timeStamp time, void* userData);
    public:
	~Histogram();
	// constructors for base start time and bucket width
	Histogram(metricStyle, dataCallBack, foldCallBack, void* , bool);
	Histogram(Bin *buckets, metricStyle, dataCallBack, foldCallBack, void*, bool);
	// constructors for specified start time
	Histogram(timeStamp, metricStyle, dataCallBack, foldCallBack, void*, bool );
	Histogram(Bin *buckets,  
		  timeStamp start,  // binWidth is computed by this value 
		  metricStyle, 
		  dataCallBack, 
		  foldCallBack, 
		  void*,
		  bool);
	sampleValue getValue();
	sampleValue getValue(timeStamp start, timeStamp end);
	sampleValue getBinValue(int i){
	    if(i <= lastBin){
	       return(dataPtr.buckets[i]);
	    }
	    else
	       return(make_Nan());
	}
	int getBuckets(sampleValue *buckets, int numberOfBuckets, int first);
	int getCurrBin(){return(lastBin);}
	timeStamp getStartTime(){return(startTime);}
	timeStamp getBucketWidth(){ return(bucketWidth);}
	void setActive(){ active = true;}
	void clearActive(){ active = false;}
	void setFoldOnInactive(){fold_on_inactive = true;}
	void clearFoldOnInactive(){fold_on_inactive = false;}
	bool isActive(){ return active;}
	bool foldOnInactive(){ return fold_on_inactive;}
	void addInterval(timeStamp start, timeStamp end, 
	    sampleValue value, bool smooth);
	void addPoint(timeStamp start, sampleValue value) {
	    addInterval(start, start, value, false);
	}
	static timeStamp currentTime() { 
		return((timeStamp)(lastGlobalBin*globalBucketSize)); 
	}
	static timeStamp getGlobalBucketWidth(){ return(globalBucketSize); }
	static timeStamp getMinBucketWidth(){ return(baseBucketSize);}
	static int getNumBins(){ return(numBins);}
    private:
	void foldAllHist();
	void convertToBins();
	void bucketValue(timeStamp start, timeStamp end, 
		sampleValue value, bool smooth);

	static int numBins;		/* max bins to use */

	// used to compute the current Time
	static timeStamp baseBucketSize;  // min. bucket size for all hists.
	static timeStamp globalBucketSize; // largest curr. bucket width 
	static int lastGlobalBin;  // for all hists. with global bins 

	timeStamp total_time;	/* numBins * baseBucketSize */

	// static Histogram *allHist;	/* linked list of all histograms */
	// Histogram *next;		/* linked list of all histograms */
        static vector<Histogram *> allHist;  // list of all histograms

        // these are added to support histograms with different bucket widths
	// with different startTimes  (an individual histogram does not need
	// to start at time 0, and thus will contain only values added after
	// its start time
	int lastBin;			/* current (for this hist) last bin */
	timeStamp bucketWidth;		// bucket width of this histogram 
	timeStamp startTime;		// not all histograms start at time 0
	bool active;			// if clear, don't add values 
	bool fold_on_inactive;		// if set, fold inactive histograms 

	histType storageType;	
	bool smooth;		/* prevent values greater than binWidth */
	metricStyle metricType; /* sampled function or event counter */
	int intervalCount;	/* # of intervals in use */
	int intervalLimit;	/* # of intervals in use */
	union {
	    Bin *buckets;
	    Interval *intervals;
	} dataPtr; 
	dataCallBack dataFunc;
	foldCallBack foldFunc;
	void *cData;
	bool globalData;    /* true if this histogram stores global phase data */
};

#endif
