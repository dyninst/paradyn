/*
 * This file contains the implementation of runtime dynamic instrumentation
 *   functions for a SUNOS SPARC processor.
 *
 * $Log: RTfuncs.c,v $
 * Revision 1.22  1995/12/10 16:35:52  zhichen
 * Minor clean up
 *
 * Revision 1.21  1995/10/27  01:04:40  zhichen
 * Added some comments to DYNINSTsampleValues.
 * Added some prototypes
 *
 * Revision 1.20  1995/08/29  20:26:55  mjrg
 * changed sample.observedCost to sample.obsCostIdeal
 *
 * Revision 1.19  1995/05/18  11:08:25  markc
 * added guard prevent timer start-stop during alarm handler
 * added version number
 *
 * Revision 1.18  1995/02/16  09:07:15  markc
 * Made Boolean type RT_Boolean to prevent picking up a different boolean
 * definition.
 *
 * Revision 1.17  1994/11/11  10:16:00  jcargill
 * "Fixed" pause_time definition for CM5
 *
 * Revision 1.16  1994/11/06  09:45:29  jcargill
 * Added prototype for clock functions to fix pause_time metric for cm5
 *
 * Revision 1.15  1994/11/02  19:03:54  hollings
 * Made the observed cost model use a normal variable rather than a reserved
 * register.
 *
 * Revision 1.14  1994/10/27  16:15:53  hollings
 * Temporary hack to normalize cost data until the CM-5 inst supports a max
 * operation.
 *
 * Revision 1.13  1994/09/20  18:27:17  hollings
 * removed hard coded clock value.
 *
 * Revision 1.12  1994/08/02  18:18:56  hollings
 * added code to save/restore FP state on entry/exit to signal handle
 * (really jcargill, but commited by hollings).
 *
 * changed comparisons on time regression to use 64 bit int compares rather
 * than floats to prevent fp rounding error from causing false alarms.
 *
 * Revision 1.11  1994/07/26  20:04:49  hollings
 * removed slots used variables.
 *
 * Revision 1.10  1994/07/22  19:24:53  hollings
 * added actual paused time for CM-5.
 *
 * Revision 1.9  1994/07/14  23:35:34  hollings
 * added return of cost model record.
 *
 * Revision 1.8  1994/07/11  22:47:49  jcargill
 * Major CM5 commit: include syntax changes, some timer changes, removal
 * of old aggregation code, old pause code, added signal-driven sampling
 * within node processes
 *
 * Revision 1.7  1994/07/05  03:25:09  hollings
 * obsereved cost model.
 *
 * Revision 1.6  1994/02/02  00:46:11  hollings
 * Changes to make it compile with the new tree.
 *
 * Revision 1.5  1993/12/13  19:47:29  hollings
 * support for DYNINSTsampleMultiple.
 *
 * Revision 1.4  1993/10/19  15:29:58  hollings
 * new simpler primitives.
 *
 * Revision 1.3  1993/10/01  18:15:53  hollings
 * Added filtering and resource discovery.
 *
 * Revision 1.2  1993/08/26  19:43:58  hollings
 * new include syntax.
 *
 * Revision 1.1  1993/07/02  21:49:35  hollings
 * Initial revision
 *
 *
 */
#include <sys/types.h>
#include <assert.h>
#include <sys/signal.h>
#include <stdio.h>

/*
 * Now our include files.
 *
 */
#include "rtinst/h/trace.h"
#include "rtinst/h/rtinst.h"

extern time64 DYNINSTgetCPUtime();
extern time64 DYNINSTgetWallTime();
/* zxu added the following */
extern void DYNINSTgenerateTraceRecord(traceStream sid, short type, short length, void *eventData, int flush) ;
extern void saveFPUstate(float *base) ;
void restoreFPUstate(float *base) ;
void DYNINSTflushTrace() ;

/* see note below - jkh 10/19/94 */
int DYNINSTnprocs;
int DYNINSTnumSampled;
int DYNINSTnumReported;
int DYNINSTtotalSamples;
float DYNINSTsamplingRate;
time64 DYNINSTlastCPUTime;
time64 DYNINSTlastWallTime;
int DYNINSTtotalAlaramExpires;
char DYNINSTdata[SYN_INST_BUF_SIZE];
char DYNINSTglobalData[SYN_INST_BUF_SIZE];

/* Prevents timers from being started-stopped during alarm handling */
int DYNINSTin_sample = 0;

/*
 * for now costCount is in cycles. 
 */
float DYNINSTcyclesToUsec;
time64 DYNINSTtotalSampleTime;

void DYNINSTreportCounter(intCounter *counter)
/*
 * also total ; i will chage to asynchronous samping anyway.
 */
{
    traceSample sample;

/*  printf("DYNINSTreportCounter ...\n") ; */
    sample.value = counter->value;
    sample.id = counter->id;
    DYNINSTtotalSamples++;
    DYNINSTgenerateTraceRecord(0, TR_SAMPLE, sizeof(sample), &sample, 0);
}

void DYNINSTsimplePrint()
{
    printf("inside dynamic inst function\n");
}

void DYNINSTentryPrint(int arg)
{
    printf("enter %d\n", arg);
}

void DYNINSTcallFrom(int arg)
{
    printf("call from %d\n", arg);
}

void DYNINSTcallReturn(int arg)
{
    printf("return to %d\n", arg);
}

void DYNINSTexitPrint(int arg)
{
    printf("exit %d\n", arg);
}

volatile int DYNINSTsampleMultiple = 1;

/*
 * This is a function that should be called when we want to sample the
 *   timers and counters.  The code to do the sampling is added as func
 *   entry dynamic instrumentation.
 *
 *   It also reports the current value of the observed cost.
 *
 */
void DYNINSTsampleValues()
{
/*     printf ("DYNINSTsampleValues called...\n"); */
/* 
 * There is alarm that will go off periodically. Each time the alaram expires. A routine
 * called DYNINSTalarmExpires (which is a handler for the alarm) will be executed.
 *
 * DYNINSTalarmExpires()
 * {
 * DYNINSTsampleValue() ; //DYNINSTsampleValue is initially  a dummy at the beginning 
 * }
 *
 * Each time a new focus-metric is instrumented (generated), paradynd
 * will also instrument DYNINSTsamplevalue() accordingly, basically it will add 
 * a pair of rountines DYNINSTreportCounter and DYNINSTreportTimer which will
 * report the corresponding timer and counter value when the alarm expires.
 * So my understanding now is that the alarm actually trigers the writing of 
 * TRACELIBbuffer which make sense. 
 */
    DYNINSTnumReported++;
}

/* The current observed cost since the last call to 
 *      DYNINSTgetObservedCycles(false) 
 */
unsigned DYNINSTobsCostLow;

/************************************************************************
 * int64 DYNINSTgetObservedCycles(RT_Boolean in_signal)
 *
 * report the observed cost of instrumentation in machine cycles.
 *
 * We keep cost as a 64 bit int, but the code generated by dyninst is
 *   a 32 bit counter (for speed).  So this function also converts the
 *   cost into a 64 bit value.
 ************************************************************************/
int64 DYNINSTgetObservedCycles(RT_Boolean in_signal) 
{
    static int64    value = 0;

    if (in_signal) {
        return value;
    }

    /* update the 64 bit version of the counter */
    value += DYNINSTobsCostLow;

    /* reset the low counter */
    DYNINSTobsCostLow = 0;
    return value;
}
void DYNINSTreportBaseTramps()
{
    costUpdate sample;
    time64 currentCPU;
    time64 currentWall;
    time64 elapsedWallTime;
    static time64 elapsedPauseTime = 0;

    sample.slotsExecuted = 0;

    sample.obsCostIdeal = ((double) DYNINSTgetObservedCycles(1)) *
	(DYNINSTcyclesToUsec / 1000000.0);

    currentCPU = DYNINSTgetCPUtime();
    currentWall = DYNINSTgetWallTime();
    elapsedWallTime = currentWall - DYNINSTlastWallTime;
    elapsedPauseTime += elapsedWallTime - (currentCPU - DYNINSTlastCPUTime);
    sample.pauseTime = ((double) elapsedPauseTime);
    sample.pauseTime /= 1000000.0;
    DYNINSTlastWallTime = currentWall;
    DYNINSTlastCPUTime = currentCPU;

    DYNINSTgenerateTraceRecord(0, TR_COST_UPDATE, sizeof(sample), 
	&sample, 0);
}

void DYNINSTreportCost(intCounter *counter)
{
    /*
     *     This should eventually be replaced by the normal code to report
     *     a mapped counter???
     */

    double cost;
    int64 value; 
    static double prevCost;
    traceSample sample;

    value = DYNINSTgetObservedCycles(1);

    cost = ((double) value) * (DYNINSTcyclesToUsec / 1000000.0);

    if (cost < prevCost) {
	fprintf(stderr, "Fatal Error Cost counter went backwards\n");
	fflush(stderr);
	sigpause(0xffff);
    }

    prevCost = cost;

    /* temporary until CM-5 aggregation code can handle avg operator */
    if (DYNINSTnprocs) cost /= DYNINSTnprocs;

    sample.value = cost;
    sample.id = counter->id;
    DYNINSTtotalSamples++;

    DYNINSTgenerateTraceRecord(0, TR_SAMPLE, sizeof(sample), &sample, 0);
}

/*
 * Call this function to generate a sample when needed.
 *   Exception is the exit from the program which DYNINSTsampleValues should
 *   be called directly!!!
 *
 */
void DYNINSTalarmExpire()
{
    time64 start, end;
    float fp_context[33];	/* space to store fp context */


     /* printf ("DYNINSTalarmExpired\n");        */
    /* should use atomic test and set for this */
    if (DYNINSTin_sample) return;

    DYNINSTin_sample = 1;

    /* only sample every DYNINSTsampleMultiple calls */
    DYNINSTtotalAlaramExpires++;
    if ((++DYNINSTnumSampled % DYNINSTsampleMultiple) == 0)  {

	saveFPUstate(fp_context);

	start = DYNINSTgetCPUtime();

	/* make sure we call this enough to keep observed cost accurate due to
	   32 cycle rollover */
	(void) DYNINSTgetObservedCycles(0);

	/* generate actual samples */
	DYNINSTsampleValues();

	DYNINSTreportBaseTramps();

	DYNINSTflushTrace();
	end = DYNINSTgetCPUtime();
	DYNINSTtotalSampleTime += end - start;

	restoreFPUstate(fp_context);
    }

    DYNINSTin_sample = 0;
}
