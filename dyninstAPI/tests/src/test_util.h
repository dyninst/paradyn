
#ifndef _test_h_
#define _test_h_

#include "BPatch_thread.h"
#include "BPatch_image.h"

void waitUntilStopped(BPatch *, BPatch_thread *appThread, 
	int testnum, char *testname);
void signalAttached(BPatch_thread *appThread, BPatch_image *appImage);
int startNewProcessForAttach(char *pathname, char *argv[]);

#endif
