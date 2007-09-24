#include "mutatee_util.h"

/* group_mutatee_boilerplate.c is prepended to this file by the make system */

/* Externally accessed function prototypes.  These must have globally unique
 * names.  I suggest following the pattern <testname>_<function>
 */

void test2_11_func1();

/* Global variables accessed by the mutator.  These must have globally unique
 * names.
 */

int test2_11_passed = 0;

/* Internally used function prototypes.  These should be declared with the
 * keyword static so they don't interfere with other mutatees in the group.
 */

/* Global variables used internally by the mutatee.  These should be declared
 * with the keyword static so they don't interfere with other mutatees in the
 * group.
 */

/* Function definitions follow */

void test2_11_func1() {
  DUMMY_FN_BODY;
}

int test2_11_mutatee() {
  test2_11_func1();

  if (test2_11_passed) {
    test_passes(testname);
    return 0; /* No error */
  } else {
    return -1; /* Error */
  }
}
