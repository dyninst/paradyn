
#ifndef RPC_UTIL
#define RPC_UTIL

/*
 * $Log: rpcUtil.h,v $
 * Revision 1.15  1994/03/31 22:59:04  hollings
 * added well known port as a paramter to xdrRPC constructor.
 *
 * Revision 1.14  1994/03/25  16:07:31  markc
 * Added option to specify timeout on readReady.
 *
 * Revision 1.13  1994/03/20  01:45:23  markc
 * Changed xdr_Boolean to char rather than int to reflect Boolean change.
 *
 * Revision 1.12  1994/03/11  21:01:24  hollings
 * Changed Boolean from int to char to match X11 convention.
 *
 *
 */

/* prevents malloc from being redefined */
#ifdef MIPS
#define MALLOC_DEFINED_AS_VOID
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/file.h>

extern "C" {
#include <rpc/types.h>
#include <rpc/xdr.h>
}

// functions that g++-fixincludes missed
#ifdef MIPS
void bzero (char*, int);
int select (int, fd_set*, fd_set*, fd_set*, struct timeval*);
char *strdup (char*);
int gethostname(char*, int);
int socket(int, int, int);
int bind(int s, struct sockaddr *, int);
int getsockname(int, struct sockaddr*, int *);
int listen(int, int);
int connect(int s, struct sockaddr*, int);
int socketpair(int, int, int, int sv[2]);
int vfork();
int accept(int, struct sockaddr *addr, int *);
#endif

#define xdr_Boolean xdr_char
typedef XDR *XDRptr;
typedef int (*xdrIOFunc)(int handle, char *buf, unsigned int len);

typedef char Boolean;
typedef char *String;

//
// Functions common to server and client side.
//
class XDRrpc {
  public:
    XDRrpc(char *m, char *u, char *p, xdrIOFunc, xdrIOFunc, 
	   char **arg_list=0, int nblock=0, int wellKnownPortFd = 0);
    XDRrpc(int fd, xdrIOFunc readRoutine, xdrIOFunc writeRoutine, int nblock=0);
    XDRrpc(int family, int port, int type, char *machine, xdrIOFunc readFunc,
	   xdrIOFunc writeFunc, int nblock=0);
    virtual ~XDRrpc();
    void closeConnect() { if (fd >= 0) close(fd); fd = -1;}
    void setNonBlock();
    int get_fd() { return fd;}
    int readReady(int timeout=0);
    XDR *__xdrs__;
    int fd;
    int pid;		// pid of child;
    static int __wellKnownPortFd__;
    // should the fd be closed by a destructor ??
};

class THREADrpc {
  public:
    THREADrpc(int tid);
    void setNonBlock() { ; }
    void setTid(int id) { tid = id; }
  protected:
    int tid;
    // these are only to be used by implmentors of thread RPCs.
    //   the value is only valid during a thread RPC.
    unsigned int requestingThread;
    unsigned int getRequestingThread()	{ return requestingThread; }
};


//
// client side common routines that are transport independent.
//
class RPCUser {
  public:
    void awaitResponce(int tag);
    void verifyProtocolAndVersion();
    int callErr;
};

//
// server side routines that are transport independent.
//
class RPCServer {
  public:
	int __versionVerifyDone__;
};

extern int RPC_readReady (int fd, int timeout=0);
extern int RPC_setup_socket (int *sfd,   // return file descriptor
			     int family, // AF_INET ...
			     int type);   // SOCK_STREAM ...
extern int xdr_String(XDR*, String*);
extern int RPCprocessCreate(int *pid, char *hostName, char *userName,
			    char *commandLine, char **arg_list = 0,
			    int wellKnownPort = 0);
extern char **RPC_make_arg_list (char *program, int family, int type, int port, int flag);
extern int 
RPC_undo_arg_list (int argc, char **arg_list, char **machine, int &family,
		   int &type, int &well_known_socket, int &flag);
extern int RPC_getConnect (int fd);

#endif
