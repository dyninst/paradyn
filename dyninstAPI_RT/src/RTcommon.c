/*
 * Copyright (c) 1996 Barton P. Miller
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

/* $Id: RTcommon.c,v 1.18 2001/12/11 20:24:41 chadd Exp $ */

#if defined(i386_unknown_nt4_0)
#include <process.h>
#define getpid _getpid
#else
#if !defined(mips_unknown_ce2_11) /*ccw 15 may 2000 : 29 mar 2001*/
#include <unistd.h>
#endif

#endif
#if !defined(mips_unknown_ce2_11) /*ccw 15 may 2000 : 29 mar 2001*/
#include <assert.h>
#endif

#include "dyninstAPI_RT/h/dyninstAPI_RT.h"


#if defined(sparc_sun_solaris2_4) /* ccw 19 nov 2001*/
#include  <fcntl.h>
#include  <libelf.h>
#include <sys/mman.h>
#include <dlfcn.h>
#endif

extern void DYNINSTbreakPoint();
extern void DYNINSTos_init(int calledByFork, int calledByAttach);

unsigned int DYNINSTversion = 1;
unsigned int DYNINSTobsCostLow;
unsigned int DYNINSThasInitialized = 0; /* 0 : has not initialized
					   2 : initialized by Dyninst
					   3 : initialized by Paradyn */

struct DYNINST_bootstrapStruct DYNINST_bootstrap_info ={0,0,0,'\0'} ; /*ccw 10 oct 2000 : 29 mar 2001*/

double DYNINSTglobalData[SYN_INST_BUF_SIZE/sizeof(double)];
double DYNINSTstaticHeap_32K_lowmemHeap_1[32*1024/sizeof(double)];
double DYNINSTstaticHeap_4M_anyHeap_1[4*1024*1024/sizeof(double)];

/* Written to by daemon just before launching an inferior RPC */
rpcInfo curRPC = { 0, 0, 0 };
unsigned pcAtLastIRPC;  /* just used to check for errors */
/* 1 = a trap was ignored and needs to be regenerated
   0 = there is not a trap that hasn't been processed */
int trapNotHandled = 0;

#ifdef DEBUG_PRINT_RT
int DYNINSTdebugPrintRT = 1;
#else
int DYNINSTdebugPrintRT = 0;
#endif

int DYNINST_mutatorPid = -1;

extern const char V_libdyninstAPI_RT[];

double DYNINSTdummydouble = 4321.71; /* Global so the compiler won't
					optimize away FP code in initFPU */
static void initFPU()
{
       /* Init the FPU.  We've seen bugs with Linux (e.g., Redhat 6.2
	  stock kernel on PIIIs) where processes started by Paradyn
	  started with FPU uninitialized. */
       double x = 17.1234;
       DYNINSTdummydouble *= x;
}

#if defined(i386_unknown_nt4_0)  /*ccw 13 june 2001*/
/* these variables are used by the mutator to pass values to the dll
 they are only used by the win2k/nt40 dyninstAPI*/
int libdyninstAPI_RT_DLL_localCause=-1, libdyninstAPI_RT_DLL_localPid=-1; /*ccw 2 may 2001*/
#endif


#if defined(sparc_sun_solaris2_4)

unsigned int checkAddr;
int checkElfFile(){


	Elf32_Shdr *shdr;
        Elf32_Ehdr *   ehdr;
        Elf *          elf;
        int       cnt,fd;
        Elf_Data *elfData,*strData;
        Elf_Scn *scn;
        char execStr[256];
	int retVal = 0;
	unsigned int mmapAddr;
	int pageSize;
	Address dataAddress;
	int dataSize;
       	char* tmpPtr;
        unsigned int updateAddress, updateSize, updateOffset;
        unsigned int *dataPtr;
 	unsigned int numberUpdates,i ;
	char* oldPageData;
	Dl_info dlip; 
	elf_version(EV_CURRENT);

        sprintf(execStr,"/proc/%d/object/a.out",getpid());
	
        if((fd = (int) open(execStr, O_RDONLY)) == -1){
                RTprintf("cannot open : %s\n",execStr);
    		fflush(stdout); 
		return;
        }
        if((elf = elf_begin(fd, ELF_C_READ, NULL)) ==NULL){
                RTprintf("%s %s \n",execStr, elf_errmsg(elf_errno()));
                RTprintf("cannot elf_begin\n");
		fflush(stdout);
                close(fd);
                return;
        }

        ehdr = elf32_getehdr(elf);
        scn = elf_getscn(elf, ehdr->e_shstrndx);
        strData = elf_getdata(scn,NULL);
	pageSize =  getpagesize();
   	for(cnt = 0, scn = NULL; scn = elf_nextscn(elf, scn);cnt++){
                shdr = elf32_getshdr(scn);
		if(!strncmp((char *)strData->d_buf + shdr->sh_name, "dyninstAPI_data", 15)) {
			elfData = elf_getdata(scn, NULL);
			tmpPtr = elfData->d_buf;
			dataAddress = -1;
			while( dataAddress != 0 ) { 
				dataSize = *(int*) tmpPtr;
				tmpPtr+=sizeof(int);
				dataAddress = *(Address*) tmpPtr;
				tmpPtr += sizeof(Address);
				if(dataAddress){
					memcpy((char*) dataAddress, tmpPtr, dataSize);
					tmpPtr += dataSize;
				}
			}

		}else if(!strncmp((char *)strData->d_buf + shdr->sh_name, "dyninstAPI_",11)){
			retVal = 1; /* this is a restored run */
		}
		if(!strncmp((char *)strData->d_buf + shdr->sh_name, "dyninstAPIhighmem_",18)){
			/*the layout of dyninstAPIhighmem_ is:
			pageData
			address of update
			size of update
			...	
			address of update
			size of update	
			number of updates
	
			we must ONLY overwrite the updates, the other
			areas of the page may be important (and different than
			the saved data in the file.  we first copy out the
			page, the apply the updates to it, and then
			write it back.
			*/
			elfData = elf_getdata(scn, NULL);
			numberUpdates = (unsigned int) ( ((char*) elfData->d_buf)[
				elfData->d_size - sizeof(unsigned int)]);
			oldPageData = (char*) malloc(shdr->sh_size-((2*numberUpdates+1)*
				sizeof(unsigned int)) );	
			/*copy old page data */

			/** TEST 
			dataPtr =(unsigned int*) &(((char*)  elfData->d_buf)[sizeof(oldPageData)]);
                        for(i = 0; i<= numberUpdates; i++){
                                updateAddress = *dataPtr;
                                updateSize = *(++dataPtr);

                                updateOffset = updateAddress - shdr->sh_addr;
                      		printf(" TOUCHING ADDESS %x\n", updateAddress);
				fflush(stdout);
			        memcpy(&( oldPageData[updateOffset]),
                                        updateAddress,updateSize);
		        }
			 end test */


			/* probe memory to see if we own it */
			checkAddr = dladdr((void*)shdr->sh_addr, &dlip); 

			updateSize  = shdr->sh_size-((2*numberUpdates+1)* sizeof(unsigned int));
		
			if(!checkAddr){ 
				/* we dont own it,mmap it!*/

                        	mmapAddr = shdr->sh_offset + pageSize- (shdr->sh_offset % pageSize);
                        	mmapAddr =(unsigned int) mmap((void*) shdr->sh_addr,sizeof(oldPageData),
                                	PROT_READ|PROT_WRITE|PROT_EXEC,MAP_FIXED|MAP_PRIVATE,fd,mmapAddr);
			}else{
				/*we own it, finish the memcpy */
				memcpy(oldPageData, (void*) shdr->sh_addr, updateSize);
			}
	
			dataPtr =(unsigned int*) &(((char*)  elfData->d_buf)[sizeof(oldPageData)]);	
			/*apply updates*/
			for(i = 0; i<= numberUpdates; i++){
				updateAddress = *dataPtr; 
				updateSize = *(++dataPtr);

				updateOffset = updateAddress - shdr->sh_addr;
				/*do update*/	
				memcpy(&( oldPageData[updateOffset]),
					&(((char*)elfData->d_buf)[updateOffset]) , updateSize);	
			} 
			if(!checkAddr){
				mmapAddr = shdr->sh_offset + pageSize- (shdr->sh_offset % pageSize);
				mmapAddr =(unsigned int) mmap((void*) shdr->sh_addr,sizeof(oldPageData), 
					PROT_READ|PROT_WRITE|PROT_EXEC, MAP_FIXED| MAP_PRIVATE,fd,mmapAddr);
			}else{
				memcpy((void*) shdr->sh_addr, oldPageData, updateSize);
			}
		}

	}


        elf_end(elf);
        close(fd);

	return retVal;
}


#endif


/*
 * The Dyninst API arranges for this function to be called at the entry to
 * main().
 */
void DYNINSTinit(int cause, int pid)
{
    int calledByFork = 0, calledByAttach = 0;
	int isRestart=0;

    initFPU();


#if defined(sparc_sun_solaris2_4)
	/* this checks to see if this is a restart or a
	  	normal attach  ccw 19 nov 2001*/
	isRestart = checkElfFile();	
	if(isRestart){
		fflush(stdout);
		cause = 9;
	}
#endif

    DYNINSThasInitialized = 2;

   if (cause == 2) calledByFork = 1;
   else if (cause == 3) calledByAttach = 1;

    /* sanity check */
#if !defined(mips_unknown_ce2_11) /*ccw 15 may 2000 : 29 mar 2001*/
    assert(sizeof(int64_t) == 8);
    assert(sizeof(int32_t) == 4);
#endif

#ifndef mips_unknown_ce2_11 /*ccw 23 july 2001*/
    RTprintf("%s\n", V_libdyninstAPI_RT);
#endif

    DYNINSTos_init(calledByFork, calledByAttach);

#if !defined(mips_unknown_ce2_11) /*ccw 16 may 2000 : 29 mar 2001*/
    DYNINST_bootstrap_info.pid = getpid();
#endif
    DYNINST_bootstrap_info.ppid = pid;
    DYNINST_bootstrap_info.event = cause;

    DYNINST_mutatorPid = pid;
	
#ifndef i386_unknown_nt4_0 /*ccw 13 june 2001*/

#ifndef mips_unknown_ce2_11 
	if(isRestart==0){ /*ccw 19 nov 2001 */
	   DYNINSTbreakPoint();
	}
#else
	__asm("break 1"); /*ccw 25 oct 2000 : 29 mar 2001*/
	__asm("nop");
#endif

#endif

}

#if defined(i386_unknown_nt4_0)  /*ccw 13 june 2001*/
#include <windows.h>

/* this function is automatically called when windows loads this dll
 if we are launching a mutatee to instrument, dyninst will place
 the correct values in libdyninstAPI_RT_DLL_localPid and
 libdyninstAPI_RT_DLL_localCause and they will be passed to
 DYNINSTinit to correctly initialize the dll.  this keeps us
 from having to instrument two steps from the mutator (load and then 
 the execution of DYNINSTinit()
*/
BOOL WINAPI DllMain(
  HINSTANCE hinstDLL,  /* handle to DLL module */
  DWORD fdwReason,     /* reason for calling function */
  LPVOID lpvReserved   /* reserved */
){

	if(libdyninstAPI_RT_DLL_localPid != -1 || libdyninstAPI_RT_DLL_localCause != -1){
		DYNINSTinit(libdyninstAPI_RT_DLL_localCause,libdyninstAPI_RT_DLL_localPid);
	}
	return 1; 
}
 

#endif

#if !defined(i386_unknown_nt4_0)  && !defined(mips_unknown_ce2_11)/*ccw 2 may 2000 : 29 mar 2001*/
/*
 * handle vfork special case
 */
void DYNINSTvfork(int parent)
{
    /* sanity check */
    assert(sizeof(int64_t)  == 8);
    assert(sizeof(int32_t)  == 4);

    if (parent != getpid()) {
	DYNINST_bootstrap_info.pid = getpid();
	DYNINST_bootstrap_info.ppid = getppid();
	DYNINST_bootstrap_info.event = 3;
    } else {
	DYNINSTbreakPoint();
	DYNINST_bootstrap_info.event = 0;
    }
}
#endif
