/*
 * Copyright (c) 1998 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as Paradyn") on an AS IS basis, and do not warrant its
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

/* $Id: RTheap-svr4.c,v 1.1 2000/03/05 15:44:26 zandy Exp $ */
/* RTheap-svr4.c: heap management for SVR4 platforms */

#include <stdio.h>
#include <sys/stat.h>   /* open */
#include <fcntl.h>      /* open */
#include <sys/types.h>  /* getpid, open */
#include <unistd.h>     /* getpid, ioctl */
#include "RTheap.h"     /* dyninstmm_t */

int
DYNINSTgetMemoryMap(unsigned *nump, dyninstmm_t **mapp)
{
     int fd;
     char buf[1024];
     dyninstmm_t *ms;
     unsigned num;

     sprintf(buf, "/proc/%d", getpid());
     fd = open(buf, O_RDONLY);
     if (0 > fd) {
	  perror("open /proc");
	  return -1;
     }
     if (0 > ioctl(fd, PIOCNMAP, &num)) {
	  perror("getMemoryMap (PIOCNMAP)");
	  close(fd);
	  return -1;
     }
     ms = (dyninstmm_t *) malloc((num+1) * sizeof(dyninstmm_t));
     if (!ms) {
	  fprintf(stderr, "DYNINSTgetMemoryMap: Out of memory\n");
	  close(fd);
	  return -1;
     }
     if (0 > ioctl(fd, PIOCMAP, ms)) {
	  perror("getMemoryMap (PIOCMAP)");
	  free(ms);
	  close(fd);
	  return -1;
     }

     /* success */
     close(fd);
     *nump = num;
     *mapp = ms;
     return 0;
}
