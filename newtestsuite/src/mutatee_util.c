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

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "mutatee_util.h"

#ifdef os_windows
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#include <dlfcn.h>
#endif

#include "mutatee_call_info.h"
extern mutatee_call_info_t mutatee_funcs[];
extern int passedTest[];
extern unsigned int MAX_TEST;

#include "test_results.h"

#ifdef __cplusplus
}
#endif

/*
 * Stop the process (in order to wait for the mutator to finish what it's
 * doing and restart us).
 */
#if defined(alpha_dec_osf4_0) && defined(__GNUC__)
static long long int  beginFP;
#endif

/* New output driver system */
output_t *output = NULL;
char *loginfofn = "-";
char *logerrfn = "-";
char *stdoutfn = "-";
char *stderrfn = "-";
char *humanfn = "-";
static char *od_testname = NULL;

void warningLogResult(test_results_t result) {
  fprintf(stderr, "[%s:%u] - WARNING: output object not properly initialized\n", __FILE__, __LINE__);
}

void nullSetTestName(const char *testname) {
}

void stdLogResult(test_results_t result);
void stdSetTestName(const char *testname);

void initOutputDriver() {
  output = (output_t *) malloc(sizeof (*output));
  if (NULL == output) {
    /* This is a nasty error, and the mutatee can't function any more */
    fprintf(stderr, "[%s:%u] - Error allocating output driver object\n", __FILE__, __LINE__);
    abort(); /* Is this the right thing to do? */
  }
  output->log = stdOutputLog;
  output->vlog = stdOutputVLog;
  output->redirectStream = redirectStream;
  output->logResult = stdLogResult;
  output->setTestName = stdSetTestName;
}

/* I want to store *copies* of the filenames */
/* I also want to maintain "-" as a constant, so I know I don't need to free it
 * if it's what's being used.
 */
void redirectStream(output_stream_t stream, const char *filename) {
  size_t length;

  if ((filename != NULL) && (strcmp(filename, "-") != 0)) {
    length = strlen(filename) + 1;
  }

  switch (stream) {
  case STDOUT:
    if ((stdoutfn != NULL) && (strcmp(stdoutfn, "-") != 0)) {
      free(stdoutfn);
    }
    if (NULL == filename) {
      stdoutfn = NULL;
    } else if (strcmp(filename, "-") == 0) {
      stdoutfn = "-";
    } else {
      stdoutfn = (char *) malloc(length * sizeof (*stdoutfn));
      if (stdoutfn != NULL) {
	strncpy(stdoutfn, filename, length);
      }
    }
    break;

  case STDERR:
    if ((stderrfn != NULL) && (strcmp(stderrfn, "-") != 0)) {
      free(stderrfn);
    }
    if (NULL == filename) {
      stderrfn = NULL;
    } else if (strcmp(filename, "-") == 0) {
      stderrfn = "-";
    } else {
      stderrfn = (char *) malloc(length * sizeof (*stderrfn));
      if (stderrfn != NULL) {
	strncpy(stderrfn, filename, length);
      }
    }
    break;

  case LOGINFO:
    if ((loginfofn != NULL) && (strcmp(loginfofn, "-") != 0)) {
      free(loginfofn);
    }
    if (NULL == filename) {
      loginfofn = NULL;
    } else if (strcmp(filename, "-") == 0) {
      loginfofn = "-";
    } else {
      loginfofn = (char *) malloc(length * sizeof (*loginfofn));
      if (loginfofn != NULL) {
	strncpy(loginfofn, filename, length);
      }
    }
    break;

  case LOGERR:
    if ((logerrfn != NULL) && (strcmp(logerrfn, "-") != 0)) {
      free(logerrfn);
    }
    if (NULL == filename) {
      logerrfn = NULL;
    } else if (strcmp(filename, "-") == 0) {
      logerrfn = "-";
    } else {
      logerrfn = (char *) malloc(length * sizeof (*logerrfn));
      if (logerrfn != NULL) {
	strncpy(logerrfn, filename, length);
      }
    }
    break;

  case HUMAN:
    if ((humanfn != NULL) && (strcmp(humanfn, "-") != 0)) {
      free(humanfn);
    }
    if (NULL == filename) {
      humanfn = NULL;
    } else if (strcmp(filename, "-") == 0) {
      humanfn = "-";
    } else {
      humanfn = (char *) malloc(length * sizeof (*humanfn));
      if (humanfn != NULL) {
	strncpy(humanfn, filename, length);
      }
    }
    break;
  }
}

void stdOutputLog(output_stream_t stream, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  stdOutputVLog(stream, fmt, args);
  va_end(args);
}

void stdOutputVLog(output_stream_t stream, const char *fmt, va_list args) {
  char *logfn = NULL;
  switch (stream) {
  case STDOUT:
    logfn = stdoutfn;
    break;

  case STDERR:
    logfn = stderrfn;
    break;

  case LOGINFO:
    logfn = loginfofn;
    break;

  case LOGERR:
    logfn = logerrfn;
    break;

  case HUMAN:
    logfn = humanfn;
    break;
  }

  if (logfn != NULL) { /* Print nothing if it is NULL */
    FILE *out = NULL;
    if (strcmp(logfn, "-") == 0) { /* Default output */
      switch (stream) {
      case STDOUT:
      case LOGINFO:
      case HUMAN:
	out = stdout;
	break;

      case STDERR:
      case LOGERR:
	out = stderr;
	break;
      }
    } else { /* Specified output */
      out = fopen(logfn, "a");
      if (NULL == out) {
	fprintf(stderr, "[%s:%u] - Error opening log output file '%s'\n", __FILE__, __LINE__, logfn);
      }
    }

    if (out != NULL) {
      vfprintf(out, fmt, args);
      if ((out != stdout) && (out != stderr)) {
	fclose(out);
      }
    }
  }
} /* stdOutputVLog() */

void stdLogResult(test_results_t result) {
  printResultHumanLog(od_testname, result);
} /* stdLogResult() */

void stdSetTestName(const char *testname) {
  if (NULL == testname) {
    if (od_testname != NULL) {
      free(od_testname);
    }
    od_testname = NULL;
  } else {
    size_t len = strlen(testname) + 1;
    od_testname = (char *) malloc(len * sizeof (char));
    strncpy(od_testname, testname, len);
  }
}


/* Provide standard output functions that respect the specified output files */
FILE *outlog = NULL;
FILE *errlog = NULL;
void logstatus(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  stdOutputVLog(LOGINFO, fmt, args);
  va_end(args);
}
void logerror(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  stdOutputVLog(LOGERR, fmt, args);
  va_end(args);
}
void flushOutputLog() {
  if (outlog != NULL) {
    fflush(outlog);
  }
}
void flushErrorLog() {
  if (errlog != NULL) {
    fflush(errlog);
  }
}

/************************************************/
/* Support functions for database output driver */
/************************************************/

static const char *temp_logfilename = "mutatee_dblog";
static char *dblog_filename = NULL;

/* Redirect all output to a file, so test_driver can pick it up when it's
 * time to report results to the database server
 */
void initDatabaseOutputDriver() {
  output = (output_t *) malloc(sizeof (*output));
  if (output != NULL) {
    output->log = dbOutputLog;
    output->vlog = dbOutputVLog;
    output->redirectStream = dbRedirectStream;
    output->logResult = dbLogResult;
    output->setTestName = dbSetTestName;
  } else {
    fprintf(stderr, "[%s:%u] - Out of memory allocating output object\n", __FILE__, __LINE__);
    exit(-1); /* This is very bad */
  }
}

/* Sets the current test name, which controls the file we log to */
void dbSetTestName(const char *testname) {
  size_t len; /* For string allocation */
  FILE *pretestLog;

  if (NULL == testname) {
    /* TODO Handle error */
    return;
  }

  len = strlen("dblog.") + strlen(testname) + 1;
  dblog_filename = (char *) realloc(dblog_filename, len * sizeof (char));
  if (NULL == dblog_filename) {
    fprintf(stderr, "[%s:%u] - Out of memory allocating mutatee database log file name\n", __FILE__, __LINE__);
    return;
  }

  snprintf(dblog_filename, len, "dblog.%s", testname);

  /* Copy contents of temp log file into the dblog file, if there's
   * anything there.
   */
  pretestLog = fopen(temp_logfilename, "r");
  if (pretestLog != NULL) {
    /* Copy the file */
    size_t count;
    char *buffer[4096]; /* FIXME Magic number */
    FILE *out = fopen(dblog_filename, "a");
    if (out != NULL) {
      do {
	count = fread(buffer, sizeof (char), 4096, pretestLog);
	fwrite(buffer, sizeof (char), count, out);
      } while (4096 == count); /* FIXME Magic number */
    }
  } else if (errno != ENOENT) {
    /* It's not a problem is there is no pretest log file */
    fprintf(stderr, "[%s:%u] - Error opening pretest log: '%s'\n", __FILE__, __LINE__, strerror(errno));
  }
}

void warningRedirectStream(output_stream_t stream, const char *filename) {
  fprintf(stderr, "[%s:%u] - WARNING: output object not properly initialized\n", __FILE__, __LINE__);
}

void warningVLog(output_stream_t stream, const char *fmt, va_list args) {
  fprintf(stderr, "[%s:%u] - WARNING: output object not properly initialized\n", __FILE__, __LINE__);
}

void warningLog(output_stream_t stream, const char *fmt, ...) {
  fprintf(stderr, "[%s:%u] - WARNING: output object not properly initialized\n", __FILE__, __LINE__);
}

void warningSetTestName(const char *testname) {
  fprintf(stderr, "[%s:%u] - WARNING: output object not properly initialized\n", __FILE__, __LINE__);
}

void closeDatabaseOutputDriver() {
  if (dblog_filename != NULL) {
    free(dblog_filename);
  }
  output->log = warningLog;
  output->vlog = warningVLog;
  output->redirectStream = warningRedirectStream;
  output->logResult = warningLogResult;
  output->setTestName = warningSetTestName;
}

/* What do I do with output I receive before the output file name has been
 * set?  I need to store it in a temp. file or something.
 */
void dbOutputLog(output_stream_t stream, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  dbOutputVLog(stream, fmt, args);
  va_end(args);
}

void dbOutputVLog(output_stream_t stream,
		  const char *fmt, va_list args) {
  FILE *out;

  if (NULL == dblog_filename) {
    out = fopen(temp_logfilename, "a");
  } else {
    out = fopen(dblog_filename, "a");
  }

  if (NULL == out) {
    fprintf(stderr, "[%s:%u] - Error opening log file '%s'\n", __FILE__, __LINE__, (dblog_filename) ? dblog_filename : temp_logfilename);
    return;
  }

  vfprintf(out, fmt, args);
  fclose(out);
}

void dbRedirectStream(output_stream_t stream, const char *filename) {
  /* This doesn't do anything */
}

void dbLogResult(test_results_t result) {
  FILE *out;

  if (NULL == dblog_filename) {
    /* TODO Handle error */
  }

  out = fopen(dblog_filename, "a");
  if (NULL == out) {
    /* TODO Handle error */
  }

  fprintf(out, "RESULT: %d\n\n", result);
  fclose(out);
}


/*********************************************************/
/* Support for cleaning up mutatees after a test crashes */
/*********************************************************/

static char *pidFilename = NULL;
void setPIDFilename(char *pfn) {
  pidFilename = pfn;
}
char *getPIDFilename() {
  return pidFilename;
}
void registerPID(int pid) {
  if (NULL == pidFilename) {
    logerror("[%d:%u] - Error registering mutatee PID: pid file not set\n", __FILE__, __LINE__);
  } else {
    FILE *pidFile = fopen(pidFilename, "a");
    if (NULL == pidFile) {
      logerror("[%s:%u] - Error registering mutatee PID: error opening pid file\n", __FILE__, __LINE__);
    } else {
      fprintf(pidFile, "%d\n", pid);
      fclose(pidFile);
    }
  }
}

static int saved_stdout_fd = -1;
static int stdout_fd = 1;
/* setupFortranOutput() redirects stdout to point to outlog.  This seemed
 * like the easiest way to make the Fortran mutatees output the same way that
 * the rest of the mutatees do.
 * Returns 0 on success, <0 on failure.
 */
/* So there's a bug here, or else some behavior that I just don't understand,
 * where unless we write something to stdout before the dup2() redirection
 * it doesn't actually get redirected.  That's what the printf(" ") is in
 * there for.
 */
int setupFortranOutput() {
  int outlog_fd = fileno(outlog);
  if (-1 == outlog_fd) {
    return -1; /* Error */
  }
  printf(" "); /* Workaround */
  stdout_fd = fileno(stdout);
  saved_stdout_fd = dup(stdout_fd); /* Duplicate stdout */
  if (-1 == saved_stdout_fd) {
    return -2; /* Error */
  }
  if (dup2(outlog_fd, stdout_fd) == -1) {
    return -3; /* Error */
  }
  return 0;
}
/* cleanupFortranOutput() restores stdout so it points where it did before
 * a call to setupFortranOutput().  If setupFortranOutput() was never called,
 * then it just returns failure.
 * Returns 0 on successs, <0 on failure.
 */
int cleanupFortranOutput() {
  if (-1 == saved_stdout_fd) {
    return -1;
  }
  if (dup2(saved_stdout_fd, stdout_fd) == -1) {
    return -2; /* Error */
  }
  return 0;
}

int fastAndLoose = 0;

/* executable_name holds the name of the current mutatee binary */
char *executable_name = NULL;

void setExecutableName(const char *new_name) {
  executable_name = (char *) new_name;
}

/* Flag that says whether we're running in create or attach mode */
int use_attach = FALSE;
void setUseAttach(int v) {
  if (v) { /* normalize to {TRUE, FALSE} */
    use_attach = TRUE;
  } else {
    use_attach = FALSE;
  }
}

/* Filename for single line test results output */
char *humanlog_name = "-";
/* Change this to default to FALSE once test_driver is passing -humanlog
 * parameter to the mutatee
 */
int use_humanlog = TRUE;
void setHumanLog(const char *new_name) {
  if (NULL == new_name) {
    use_humanlog = FALSE;
  } else {
    use_humanlog = TRUE;
  } 
  /* humanlog_name = (char *) new_name; */
  redirectStream(HUMAN, new_name);
}

/* Print out single line message reporting whether a test passed, failed, was
 * skipped, or crashed.
 */
void printResultHumanLog(const char *testname, test_results_t result)
{
  FILE *human;

  if ((NULL == humanlog_name) || !strcmp(humanlog_name, "-")) {
    human = stdout;
  } else {
    human = fopen(humanlog_name, "a");
    if (NULL == human) {
      output->log(STDERR, "Error opening human log file '%s': %s\n",
	      humanlog_name, strerror(errno));
      human = stdout;
    }
  }

  output->log(HUMAN, "%s: mutatee: %s create_mode: ", testname,
	  (NULL == executable_name) ? "(unknown)" : executable_name);
  if (use_attach) {
    output->log(HUMAN, "attach");
  } else {
    output->log(HUMAN, "create");
  }
  output->log(HUMAN, "\tresult: ");
  switch (result) {
  case PASSED:
    output->log(HUMAN, "PASSED\n");
    break;

  case SKIPPED:
    output->log(HUMAN, "SKIPPED\n");
    break;

  case FAILED:
    output->log(HUMAN, "FAILED\n");
    break;
  }

  if (stdout == human) {
    fflush(human);
  } else {
    fclose(human);
  }

}

#ifdef DETACH_ON_THE_FLY
/*
 All this to stop ourselves.  We may be detached, but the mutator
 needs to notice the stop.  We must send a SIGILL to ourselves, not
 SIGSTOP, to get the mutator to notice.

 DYNINSTsigill is a runtime library function that does this.  Here we
 obtain a pointer to DYNINSTsigill from the runtime loader and then
 call it.  Note that this depends upon the mutatee having
 DYNINSTAPI_RT_LIB defined (with the same value as mutator) in its
 environment, so this technique does not work for ordinary mutatees.

 We could call kill to send ourselves SIGILL, but this is unsupported
 because it complicates the SIGILL signal handler.  */
static void
dotf_stop_process()
{
     void *h;
     char *rtlib;
     static void (*DYNINSTsigill)() = NULL;

     if (!DYNINSTsigill) {
	  /* Obtain the name of the runtime library linked with this process */
	  rtlib = getenv("DYNINSTAPI_RT_LIB");
	  if (!rtlib) {
	       output->log(STDERR, "ERROR: Mutatee can't find the runtime library pathname\n");
	       assert(0);
	  }

	  /* Obtain a handle for the runtime library */
	  h = dlopen(rtlib, RTLD_LAZY); /* It should already be loaded */
	  if (!h) {
	       output->log(STDERR, "ERROR: Mutatee can't find its runtime library: %s\n",
		       dlerror());
	       assert(0);
	  }

	  /* Obtain a pointer to the function DYNINSTsigill in the runtime library */
	  DYNINSTsigill = (void(*)()) dlsym(h, "DYNINSTsigill");
	  if (!DYNINSTsigill) {
	       output->log(STDERR, "ERROR: Mutatee can't find DYNINSTsigill in the runtime library: %s\n",
		       dlerror());
	       assert(0);
	  }
     }
     DYNINSTsigill();
}
#endif /* DETACH_ON_THE_FLY */

void stop_process_()
{
#ifdef i386_unknown_nt4_0
    DebugBreak();
#else

#if defined(alpha_dec_osf4_0) && defined(__GNUC__)
    /* This GCC-specific hack doesn't compile with other compilers, 
       which is unfortunate, since the native build test fails part 15
       with the error "process did not signal mutator via stop". */
    /* It also doesn't appear to be necessary when compiling with gcc-2.8.1,
       which makes its existance even more curious. */
    register long int fp asm("15");

    beginFP = fp;
#endif

#ifdef DETACH_ON_THE_FLY
    dotf_stop_process();
    return;
#endif

#if !defined(bug_irix_broken_sigstop)
    kill(getpid(), SIGSTOP);
#else
    kill(getpid(), SIGEMT);
#endif

#if defined(alpha_dec_osf4_0) && defined(__GNUC__)
    fp = beginFP;
#endif

#endif
}

/* This function sets a flag noting that testname ran successfully */
/* FIXME Need to give this a handle to the driver's mutatee_info structure
 * somehow
 */
void test_passes(const char *testname) {
  unsigned int i;
  for (i = 0; i < MAX_TEST; i++) {
    if (!strcmp(mutatee_funcs[i].testname, testname)) {
      /* Found it */
      passedTest[i] = TRUE;
      break;
    }
  }
}

/* This function sets a flag noting that testname ran unsuccessfully */
void test_fails(const char *testname) {
  unsigned int i;
  for (i = 0; i < MAX_TEST; i++) {
    if (!strcmp(mutatee_funcs[i].testname, testname)) {
      /* Found it */
      passedTest[i] = FALSE;
      break;
    }
  }
}

/* This function returns TRUE if the test has been marked as passing, and false
 * otherwise
 */
static int test_passed(const char *testname) {
  unsigned int i;
  int retval;
  for (i = 0; i < MAX_TEST; i++) {
    if (!strcmp(mutatee_funcs[i].testname, testname)) {
      /* Found it */
      retval = passedTest[i];
      break;
    }
  }
  if (i >= MAX_TEST) {
    retval = FALSE; /* Not found */
  }
  return retval;
}

/*
 * Verify that a scalar value of a variable is what is expected
 * returns TRUE on success, FALSE on failure
 */
int verifyScalarValue(const char *name, int a, int value,
		      const char *testName, const char *testDesc)
{
  if (a != value) {
    if (test_passed(testName)) {
      logerror("**Failed** test %s (%s)\n", testName, testDesc);
    }
    logerror("  %s = %d, not %d\n", name, a, value);
    return FALSE;
  }
  return TRUE;
}
