#include "mutatee_util.h"

/* Externally accessed function prototypes.  These must have globally unique
 * names.  I suggest following the pattern <testname>_<function>
 */

void test1_25_call1();

/* Global variables accessed by the mutator.  These must have globally unique
 * names.
 */

int test1_25_globalVariable1;
int *test1_25_globalVariable2;	/* used to hold addres of test1_25_globalVariable1 */
int test1_25_globalVariable3;
int test1_25_globalVariable4;
int test1_25_globalVariable5;
int test1_25_globalVariable6;
int test1_25_globalVariable7;

/* Internally used function prototypes.  These should be declared with the
 * keyword static so they don't interfere with other mutatees in the group.
 */

/* Global variables used internally by the mutatee.  These should be declared
 * with the keyword static so they don't interfere with other mutatees in the
 * group.
 */

static int test_failed = 0;

/* Function definitions follow */

/*
 * Test #25 - unary operators
 */
int test1_25_mutatee() {
  int retval;

/* MIPS is no longer supported, so this test isn't necessary
 * #if defined(mips_sgi_irix6_4)
 *   logerror("Skipped test #25 (unary operators)\n");
 *   logerror("\t- not implemented on this platform\n");
 *   passedTest[25] = TRUE;
 * #else
 */

    test1_25_globalVariable1 = 25000001;
    test1_25_globalVariable2 = (int *) 25000002;
    test1_25_globalVariable3 = 25000003;
    test1_25_globalVariable4 = 25000004;
    test1_25_globalVariable5 = 25000005;
    test1_25_globalVariable6 = -25000006;
    test1_25_globalVariable7 = 25000007;

    /* inst code we put into this function:
     *  At Entry:
     *     test1_25_globalVariable2 = &test1_25_globalVariable1
     *     test1_25_globalVariable3 = *test1_25_globalVariable2
     *     test1_25_globalVariable5 = -test1_25_globalVariable4
     *     test1_25_globalVariable7 = -test1_25_globalVariable6
     */

    test1_25_call1();

    if ((int *) test1_25_globalVariable2 != &test1_25_globalVariable1) {
	if (!test_failed) {
	  logerror("**Failed** test #25 (unary operators)\n");
	}
	test_failed = TRUE;
	logerror("    test1_25_globalVariable2 = %p, not %p\n",
	    test1_25_globalVariable2, (void *) &test1_25_globalVariable1);
    }

    if (test1_25_globalVariable3 != test1_25_globalVariable1) {
	if (!test_failed) {
	  logerror("**Failed** test #25 (unary operators)\n");
	}
	test_failed = TRUE;
	logerror("    test1_25_globalVariable3 = %d, not %d\n",
	    test1_25_globalVariable3, test1_25_globalVariable1);
    }

    if (test1_25_globalVariable5 != -test1_25_globalVariable4) {
	if (!test_failed) {
	  logerror("**Failed** test #25 (unary operators)\n");
	}
	test_failed = TRUE;
	logerror("    test1_25_globalVariable5 = %d, not %d\n",
	    test1_25_globalVariable5, -test1_25_globalVariable4);
    }

    if (test1_25_globalVariable7 != -test1_25_globalVariable6) {
	if (!test_failed) {
	  logerror("**Failed** test #25 (unary operators)\n");
	}
	test_failed = TRUE;
	logerror("    test1_25_globalVariable7 = %d, not %d\n",
	    test1_25_globalVariable7, -test1_25_globalVariable6);
    }

    if (!test_failed) {
      logerror("Passed test #25 (unary operators)\n");
      test_passes(testname);
      retval = 0; /* Test passed */
    } else {
      retval = -1; /* Test failed */
    }
    return retval;
/* #endif /* !MIPS */
}

void test1_25_call1() {
  DUMMY_FN_BODY;
}
