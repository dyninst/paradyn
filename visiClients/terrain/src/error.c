/*
 * Copyright (c) 1989, 1990 Barton P. Miller, Morgan Clark, Timothy Torzewski
 *     Jeff Hollingsworth, and Bruce Irvin. All rights reserved.
 *
 * This software is furnished under the condition that it may not
 * be provided or otherwise made available to, or used by, any
 * other person.  No title to or ownership of the software is
 * hereby transferred.  The name of the principals
 * may not be used in any advertising or publicity related to this
 * software without specific, written prior authorization.
 * Any use of this software must include the above copyright notice.
 *
 */

/* 
 * error.c - code to generate an error window when an error occurs.  
 *
 * $Id: error.c,v 1.7 1998/03/30 01:22:23 wylie Exp $
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xmu/Xmu.h>

#include "Xbase.h"		/* For WindowInfo structure */
#include "miscx.h"
#include "action.h"
#include "error.h"
#include "terrain.h"

#define BUTTONC	3
char *buttons[BUTTONC] = {
    "Continue",
    "Quit",
    "Dump Core",
};




void ErrorWrapup(int pressed)
{
    if (pressed == 0)
	return;
    else if (pressed == 1)
	exit(0);
    else 	/* dump core */
	abort();
}

/*
 * host is an optional parameter.
 */

void popUpMsgs(int title, char *message)
{
    
    char *ch;
    int lCount;
    char *log[10];

    WindowInfo winInfo;

    log[0] = message;
    lCount = 1;
    for (ch=message; *ch != '\0'; ch++) {
       if (*ch == '\n') {
	  log[lCount++] = ch+1;
	  *ch = '\0';
       }
    }

    /* Put the message at the middle of the terrain window */

    IGetGeometry(win, &winInfo);
    InitAction(winInfo.x + (winInfo.width>>1),
	       winInfo.y + (winInfo.height>>1));
 
    RequestAction(title, (int)NULL, (int)NULL, (char*)lCount, log, buttons,
                  (struct Logo*)buttons, (struct Logo*)XtJustifyCenter, ErrorWrapup);
}

