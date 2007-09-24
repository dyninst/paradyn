#include "mutatee_util.h"

/* Externally accessed function prototypes.  These must have globally unique
 * names.  I suggest following the pattern <testname>_<function>
 */

int test1_16_func1();
void test1_16_func2();
void test1_16_func3();
void test1_16_func4();

/* Global variables accessed by the mutator.  These must have globally unique
 * names.
 */

int test1_16_globalVariable16_1 = 0;
int test1_16_globalVariable16_2 = 0;
int test1_16_globalVariable16_3 = 0;
int test1_16_globalVariable16_4 = 0;
int test1_16_globalVariable16_5 = 0;
int test1_16_globalVariable16_6 = 0;
int test1_16_globalVariable16_7 = 0;
int test1_16_globalVariable16_8 = 0;
int test1_16_globalVariable16_9 = 0;
int test1_16_globalVariable16_10 = 0;

/* Internally used function prototypes.  These should be declared with the
 * keyword static so they don't interfere with other mutatees in the group.
 */

/* Global variables used internally by the mutatee.  These should be declared
 * with the keyword static so they don't interfere with other mutatees in the
 * group.
 */

/* Function definitions follow */

/*
 * Test #16 - if-else
 *	Test if-then-else clauses
 */
int test1_16_func1() {
    int retval;
    int failed = FALSE;

    test1_16_func2();
    if (test1_16_globalVariable16_1 != 1 || test1_16_globalVariable16_2 != 0) {
        logerror("**Failed test #16 (if-else)\n");
	if (test1_16_globalVariable16_1 != 1)
	    logerror("    True clause of first if should have been executed but was not.\n");
	if (test1_16_globalVariable16_2 != 0)
	    logerror("    False clause of first if should not have been executed but was.\n");
	failed = 1;
    }

    test1_16_func3();
    if (test1_16_globalVariable16_3 != 0 || test1_16_globalVariable16_4 != 1) {
        logerror("**Failed test #16 (if-else)\n");
	if (test1_16_globalVariable16_3 != 1)
	    logerror("    True clause of second if should not have been executed but was.\n");
	if (test1_16_globalVariable16_4 != 0)
	    logerror("    False clause of second if should have been executed but was not.\n");
	failed = 1;
    }

    test1_16_func4();
    if ((test1_16_globalVariable16_5 != 0 || test1_16_globalVariable16_6 != 1) ||
        (test1_16_globalVariable16_7 != 0 || test1_16_globalVariable16_8 != 1) ||
        (test1_16_globalVariable16_9 != 0 || test1_16_globalVariable16_10 != 1)) {
	    logerror("    failed large if clauses tests.\n");
	failed = 1;
    }

    if (!failed) {
      logerror("Passed test #16 (if-else)\n");
      retval = 0; /* Test passed */
    } else {
      retval = -1; /* Test failed */
    }
    return retval;
}

void test1_16_func2() {
  dprintf("test1_16_func2 () called\n");
}

void test1_16_func3() {
  dprintf("test1_16_func3 () called\n");
}

void test1_16_func4() {
  dprintf("test1_16_func4 () called\n");
}

int test1_16_mutatee() {
  if (test1_16_func1() == 0) {
    test_passes(testname);
    return 0;
  } else {
    return -1;
  }
}
