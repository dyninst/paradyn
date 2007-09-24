#include "mutatee_util.h"

/* Externally accessed function prototypes.  These must have globally unique
 * names.  I suggest following the pattern <testname>_<function>
 */

void test1_6_func2();
int test1_6_func1();

/* Global variables accessed by the mutator.  These must have globally unique
 * names.
 */

volatile int test1_6_globalVariable1 = (int)0xdeadbeef;
volatile int test1_6_globalVariable2 = (int)0xdeadbeef;
volatile int test1_6_globalVariable3 = (int)0xdeadbeef;
volatile int test1_6_globalVariable4 = (int)0xdeadbeef;
volatile int test1_6_globalVariable5 = (int)0xdeadbeef;
volatile int test1_6_globalVariable6 = (int)0xdeadbeef;
volatile int test1_6_globalVariable1a = (int)0xdeadbeef;
volatile int test1_6_globalVariable2a = (int)0xdeadbeef;
volatile int test1_6_globalVariable3a = (int)0xdeadbeef;
volatile int test1_6_globalVariable4a = (int)0xdeadbeef;
volatile int test1_6_globalVariable5a = (int)0xdeadbeef;
volatile int test1_6_globalVariable6a = (int)0xdeadbeef;

volatile int test1_6_constVar0 = 0;
volatile int test1_6_constVar1 = 1;
volatile int test1_6_constVar2 = 2;
volatile int test1_6_constVar3 = 3;
volatile int test1_6_constVar4 = 4;
volatile int test1_6_constVar5 = 5;
volatile int test1_6_constVar6 = 6;
volatile int test1_6_constVar7 = 7;
volatile int test1_6_constVar9 = 9;
volatile int test1_6_constVar10 = 10;
volatile int test1_6_constVar60 = 60;
volatile int test1_6_constVar64 = 64;
volatile int test1_6_constVar66 = 66;
volatile int test1_6_constVar67 = 67;

/* Internally used function prototypes.  These should be declared with the
 * keyword static so they don't interfere with other mutatees in the group.
 */

/* Global variables used internally by the mutatee.  These should be declared
 * with the keyword static so they don't interfere with other mutatees in the
 * group.
 */

/* Function definitions follow */

int test1_6_func1() {
  int retval;
  test1_6_func2();
  if ((test1_6_globalVariable1 == 60+2) && (test1_6_globalVariable2 == 64-1) &&
      (test1_6_globalVariable3 == 66/3) && (test1_6_globalVariable4 == 67/3) &&
      (test1_6_globalVariable5 == 6 * 5) && (test1_6_globalVariable6 == 3) &&
      (test1_6_globalVariable1a == 60+2) && (test1_6_globalVariable2a == 64-1) &&
      (test1_6_globalVariable3a == 66/3) && (test1_6_globalVariable4a == 67/3) &&
      (test1_6_globalVariable5a == 6 * 5) && (test1_6_globalVariable6a == 3)) {
    logerror("Passed test #6 (arithmetic operators)\n");
    retval = 0; /* Test passed */
  } else {
    logerror("**Failed** test #6 (arithmetic operators)\n");
    if (test1_6_globalVariable1 != 60+2)
      logerror("    addition error 60+2 got %d\n", test1_6_globalVariable1);
    if (test1_6_globalVariable2 != 64-1)
      logerror("    subtraction error 64-1 got %d\n", test1_6_globalVariable2);
    if (test1_6_globalVariable3 != 66/3)
      logerror("    division error 66/3 got %d\n", test1_6_globalVariable3);
    if (test1_6_globalVariable4 != 67/3)
      logerror("    division error 67/3 got %d\n", test1_6_globalVariable4);
    if (test1_6_globalVariable5 != 6 * 5)
      logerror("    mult error 6*5 got %d\n", test1_6_globalVariable5);
    if (test1_6_globalVariable6 != 3)
      logerror("    comma error 10,3 got %d\n", test1_6_globalVariable6);
    if (test1_6_globalVariable1a != 60+2)
      logerror("    addition error 60+2 got %d\n", test1_6_globalVariable1a);
    if (test1_6_globalVariable2a != 64-1)
      logerror("    subtraction error 64-1 got %d\n", test1_6_globalVariable2a);
    if (test1_6_globalVariable3a != 66/3)
      logerror("    division error 66/3 got %d\n", test1_6_globalVariable3a);
    if (test1_6_globalVariable4a != 67/3)
      logerror("    division error 67/3 got %d\n", test1_6_globalVariable4a);
    if (test1_6_globalVariable5a != 6 * 5)
      logerror("    mult error 6*5 got %d\n", test1_6_globalVariable5a);
    if (test1_6_globalVariable6a != 3)
      logerror("    comma error 10,3 got %d\n", test1_6_globalVariable6a);
    retval = -1; /* Test failed */
  }
  return retval;
}

/*
 * Start of Test #6 - arithmetic operators
 *	Verify arithmetic operators:
 *		constant integer addition
 *		constant integer subtraction
 *		constant integer divide (using large constants)
 *		constant integer divide (small consts)
 *		constant integer multiply
 *		constant comma operator
 *		variable integer addition
 *		variable integer subtraction
 *		variable integer divide (using large constants)
 *		variable integer divide (small consts)
 *		variable integer multiply
 *		variable comma operator
 *
 *	constant - use constant expressions
 *	variable - use variables in the expressions
 *
 */
int test1_6_mutatee() {
  if (test1_6_func1()) {
    return -1; /* Test failed */
  } else {
    test_passes(testname);
    return 0; /* Test passed */
  }
}

void test1_6_func2() {
  dprintf("test1_6_func2 () called\n");
}
