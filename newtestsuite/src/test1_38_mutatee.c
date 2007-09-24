#include "mutatee_util.h"

/* group_mutatee_boilerplate.c is prepended to this file by the make system */

/* Externally accessed function prototypes.  These must have globally unique
 * names.  I suggest following the pattern <testname>_<function>
 */

void test1_38_call1();
/* Global variables accessed by the mutator.  These must have globally unique
 * names.
 */

int test1_38_globalVariable2 = 0;

/* Internally used function prototypes.  These should be declared with the
 * keyword static so they don't interfere with other mutatees in the group.
 */

static void funCall38_1();
static void funCall38_2();
static void funCall38_3();
static void funCall38_4();
static void funCall38_5();
static void funCall38_6();
static void funCall38_7();

/* Global variables used internally by the mutatee.  These should be declared
 * with the keyword static so they don't interfere with other mutatees in the
 * group.
 */

static int globalVariable38_1 = 0;

/* Function definitions follow */

/* Test #38 (basic block addresses) */

int test1_38_mutatee() {
    /* The only possible failures occur in the mutator. */
  if (1 == test1_38_globalVariable2) {
    logerror( "Passed test #38 (basic block addresses)\n" );
    test_passes(testname);
    return 0; /* Test passed */
  } else {
    return -1; /* Test failed */
  }
}

/* the mutator checks if the addresses of these call* functions are within 
   the address ranges of the basic blocks in the flowgraph */
void test1_38_call1() {
    int i, j, k, m;

    funCall38_1();

    for (i = 0; i < 50; i++) {
	m = i;
	funCall38_2();

	for (j = 0; i < 100; i++) {
	    for (k = 0; k < i ; i++) {
		funCall38_3();
	    }
	}

	funCall38_4();

	while (m < 100) {
	    funCall38_5();
	    m++;
	}

	funCall38_6();
    }

    funCall38_7();
}

void funCall38_1() { globalVariable38_1++; }
void funCall38_2() { globalVariable38_1++; }
void funCall38_3() { globalVariable38_1++; }
void funCall38_4() { globalVariable38_1++; }
void funCall38_5() { globalVariable38_1++; }
void funCall38_6() { globalVariable38_1++; }
void funCall38_7() { globalVariable38_1++; }
