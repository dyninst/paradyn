#if !defined(_netbsd_headers_h)
#define _netbsd_headers_h

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <memory.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <sys/resource.h>
#include <stdarg.h>
#include <sys/utsname.h>
#include <sys/stat.h>

#include <rpc/types.h>
#include <rpc/xdr.h>

typedef int (*P_xdrproc_t)(XDR*, ...);

/* POSIX */
inline void P_abort (void) { abort();}
inline int P_close (int FILEDES) { return (close(FILEDES));}
inline int P_dup2 (int OLD, int NEW) { return (dup2(OLD, NEW));}
inline int P_execvp (const char *FILENAME, char *const ARGV[]) {
  return (execvp(FILENAME, ARGV));}
inline void P__exit (int STATUS) { _exit(STATUS);}
inline int P_fcntl (int FILEDES, int COMMAND, int ARG2) {
  return (fcntl(FILEDES, COMMAND, ARG2));}
inline FILE * P_fdopen (int FILEDES, const char *OPENTYPE) {
  return (fdopen(FILEDES, OPENTYPE));}
inline FILE * P_fopen (const char *FILENAME, const char *OPENTYPE) {
  return (fopen(FILENAME, OPENTYPE));}
inline int P_fstat (int FILEDES, struct stat *BUF) { return (fstat(FILEDES, BUF));}
inline pid_t P_getpid () { return (getpid());}
inline int P_kill(pid_t PID, int SIGNUM) { return (kill(PID, SIGNUM));}
inline off_t P_lseek (int FILEDES, off_t OFFSET, int WHENCE) {
  return (lseek(FILEDES, OFFSET, WHENCE));}
inline int P_open(const char *FILENAME, int FLAGS, mode_t MODE) {
  return (open(FILENAME, FLAGS, MODE));}
inline int P_pclose (FILE *STREAM) { return (pclose(STREAM));}
inline FILE *P_popen (const char *COMMAND, const char *MODE) {
  return (popen(COMMAND, MODE));}
inline size_t P_read (int FILEDES, void *BUFFER, size_t SIZE) {
  return (read(FILEDES, BUFFER, SIZE));}
inline int P_uname(struct utsname *un) { return (uname(un));}
inline pid_t P_wait(int *status_ptr) { return (wait(status_ptr));}
inline int P_waitpid(pid_t pid, int *statusp, int options) {
  return (waitpid(pid, statusp, options));}
inline size_t P_write (int FILEDES, const void *BUFFER, size_t SIZE) {
  return (write(FILEDES, BUFFER, SIZE));}
inline int P_chdir(const char *path) { return (chdir(path)); }

/* ANSI */
inline void P_exit (int STATUS) { exit(STATUS);}
inline int P_fflush(FILE *stream) { return (fflush(stream));}
inline char * P_fgets (char *S, int COUNT, FILE *STREAM) {
  return (fgets(S, COUNT, STREAM));}
inline void * P_malloc (size_t SIZE) { return (malloc(SIZE));}
inline void * P_memcpy (void *A1, const void *A2, size_t SIZE) {
  return (memcpy(A1, A2, SIZE));}
inline void * P_memset (void *BLOCK, int C, size_t SIZE) {
  return (memset(BLOCK, C, SIZE));}
inline void P_perror (const char *MESSAGE) { perror(MESSAGE);}
typedef void (*P_sig_handler)(int);
inline P_sig_handler P_signal (int SIGNUM, P_sig_handler ACTION) {
  return (signal(SIGNUM, ACTION));}
inline char * P_strcat (char *TO, const char *FROM) {
  return (strcat(TO, FROM));}
inline char * P_strchr (const char *STRING, int C) {
  return (strchr(STRING, C));}
inline int P_strcmp (const char *S1, const char *S2) {
  return (strcmp(S1, S2));}
inline char * P_strcpy (char *TO, const char *FROM) {
  return (strcpy(TO, FROM));}
inline char *P_strdup(const char *S) { return (strdup(S));}
inline size_t P_strlen (const char *S) { return (strlen(S));}
inline char * P_strncat (char *TO, const char *FROM, size_t SIZE) {
  return (strncat(TO, FROM, SIZE)); }
inline int P_strncmp (const char *S1, const char *S2, size_t SIZE) {
  return (strncmp(S1, S2, SIZE));}
inline char * P_strncpy (char *TO, const char *FROM, size_t SIZE) {
  return (strncpy(TO, FROM, SIZE));}
inline char * P_strrchr (const char *STRING, int C) {
  return (strrchr(STRING, C));}
inline char * P_strstr (const char *HAYSTACK, const char *NEEDLE) {
  return (strstr(HAYSTACK, NEEDLE));}
inline double P_strtod (const char *STRING, char **TAILPTR) {
  return (strtod(STRING, TAILPTR));}
inline char * P_strtok (char *NEWSTRING, const char *DELIMITERS) {
  return (strtok(NEWSTRING, DELIMITERS));}
inline long int P_strtol (const char *STRING, char **TAILPTR, int BASE) {
  return (strtol(STRING, TAILPTR, BASE));}
inline unsigned long int P_strtoul(const char *STRING, char **TAILPTR, int BASE) { 
  return (strtoul(STRING, TAILPTR, BASE));}

/* BSD */
inline int P_accept (int SOCK, struct sockaddr *ADDR, size_t *LENGTH_PTR) {
  return (accept(SOCK, ADDR, (int*) LENGTH_PTR));}
inline int P_bind(int socket, struct sockaddr *addr, size_t len) {
  return (bind(socket, addr, len));}
inline int P_connect(int socket, struct sockaddr *addr, size_t len) {
  return (connect(socket, addr, len));}
inline struct hostent * P_gethostbyname (const char *NAME) {
  return (gethostbyname(NAME));}
inline int P_gethostname(char *name, size_t size) {
  return (gethostname(name, size));}
inline int P_getrusage(int i, struct rusage *ru) { 
  return (getrusage(i, ru));}
inline struct servent * P_getservbyname (const char *NAME, const char *PROTO) {
  return (getservbyname(NAME, PROTO));}
inline int P_getsockname (int SOCKET, struct sockaddr *ADDR, size_t *LENGTH_PTR) {
  return (getsockname(SOCKET, ADDR, (int*) LENGTH_PTR));}
/* inline int P_gettimeofday (struct timeval *TP, struct timezone *TZP) {
  return (gettimeofday(TP, TZP));} */
inline int P_listen (int socket, unsigned int n) { return (listen(socket, n));}
inline caddr_t P_mmap(caddr_t addr, size_t len, int prot, int flags, int fd, off_t off) {
  return (mmap(addr, len, prot, flags, fd, off));}
inline int P_munmap(caddr_t addr, int i) { return (munmap(addr, i));}
inline int P_socket (int NAMESPACE, int STYLE, int PROTOCOL) {
  return (socket(NAMESPACE, STYLE, PROTOCOL));}
inline int P_socketpair(int namesp, int style, int protocol, int filedes[2]) {
  return (socketpair(namesp, style, protocol, filedes));}
inline int P_pipe(int fds[2]) { return (pipe(fds)); }
inline int P_strcasecmp(const char *s1, const char *s2) {
  return (strcasecmp(s1, s2));}
inline int P_strncasecmp (const char *S1, const char *S2, size_t N) {
  return (strncasecmp(S1, S2,N));}
inline int P_endservent(void) { return endservent(); }

/* Ugly */
inline int P_ptrace(int req, pid_t pid, caddr_t addr, int data) {
  return (ptrace(req, pid, addr, data));}
inline int P_select(int wid, fd_set *rd, fd_set *wr, fd_set *ex, 
		    struct timeval *tm) {
  return (select(wid, rd, wr, ex, tm));}
inline int P_rexec(char **ahost, u_short inport, char *user,
		   char *passwd, char *cmd, int *fd2p) {
  abort();
}

extern void   P_xdr_destroy(XDR *x);
inline bool_t P_xdr_u_char(XDR *x, u_char *uc) { return (xdr_u_char(x, uc));}
inline bool_t P_xdr_int(XDR *x, int *i) { return (xdr_int(x, i));}
inline bool_t P_xdr_double(XDR *x, double *d) {
  return (xdr_double(x, d));}
inline bool_t P_xdr_u_int(XDR *x, u_int *u){
  return (xdr_u_int(x, u));}
inline bool_t P_xdr_float(XDR *x, float *f) {
  return (xdr_float(x, f));}
inline bool_t P_xdr_char(XDR *x, char *c) {
  return (xdr_char(x, c));}
inline bool_t P_xdr_string(XDR *x, char **h, const u_int maxsize) {
  return (xdr_string(x, h, maxsize));}
extern void P_xdrrec_create(XDR *x, const u_int send_sz, const u_int rec_sz,
			    const caddr_t handle, 
			    xdr_rd_func read_r, xdr_wr_func write_f);
			    extern bool_t P_xdrrec_endofrecord(XDR *x, int now);
			    extern bool_t P_xdrrec_skiprecord(XDR *x);
inline bool_t P_xdrrec_eof(XDR *x) { return (xdrrec_eof(x)); }

#endif
