/*
 * Copyright (c) 1996-2004 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as "Paradyn") on an AS IS basis, and do not warrant its
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

// $Id: main.C,v 1.155 2007/05/11 21:47:35 legendre Exp $

#include "common/h/headers.h"
#include "pdutil/h/makenan.h"
#include "common/h/Ident.h"

#define  DEBUG 1

extern "C" const char V_paradynd[];
Ident V_id(V_paradynd,"Paradyn");

#include "rtinst/h/rtinst.h"
#include "paradynd/src/comm.h"
#include "paradynd/src/internalMetrics.h"
#include "common/h/machineType.h"
#include "paradynd/src/init.h"
#include "paradynd/src/perfStream.h"
#include "paradynd/src/mdld.h"
#include "paradynd/src/processMgr.h"
#include "paradynd/src/pd_process.h"
#include "paradynd/src/processMetFocusNode.h"
#include "paradynd/src/debug.h"
#include "dyninstAPI/h/BPatch_function.h"

int StartOrAttach( void );


int StartRunPastMPIinit( pdvector<pdstring> &argv, pdstring dir  );

bool ok_toSendNewResources = false;


bool isInfProcAttached = false;
static bool reportedSelf = false;
bool startOnReportSelfDone = false;

bool runme_passed = false;
bool readyToRun = false;
pdRPC *tp = NULL;
pd_process * initial_process = NULL;

MRN::Stream * equivClassReportStream = NULL;
MRN::Stream * defaultStream = NULL;
unsigned assignedDaemonId = 0;
MRN::Network * ntwrk;
bool sdm_id_set = false;
unsigned sdm_id = 0;
pdstring machine_name_out;
pdstring program_name;
pdstring flavor_name;
pdstring MPIscript_name;

MRN::Rank myRank = 0;

static pdstring machine_name;
pdstring osName;
int pd_debug=0;

int ready;

unsigned SHARED_SEGMENT_SIZE = 2097152;

#if defined(os_windows)
const char V_paradynd[] = "$Paradyn: v5.0 paradynd #0 " __DATE__ __TIME__ "paradyn@cs.wisc.edu$";
#endif

/*
 * These variables are global so that we can easily find out what
 * machine/socket/etc we're connected to paradyn on; we may need to
 * start up other paradynds (such as on the CM5), and need this later.
 */
const int StartedByParadyn=0;
const int StartedByOther=1;
static int daemon_start_mode=StartedByParadyn;

pdstring pd_machine;
static int pd_attpid;
static int pd_known_socket_portnum=0;
pdstring pd_flavor;
pdstring MPI_impl;
pdstring newProcDir;
pdvector<pdstring> newProcCmdLine;
// Unused on NT, but this makes prototypes simpler.
int termWin_port = -1;

void configStdIO(bool closeStdIn)
{
    int nullfd;

    /* now make stdin, out and error things that we can't hurt us */
    if ((nullfd = open("/dev/null", O_RDWR, 0)) < 0) {
			abort();
    }

    if (closeStdIn) (void) dup2(nullfd, 0);
    (void) dup2(nullfd, 1);
    (void) dup2(nullfd, 2);

    if (nullfd > 2) close(nullfd);
}

void sigtermHandler()
{
  showErrorCallback(98,"paradynd has been terminated");
}

bool frontendExited = false;

// Now also cleans up shm segs by deleting all processes  -ari
void cleanUpAndExit(int status) {
   // Don't send anything more to the frontend, for safety.
   frontendExited = true;
   // TODO: replace this with a real check. But right now we only call
   // cleanUpAndExit if the frontend goes away.

   // If the process manager hasn't been initialized, then we can
   // skip the following.
   if (!isProcMgrInitialized())
      P_exit(status);

   processMetFocusNode::removeProcNodesToDeleteLater();
   processMgr::procIter itr = getProcMgr().begin();
   while(itr != getProcMgr().end()) {
      pd_process *theProc = *itr++;
      if (theProc == NULL)
         continue; // process has already been cleaned up
      getProcMgr().exitedProcess(theProc);
   }

   // We should really delete the pd_process objects for the processes that
   // have already exited when the those processes exit.  However, since
   // we're so close to the release, I don't feel we could flush out bugs
   // that might be related to doing this so for now, we're going to delete
   // these processes at the exit of Paradyn.
   pdvector<unsigned> pidToKill;
   pdvector<pd_process *> pd_processes_to_delete;
   getProcMgr().getExitedProcs(&pd_processes_to_delete);

   for(unsigned j=0; j<pd_processes_to_delete.size(); j++) {
      pd_process *theProc = pd_processes_to_delete[j];
      assert(theProc != NULL);
      
      // Try to be a bit smarter when we clean up the processes - kill
      // all processes that were created, leave all processes that were
      // attached to running.  This should really go into the process class,
      // but I hesitate to do that this close to the release (3.0)
      // -nick (24-Mar-2000)
      int pid = theProc->getPid();
      bool wasAttachedTo = theProc->wasCreatedViaAttach();

      if(!wasAttachedTo) {
         pidToKill.push_back(pid);
      }
      delete theProc; // calls pd_process::~pd_process, which fries the shm seg
   }

   for(unsigned k=0; k<pidToKill.size(); k++) {

#if defined (os_windows)
#else
      P_kill(pidToKill[k], 9);
#endif

   }

   P_exit(status);
}

bool
RPC_undo_arg_list (pdstring &flavor, unsigned argc, char **argv, 
		   pdstring &machine, int &well_known_socket,int &termWin_port,
		   int &flag, int &attpid,  pdstring &MPI_impl, pdstring &proc_dir)
{
  char *ptr;
  int c;
  extern char *optarg;
  bool err = false;
  const char optstring[] = "p:P:vVL:m:d:l:z:a:r:t:s:M:o:N:";

  // Defaults (for ones that make sense)
  machine = "localhost";
  flavor = "unix";
  proc_dir=getenv("PWD");
  flag = 2;
  well_known_socket = 0;
  termWin_port = 0;
  attpid = 0;
  bool stop = false;
  while ( (c = P_getopt(argc, argv, optstring)) != EOF && !stop)
    switch (c) {
    case 'p':
      // Port number to connect to for the Paradyn command port
      well_known_socket = P_strtol(optarg, &ptr, 10);
      if (ptr == optarg) {
         fprintf(stderr, "%s[%d]:  bad conversion for string %s\n", FILE__, __LINE__, optarg);
         err = true;
      }
      break;
    case 'P':
      termWin_port = P_strtol(optarg, &ptr, 10);
      if (ptr == optarg) {
        fprintf(stderr, "%s[%d]:  bad conversion for string %s\n", FILE__, __LINE__, optarg);
        err = true;
      }
      break;
    case 'v':
      // Obsolete argument
      err = true;
      break;
    case 'V':
      cout << V_id << endl;
      break;
    case 'L':
      // Debugging flag to specify runtime library
      pd_process::defaultParadynRTname = optarg;
      break;
    case 'm':
      // Machine specification. We could do "machine:portname", but there are
      // two ports. 
      machine = optarg;
      break;
    case 'd':
      // directory from which to launch mpi jobs
      proc_dir = optarg;
      break;
    case 'l':
      flag = P_strtol(optarg, &ptr, 10);
      if (ptr == optarg) err = true;
      break;
    case 'z':
      flavor = optarg;
      break;
    case 'a':
      attpid = P_strtol(optarg, &ptr, 10);
      break;
    case 'r':
      // We've hit the "runme" parameter. Stop processing
      stop = true;
      runme_passed = true;
      break;
    case 't':
      // MAX_NUMBER_OF_THREADS is found in pd_process.h
      MAX_NUMBER_OF_THREADS = P_strtol(optarg, &ptr, 10);
      break;
    case 's':
      SHARED_SEGMENT_SIZE = P_strtol(optarg, &ptr, 10);
      break;
    case 'M':
      //the MPI implementation - MPICH or LAM
      MPI_impl = optarg;
      break;
    case 'N':
       MPIscript_name = optarg;
       break;
    default:
      fprintf(stderr, "%s[%d]:  no such options -%c\n", FILE__, __LINE__, c);
      err = true;
      break;
    }
  if (err) {
    fprintf(stderr, "%s[%d]:  error parsing args for paradynd\n", FILE__, __LINE__);
    return false;
  }

  // verify required parameters
  // This define should be something like ENABLE_TERMWIN, but hey
#if !defined(i386_unknown_nt4_0)
  if (!termWin_port)
    fprintf(stderr, "%s[%d]:  No termwin port specified\n", FILE__, __LINE__);
  if (!termWin_port)
    return false;
#else
	// if (!well_known_socket)
  //  return false;
#endif
  return true;
}

//added for RMA window support and MPI communicator support
//in the instrumentation code (rtinst/src/RTinst.c), we need to know
//which MPI implementation is used.
void RPC_do_environment_work(pdstring pd_flavor, pdstring MPI_impl){
  if(pd_flavor == "mpi"){
    char * temp = (char *)malloc((strlen("PARADYN_MPI=") + MPI_impl.length() + 1)*sizeof(char)) ;
    sprintf(temp, "PARADYN_MPI=%s",MPI_impl.c_str());
    //cerr<<"temp is "<<temp<<endl;
    putenv(temp);
  }
}
void RPC_undo_environment_work(){
   putenv("PARADYN_MPI=");
}

static void PauseIfDesired( void )
{
	char *pdkill = getenv( "PARADYND_DEBUG" );
   if( (pdkill && ( *pdkill == 'y' || *pdkill == 'Y' )))
	{
		int pid = getpid();
		cerr << "breaking for debug in controllerMainLoop...pid=" << pid << endl;
#if defined(i386_unknown_nt4_0)
		DebugBreak();
#else
		bool bCont = false;
		while( !bCont )
		{
			sleep( 1 );
		}
#endif
	}
}


#if !defined(i386_unknown_nt4_0)
static
void
InitSigTermHandler( void )
{
    struct sigaction act;

    // int i = 1;
    // while (i) {};

    act.sa_handler = (void (*)(int)) sigtermHandler;
    act.sa_flags   = 0;

    /* for AIX - default (non BSD) library does not restart - jkh 7/26/95 */
#if defined(SA_RESTART)
    act.sa_flags  |= SA_RESTART;
#endif

    sigfillset(&act.sa_mask);

    if (sigaction(SIGTERM, &act, 0) == -1) {
        perror("sigaction(SIGTERM)");
        abort();
    }
}
#endif // !defined(i386_unknown_nt4_0)

#if defined(i386_unknown_nt4_0)
static
void
InitWinsock( void )
{
    // Windows NT needs to initialize winsock library
    WORD wsversion = MAKEWORD(2,0);
    WSADATA wsadata;
    WSAStartup(wsversion, &wsadata);
}
#endif // defined(i386_unknown_nt4_0)

static void parseScript(pdstring &host, int &port, int my_mpi_rank)
{
   char line[512];
   char hostname[256];
   int port_num;
   int mrn_rank;
   FILE *f = fopen(MPIscript_name.c_str(), "r");
   if (!f)
   {
      fprintf(stderr, "Could not read %s:", MPIscript_name.c_str());
      perror("");
      exit(-1);
   }
   
   while (fgets(line, 512, f))
   {
      if (strstr(line, "# MRN") != line)
         continue;
      sscanf(line, "# MRN %256s %d %d\n", hostname, &port_num, &mrn_rank);
      if (my_mpi_rank == mrn_rank) {
          sdm_id = mrn_rank;
          sdm_id_set = true;
          host = hostname;
          port = port_num;
          fclose(f);
          return;
      }
   }
   fprintf(stderr, "[%s:%u] - Internal error, could not get MRNet topology info\n",
           __FILE__, __LINE__ );
   exit(-1);
}

static void InitForMPI( pdstring& pd_machine, int &pd_port, int &pd_rank ) {
	// Both IRIX and AIX MPI job-launchers will start paradynd,
	// which must report to paradyn
	// the pdRPC is allocated and reportSelf is called

	pd_rank = StartRunPastMPIinit(newProcCmdLine, newProcDir);
	parseScript(pd_machine, pd_port, pd_rank);
	tp = new pdRPC(AF_INET, pd_port, SOCK_STREAM, pd_machine, NULL, NULL, 2);
	assert( tp != NULL );
	sdm_id = pd_rank;
	sdm_id_set = true;
    reportedSelf = true;
}

static void InitDefault( const pdstring& pd_machine )
{ 
    tp = new pdRPC( AF_INET, pd_known_socket_portnum, SOCK_STREAM, pd_machine,
                    NULL, NULL, 2 );
    assert( tp != NULL );
}

#if !defined(os_windows)
void sighup_handler( int ) {
   // we get SIGHUP when Paradyn closes our connection
   // we want to ignore this, and close 
}

void sighupInit() {

   // ensure that we don't die from SIGHUP when our connection
   // to Paradyn closes
   struct sigaction act;
   
   act.sa_handler = sighup_handler;
   act.sa_flags = 0;
   sigfillset( &act.sa_mask );
   
   if( sigaction( SIGHUP, &act, 0 ) == -1 )
   {
      cerr << "Failed to install SIGHUP handler: " << errno << endl;
      // this isn't a fatal error for us
   }
}
#endif

//
// Note: the daemon_start_mode variable is set from the argument to the -l command
// line switch.  It has the following meaning:
//
// daemon_start_mode == 0 => daemon started by Paradyn
//-----------------------------------------------------------------
// daemon_start_mode == 1 => daemon started manually
//


int
main( int argc, char* argv[] )
{
	PauseIfDesired();
	init_daemon_debug();
	
#if !defined(i386_unknown_nt4_0)
    InitSigTermHandler();
#endif // defined(i386_unknown_nt4_0)
   
   
#if defined(i386_unknown_nt4_0)
    InitWinsock();
#endif // defined(i386_unknown_nt4_0)
  
  
   // process command line args passed in
   pd_process::programName = argv[0];
   bool aflag;

   aflag = RPC_undo_arg_list (pd_flavor, argc, argv, pd_machine,
                              pd_known_socket_portnum, termWin_port, 
                              daemon_start_mode, pd_attpid, MPI_impl, newProcDir );

   if (!aflag || pd_debug )
   {
      if (!aflag)
      {
         cerr << "Invalid/incomplete command-line args:" << endl;
      }
      cerr << "   -z<flavor";
      if (pd_flavor.length())
      {
         cerr << "=" << pd_flavor;
      }
      cerr << "> -l<flag";
      if (daemon_start_mode)
      {
         cerr << "=" << daemon_start_mode;
      }
      cerr << "> -m<hostmachine";
      if (pd_machine.length()) 
      {
         cerr << "=" << pd_machine;
      }
      cerr << "> -p<hostport";
      if (pd_known_socket_portnum)
      {
         cerr << "=" << pd_known_socket_portnum;
      }
      cerr << "> -apid<attachpid";
      if (pd_attpid)
      {
         cerr << "=" << pd_attpid;
      }
      cerr << "> -M<MPI_impl";
      if (MPI_impl.length())
      {
         cerr << "=" << MPI_impl;
      }
      cerr << "> -d<MPI_procdir";
      if (newProcDir.length())
      {
         cerr << "=" << newProcDir;
      }
      cerr << ">" << endl;

      if (pd_process::defaultParadynRTname.length())
      {
         cerr << "   -L<library=" << pd_process::defaultParadynRTname << ">" << endl;
      }
      if (!aflag)
      {
         cleanUpAndExit(-1);
      }
   }


#if !defined(i386_unknown_nt4_0)
   extern PDSOCKET connect_Svr(pdstring machine,int port);
   PDSOCKET stdout_fd=INVALID_PDSOCKET;
   if ((stdout_fd = connect_Svr(pd_machine,termWin_port)) == INVALID_PDSOCKET)
      cleanUpAndExit(-1);

   if (write(stdout_fd,"from_paradynd\n",strlen("from_paradynd\n")) <= 0)
      cleanUpAndExit(-1);

   //for MPICH2  - MPICH2 mpd selects on the stdout fd
   //of its children. If it is closed then EOF is detected by select.  MPICH2
   //mpd begins a shutdown sequence if it detects EOF on the fd.
   //paradynd/dyninstAPI error messages through stdout should still be seen by
   //the user, because MPICH2 mpd passes the stdout to the console

	 if(MPI_impl != "MPICH")
		 dup2(stdout_fd,1);
	 dup2(stdout_fd,2);
#endif

#if !defined(i386_unknown_nt4_0)
   aflag = RPC_make_arg_list(pd_process::arg_list,
                             pd_known_socket_portnum, termWin_port,daemon_start_mode, 0,
                             pd_machine, true);
#else
   aflag = RPC_make_arg_list(pd_process::arg_list,
                             pd_known_socket_portnum, daemon_start_mode, 0,
                             pd_machine, true);
#endif 
   RPC_do_environment_work(pd_flavor, MPI_impl);
	 assert(aflag);
   pdstring flav_arg(pdstring("-z")+ pd_flavor);
   pd_process::arg_list.push_back(flav_arg);

   machine_name = getNetworkName();

   // Put the inferior application and its command line
   // arguments into the command line. Basically, loop through argv until
   // we find -runme, and put everything after it into the command line
   unsigned int argNum = 0;
   while ((argNum < (unsigned int)argc) && (strcmp(argv[argNum], "-runme")))
   {
      argNum++;
   }
   // Okay, argNum is the command line argument which is "-runme" - skip it
   argNum++;
   // Copy everything from argNum to < numberOfArgs
   // this is the command that is to be issued

	int numberOfArgs = argc;
   if (!MPIscript_name.length())
   {
      numberOfArgs -= 3;
   }

   for (unsigned int i = argNum; i < (unsigned int)numberOfArgs; i++)
   {
       newProcCmdLine += argv[i];
   }
   // note - newProcCmdLine could be empty here, if the -runme flag is not given
   // There are several ways that we might have been started.
   // We need to connect to Paradyn differently depending on which 
   // method was used.
   //
   // Use our own stdin if:
   //   started as local daemon by Paradyn using fork+exec
   //
   // Use a socket described in our command-line args if:
   //   started as remote daemon by rsh/rexec
   //   started manually on command line
   //   started by MPI
   //   started as PVM daemon by another Paradyn daemon using pvm_spawn() 
   //
    

   pd_process::pdFlavor = pd_flavor;
   machine_name_out = machine_name;
   program_name = argv[0];
   flavor_name = pd_flavor;

   pdstring parHostname;
   MRN::Port parPort = 0;

   //********************************************
   if( tp == NULL ) {
      // we haven't yet reported to our front end     
      if( pd_flavor == "mpi" ) {
         int port;
         int rank;
         InitForMPI( parHostname, port, rank );
         parPort = port;
         myRank = rank;
      }
      else {
         // we are a daemon started by rsh/rexec or manually
         InitDefault( pd_machine );
      }
   }
   
   if( tp == NULL ) {
      if( (daemon_start_mode != 0) || (daemon_start_mode != 1) ) {
         cerr << "Paradyn daemon: invalid -l value " << daemon_start_mode << " given." 
              << endl;
      }
      else {
         cerr << "Paradyn daemon: invalid command-line options seen" << endl;
      }
      cleanUpAndExit(-1);
   }
   assert( tp != NULL );

   //----------------------------------------------------------------
    if( argc < 3 ) {
        fprintf(stderr, "usage: %s parent_hosntame parent_port my_rank\n",
                argv[0]);
        exit( -1 );
    }
	 
	 //Get MRNet output level
	 char* temp = (char *) getenv("MRNET_OUTPUT_LEVEL");

	 if (temp != NULL) {
         MRN::set_OutputLevel( atoi(temp) );
     }

    if( pd_flavor != "mpi" )
    {
       parHostname = argv[argc-4];
       char *portstr = NULL;
       if (argv[argc-2][0] == '-' && argv[argc-2][1] == 'p')
         portstr = argv[argc-3] +2;
       else
         portstr = argv[argc-3];
       parPort = (MRN::Port)strtoul( portstr/*argv[argc-2]*/, NULL, 10 );

       if (argv[argc-2][0] == '-' && argv[argc-1][1] == 'P')
         portstr = argv[argc-2] +2;
       else
         portstr = argv[argc-1];
       myRank = (MRN::Rank)strtoul( portstr/*argv[argc-1]*/, NULL, 10 );
    }

    if (parHostname[0] == '-' && parHostname[1] == 'm') {
       fprintf(stderr, "%s[%d]:  WARNING:  bad hostname %s!\n", FILE__, __LINE__, parHostname.c_str());
       pdstring temp = parHostname.c_str() + 2;
       parHostname = temp;
       fprintf(stderr, "%s[%d]:  changed to %s\n", FILE__, __LINE__, parHostname.c_str());
    }

    startup_printf("%s[%d]:  about to do MRN::Network(%s, %d, %d)\n", 
            FILE__, __LINE__, 
            parHostname.c_str(), parPort, myRank);

    char myHostname[256];
    if( gethostname( myHostname, sizeof(myHostname)) == -1 ){
        perror("gethostname()");
        exit(-1);
    }
#if 0
    ntwrk = new MRN::Network( parHostname.c_str(), parPort, myHostname, myRank );
    if( ntwrk->fail() ) {
        fprintf(stderr, "backend_init() failed\n");
        exit(-1);
    }
#else
    fprintf(stderr, "%s[%d]:  FIXME:  commented out MRNet init\n", FILE__, __LINE__);
#endif

    //----------------------------------------------------------------
    // Wait here for a stream

	 tp->wait_for(ntwrk, T_dyninstRPC::setDaemonDefaultStream_REQ);

	 tp->wait_for(ntwrk, T_dyninstRPC::getDaemonInfo_REQ);

	 tp->wait_for(ntwrk, T_dyninstRPC::setEquivClassReportStream_REQ);

	 pdstatusLine(V_paradynd);

   // Note -- it is important that this daemon receives all mdl info
   // before starting a process
   aflag = mdl_get_initial(pd_flavor, tp);
   assert(aflag);
   assert( saw_mdl == true );
   
#if defined(i386_unknown_linux2_0) || defined(ia64_unknown_linux2_4)
	 sighupInit();
#endif
	 
	 // Set up the trace socket. This is needed before we try
	 // to attach to a process
	 //extern void setupTraceSocket();

	 //setupTraceSocket();
	 
	 if (!paradyn_init()) 
		 {
			 cerr << "Paradyn init failed\n"<<endl;
			 abort();
		 }

	 // check whether we are supposed to start or attach to the process now
	 // otherwise, we wait till we get the reportSelfDone call from the
	 // front-end
#if !defined(NO_SYNC_REPORT_SELF)
	 if( reportedSelf )
		 {
			 // we want to use the synchronous reportSelfDone call
			 startOnReportSelfDone = true;
		 }
#endif // defined(NO_SYNC_REPORT_SELF)
	 
			 // start or attach to the process now if desired

	 if( pd_flavor == "mpi" && runme_passed)
		 startOnReportSelfDone = false;

	 if( !startOnReportSelfDone ) {
         int soaRet = StartOrAttach();
         if( soaRet != 0 ) {
             return soaRet;
         }
     }


     // start handling requests
     controllerMainLoop( true );
	 
     RPC_undo_environment_work();
     return 0;
}

BPatch_process *MPI_proc = NULL;

#define MPI_COMM_WORLD_LAM 134947264
#define MPI_COMM_WORLD_MPICH 91

static int break_at_mpi_init(BPatch_process *bproc)
{
	extern pdstring MPI_impl;
	unsigned long mpi_comm_world;

	if(MPI_impl == "LAM")
	{
		/* For LAM 7.1.2, at least, and probably others, MPI_COMM_WORLD is
		   #define'd to be the address of 'lam_mpi_comm_world'. */
		BPatch_variableExpr * mcw = bproc->getImage()->findVariable( "lam_mpi_comm_world" );
		if( mcw == NULL ) {
			assert( "Unable to locate lam_mpi_comm_world in LAM mpi daemon." );
			}
		mpi_comm_world = (unsigned long) mcw->getBaseAddr();
	}
	else if(MPI_impl == "MPICH")
	{
		mpi_comm_world = MPI_COMM_WORLD_MPICH;
	}
	else
	{
		assert(0);
	}

  int my_rank = -1;

  BPatch_image *img = bproc->getImage();

  BPatch_Vector<BPatch_function *> mpi_inits;

  img->findFunction("MPI_Init", mpi_inits);

  assert(mpi_inits.size() == 1); //TODO: Make me a real error

  BPatch_function *mpi_init = mpi_inits[0];

  BPatch_Vector<BPatch_point *> *exit_points;
  exit_points = mpi_init->findPoint(BPatch_exit);
  
  BPatch_Vector<BPatch_function *> mpi_comm_ranks;
	img->findFunction("MPI_Comm_rank", mpi_comm_ranks);
  assert(mpi_comm_ranks.size() == 1); //TODO: Fix me too
  BPatch_function *mpi_comm_rank = mpi_comm_ranks[0];

  BPatch_Vector<BPatch_function *> dyninst_breakpoints;
  img->findFunction("DYNINST_snippetBreakpoint", dyninst_breakpoints);
  assert(dyninst_breakpoints.size() == 1); //TODO: me too

  BPatch_function *dyninst_breakpoint = dyninst_breakpoints[0];
  BPatch_variableExpr *var = bproc->malloc(sizeof(int));

  int negone = -1;
  var->writeValue(&negone, sizeof(int), false);


  for (unsigned i=0; i<exit_points->size(); i++) {
      // Call MPI_COMM_WORLD
      BPatch_Vector<BPatch_snippet *> args;
      BPatch_constExpr arg1(mpi_comm_world); //MPI_COMM_WORLD value
      BPatch_constExpr arg2((unsigned long) var->getBaseAddr());
      args.push_back(&arg1);
      args.push_back(&arg2);
      BPatch_funcCallExpr call_to_rank(*mpi_comm_rank, args);
      
      BPatch_Vector<BPatch_snippet *> dbp_args;
      BPatch_funcCallExpr call_to_break(*dyninst_breakpoint, dbp_args);
      
      bproc->insertSnippet(call_to_rank, exit_points[i], BPatch_callAfter, BPatch_firstSnippet);
      
      bproc->insertSnippet(call_to_break, exit_points[i], BPatch_callAfter, BPatch_lastSnippet);
      
  }
  /*
    bproc->continueExecution();
    do {
    BPatch::bpatch->waitForStatusChange();
    if (bproc->isTerminated())
    break;
    if (my_rank == -1)
    var->readValue(&my_rank, sizeof(int));
    } while (my_rank == -1);
    //} while (!((my_rank != -1) && !bproc->isStopped()));
    */
  
  bproc->continueExecution();
  
  
  for (;;) {
      BPatch::getBPatch()->waitForStatusChange();
      if (bproc->isTerminated())
          break;
      if (!bproc->isStopped())
          continue;
      var->readValue(&my_rank, sizeof(int));
      if (my_rank != -1)
          break;
      bproc->continueExecution();
  }
  
  return my_rank;
}

int StartRunPastMPIinit( pdvector<pdstring> &argv, pdstring dir)
{
    initBPatch();
	initProcMgr();

	char **argv_array = new char*[argv.size()+1];
	for(unsigned i=0; i<argv.size(); i++)
		argv_array[i] = const_cast<char *>(argv[i].c_str());
	argv_array[argv.size()] = NULL;
	char *path = new char[  argv[0].length() + 5];

	strcpy(path, argv[0].c_str());

    if ((dir.length() > 0) && (P_chdir(dir.c_str()) < 0)) {
        sprintf(errorLine, "cannot chdir to '%s': %s\n", dir.c_str(), 
                strerror(errno));
        pdlogLine(errorLine);
        P__exit(-1);
    }
			
	//TODO: Change stdout to go to termwin

        // hand off info about how to start a paradynd to the application.
        //   used to catch rexec calls, and poe events.
        //
        char* paradynInfo = new char[1024];
        sprintf(paradynInfo, "PARADYN_MASTER_INFO= ");
        for (unsigned i=0; i < pd_process::arg_list.size(); i++) {
           const char *str;

           str = P_strdup(pd_process::arg_list[i].c_str());
           if (!strcmp(str, "-l1")) {
              strcat(paradynInfo, "-l0");
           } else {
              strcat(paradynInfo, str);
           }
           strcat(paradynInfo, " ");
        }

        fprintf(stderr, "%s[%d]:  setting '%s' in env\n", FILE__, __LINE__, paradynInfo);
        //  since we are not using the envp arg to processCreate, we can just stick
        //  it in our own environment and it will be transferred
        P_putenv(paradynInfo);

	MPI_proc = getBPatch().processCreate(path, 
                                         (const char **) argv_array, NULL, 
                                         0, 1,2);

	//TODO: Change stdout to go to termwin
	//MPI_proc = createBPProc(argv[0], argv, dir, 0, 1, 2);
	
	//TODO: Check for error
    
	getBPatch().registerExecCallback(pd_process::paradynExecDispatch);
	getBPatch().registerPostForkCallback(pd_process::paradynPostForkDispatch);
	getBPatch().registerExitCallback(pd_process::paradynExitDispatch);

	int rank = break_at_mpi_init(MPI_proc);
		
	return rank;
}

int
StartOrAttach( void )
{
	// spawn the given process, if necessary
	if (newProcCmdLine.size() && (pd_attpid==0))
	{
		// ignore return val (is this right?)
		pd_process *p =  pd_createProcess(newProcCmdLine, newProcDir);		
		if( pd_flavor == "mpi" && runme_passed)
		{
			metricFocusNode::handleNewProcess(p);
			resource::report_ChecksumToFE( );
		}
	} 
	else if (pd_attpid && (daemon_start_mode==StartedByOther))
	{
		// We attach after doing a last bit of initialization, below
      pd_process *p = pd_attachProcess("", pd_attpid);
      
      if (!p) 
         return -1;
      p->pauseProc();
   }
   
   isInfProcAttached = true;
   return 0;
}

