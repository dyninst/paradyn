#include "test_mem_util.h"
#include "mutatee_util.h"

/* Sun Forte/WorkShop cc releases older than 6.2 do not like these defines: */
/* (macro versions of these calls are in test_mem_util.h) */
#if defined(__SUNPRO_C) && (__SUNPRO_C < 0x530)
void passorfail(int i, int p, char* d, char* r)
{
  if(p) {
    logerror("Passed test #%d (%s)\n", (i), (d));
    passedTest[(i)] = TRUE;
  } else {
    logerror("\n**Failed** test #%d (%s): %s\n", (i), (d), (r));
  }
}

void skiptest(int i, char* d)
{
  logerror("Skipping test #%d (%s)\n", (i), (d));
  logerror("    not implemented on this platform\n");
  passedTest[(i)] = TRUE;
}
#endif

int result_of_loadsnstores;

unsigned int loadCnt = 0;
unsigned int storeCnt = 0;
unsigned int prefeCnt = 0;
unsigned int accessCnt = 0;

unsigned int accessCntEA = 0;
unsigned int accessCntBC = 0;
int doomEA = 0;
int doomBC = 0;
void* eaList[1000];
unsigned int bcList[1000];
void* eaExp[1000];

unsigned int accessCntEAcc = 0;
unsigned int accessCntBCcc = 0;
int doomEAcc = 0;
int doomBCcc = 0;
void* eaListCC[1000];
unsigned int bcListCC[1000];
void* eaExpCC[1000];
unsigned int bcExpCC[1000];

#ifdef sparc_sun_solaris2_4
/* const */ unsigned int loadExp=15;
/* const */ unsigned int storeExp=13;
/* const */ unsigned int prefeExp=2;
/* const */ unsigned int accessExp=26;
/* const */ unsigned int accessExpCC=26;

unsigned int bcExp[] = { 4,1,2,8,4,1,1,  4,8,4,  4,4,8,8,16,
                         0,0,  1,2,4,8,  4,8,16,4,8 };

int eaExpOffset[] =    { 0,3,2,0,0,3,3,  0,0,0,  0,0,0,0,0,
                         0,0,  7,6,4,0,  0,0,0,4,0 };

/* _inline */ void init_test_data()
{
  int i=0;

  /*
  dprintf("&divarw = %p\n", &divarw);
  dprintf("&dfvars = %p\n", &dfvars);
  dprintf("&dfvard = %p\n", &dfvard);
  dprintf("&dfvarq = %p\n", &dfvarq);
  */

  for(; i<10; ++i)
    eaExp[i] = (void*)((unsigned long)&divarw + eaExpOffset[i]);

  for(; i<12; ++i)
    eaExp[i] = (void*)((unsigned long)&dfvars + eaExpOffset[i]);

  for(; i<14; ++i)
    eaExp[i] = (void*)((unsigned long)&dfvard + eaExpOffset[i]);

  for(; i<17; ++i)
    eaExp[i] = (void*)((unsigned long)&dfvarq + eaExpOffset[i]);

  for(; i<21; ++i)
    eaExp[i] = (void*)((unsigned long)&divarw + eaExpOffset[i]);

  eaExp[i] = (void*)((unsigned long)&dfvars + eaExpOffset[i]);
  ++i;

  eaExp[i] = (void*)((unsigned long)&dfvard + eaExpOffset[i]);
  ++i;

  eaExp[i] = (void*)((unsigned long)&dfvarq + eaExpOffset[i]);
  ++i;

  for(; i<26; ++i)
    eaExp[i] = (void*)((unsigned long)&divarw + eaExpOffset[i]);

  /* Duplicate the stream for cc */
  for(i=0; i<accessExp; ++i) {
    eaExpCC[i] = eaExp[i];
    bcExpCC[i] = bcExp[i];
  }
}
#endif /* defined(sparc_sun_solaris2_4) */

#ifdef rs6000_ibm_aix4_1
const unsigned int loadExp=41;
const unsigned int storeExp=32;
const unsigned int prefeExp=0;
const unsigned int accessExp=73;
const unsigned int accessExpCC=73;

unsigned int bcExp[] = { 4,  1,1,1,1,  2,2,2,2,  2,2,2,2,  4,4,4,4,
			 4,4,4,  8,8,8,8,  1,1,1,1,  2,2,2,2,
			 4,4,4,4,  8,8,8,8,  2,4,2,4,  76,76,24,20,
			 20,20,  4,4,8,8,  4,  4,4,4,4,  4,  8,8,8,8,
			 4,4,4,4,  8,8,8,8,  4 };

int eaExpOffset[] =    { 0, 17,3,1,2,  0,4,2,0,  2,2,2,2,  0,4,4,4,
			 4,12,2,  0,0,0,0,  3,1,1,1,  2,6,2,2,
			 0,4,4,4,  0,0,0,0,  0,0,0,0,  -76,-76,-24,-24,
			 -20,-20,    0,0,0,0,  0,  0,4,0,4,  0,  0,8,8,8,
			 4,4,0,0,  0,8,8,8,  0 };

/* _inline */ void init_test_data()
{
  int i;

  void *toc = gettoc();
  void *sp  = getsp(1,2,3);

  dprintf("&divarw = %p\n", &divarw);
  dprintf("&dfvars = %p\n", &dfvars);
  dprintf("&dfvard = %p\n", &dfvard);

  dprintf("toc = %p\n", toc);
  dprintf("sp = %p\n", sp);

  eaExp[0] = toc; /* assuming that TOC entries are not reordered */

  for(i=1; i<44; ++i)
    eaExp[i] = (void*)((unsigned long)&divarw + eaExpOffset[i]);

  for(i=44; i<50; ++i)
    eaExp[i] = (void*)((unsigned long)sp + eaExpOffset[i]);; /* SP */
  
  for(i=50; i<54; ++i)
    eaExp[i] = (void*)((unsigned long)&divarw + eaExpOffset[i]);

  eaExp[54] = (void*)((unsigned long)toc + sizeof(void*)); /* TOC */

  for(i=55; i<59; ++i)
    eaExp[i] = (void*)((unsigned long)&dfvars + eaExpOffset[i]);

  eaExp[59] = (void*)((unsigned long)toc + 2*sizeof(void*)); /* TOC */

  for(i=60; i<64; ++i)
    eaExp[i] = (void*)((unsigned long)&dfvard + eaExpOffset[i]);

  for(i=64; i<68; ++i)
    eaExp[i] = (void*)((unsigned long)&dfvars + eaExpOffset[i]);

  for(i=68; i<72; ++i)
    eaExp[i] = (void*)((unsigned long)&dfvard + eaExpOffset[i]);
  
  eaExp[72] = (void*)((unsigned long)&dfvars + eaExpOffset[i]);


  /* Duplicate the stream for cc */
  for(i=0; i<accessExp; ++i) {
    eaExpCC[i] = eaExp[i];
    bcExpCC[i] = bcExp[i];
  }
}
#endif /* defined(rs6000_ibm_aix4_1) */

#if defined(i386_unknown_linux2_0) \
 || defined(i386_unknown_nt4_0)
unsigned int loadExp=65;
unsigned int storeExp=23;
unsigned int prefeExp=2;
unsigned int accessExp=88;
unsigned int accessExpCC=87;

const struct reduction mmxRed = { 2, 1, 0, 3, 48 };
const struct reduction sseRed = { 2, 0, 1, 3, 51 };
const struct reduction sse2Red = { 2, 0, 0, 2, 54 };
const struct reduction amdRed = { 2, 0, 1, 3, 56 };

const struct reduction ccRed = { 0, 0, 0, 1, 83 };

int eaExpOffset[] =    { 0,0,0,0,  0,0,0,0,0,0,0,  4,8,4,8,4,8,4,  0,
                         0,4,8,12,0,4,8,  12,0,8,8,8,0,4,8,4,  0,  4,4,4,0,4,0,4,8,0,0,4,0,
                         0,8,0,  0,0,0,  0,0,  0,8,0,  0,12,0,0,0,44,25,   0,0,0,0,4,8,
                         0,0,0,2,4,8,  0,0,  0,0,  0,4,8 };

unsigned int bcExp[] = { 4,4,4,4,  4,4,4,4,4,4,4,  4,4,4,4,4,4,4,  4,
                         4,4,4,4,4,4,4,   4,4,4,4,4,4,4,4,4,   4,  4,4,1,1,4,4,4,4,4,1,4,4,
                         4,8,8,  16,4,0, 16,8, 8,8,0,  12,4,16,16,49,4,4,  4,8,10,2,4,8,
                         4,8,10,2,4,8, 2,2,  28,28,  4,4,4,  4,4,4 };

void reduce(const struct reduction x)
{
  unsigned int i;

  loadExp  -= x.loadRed;
  storeExp -= x.storeRed;
  prefeExp -= x.prefeRed;

  for(i=x.axsShift; i<accessExp; ++i)
    eaExp[i] = eaExp[i+x.axsRed];

  for(i=x.axsShift; i<accessExp; ++i)
    bcExp[i] = bcExp[i+x.axsRed];

  for(i=x.axsShift; i<accessExpCC; ++i)
    eaExpCC[i] = eaExpCC[i+x.axsRed];

  for(i=x.axsShift; i<accessExpCC; ++i)
    bcExpCC[i] = bcExpCC[i+x.axsRed];

  accessExp -= x.axsRed;
  accessExpCC -= x.axsRed;
}

void reduceCC(const struct reduction x)
{
  unsigned int i;

  for(i=x.axsShift; i<accessExpCC; ++i)
    eaExpCC[i] = eaExpCC[i+x.axsRed];

  for(i=x.axsShift; i<accessExpCC; ++i)
    bcExpCC[i] = bcExpCC[i+x.axsRed];

  accessExpCC -= x.axsRed;
}


void init_test_data()
{
  int caps, i;

  dprintf("&divarw = %p\n", &divarw);
  dprintf("&dfvars = %p\n", &dfvars);
  dprintf("&dfvard = %p\n", &dfvard);
  dprintf("&dfvart = %p\n", &dfvart);
  dprintf("&dlarge = %p\n", &dlarge);

  for(i=4; i<15; ++i)
    eaExp[i] = (void*)((unsigned long)&divarw + eaExpOffset[i]); /* skip ebp for now */
  for(i=16; i<18; ++i)
    eaExp[i] = (void*)((unsigned long)&divarw + eaExpOffset[i]);
  for(i=19; i<26; ++i)
    eaExp[i] = (void*)((unsigned long)&divarw + eaExpOffset[i]);
  i=26;
  eaExp[i] = (void*)((unsigned long)&divarw + eaExpOffset[i]);
  for(i=28; i<35; ++i)
    eaExp[i] = (void*)((unsigned long)&divarw + eaExpOffset[i]);
  for(i=36; i<51; ++i)
    eaExp[i] = (void*)((unsigned long)&divarw + eaExpOffset[i]);
  for(i=51; i<53; ++i)
    eaExp[i] = (void*)((unsigned long)&dfvars + eaExpOffset[i]);
  i=53;
  eaExp[i] = (void*)((unsigned long)&divarw + eaExpOffset[i]);
  for(i=54; i<56; ++i)
    eaExp[i] = (void*)((unsigned long)&dfvard + eaExpOffset[i]);
  for(i=56; i<58; ++i)
    eaExp[i] = (void*)((unsigned long)&dfvars + eaExpOffset[i]);
  i=58;
  eaExp[i] = (void*)((unsigned long)&divarw + eaExpOffset[i]);
  for(i=59; i<62; ++i)
    eaExp[i] = (void*)((unsigned long)&divarw + eaExpOffset[i]);
  i=62; /* 2nd of mov */
  eaExp[i] = (void*)((unsigned long)&dfvars + eaExpOffset[i]);
  for(i=63; i<66; ++i) /* scas, cmps */
    eaExp[i] = (void*)((unsigned long)&dlarge + eaExpOffset[i]);
  i=66;
  eaExp[i] = (void*)((unsigned long)&dfvars + eaExpOffset[i]);
  i=67;
  eaExp[i] = (void*)((unsigned long)&dfvard + eaExpOffset[i]);
  i=68;
  eaExp[i] = (void*)((unsigned long)&dfvart + eaExpOffset[i]);
  for(i=69; i<72; ++i)
    eaExp[i] = (void*)((unsigned long)&divarw + eaExpOffset[i]);
  i=72;
  eaExp[i] = (void*)((unsigned long)&dfvars + eaExpOffset[i]);
  i=73;
  eaExp[i] = (void*)((unsigned long)&dfvard + eaExpOffset[i]);
  i=74;
  eaExp[i] = (void*)((unsigned long)&dfvart + eaExpOffset[i]);
  for(i=75; i<80; ++i)
    eaExp[i] = (void*)((unsigned long)&divarw + eaExpOffset[i]);
  for(i=80; i<82; ++i)
    eaExp[i] = (void*)((unsigned long)&dlarge + eaExpOffset[i]);
  for(i=82; i<85; ++i)
    eaExp[i] = (void*)((unsigned long)&divarw + eaExpOffset[i]);

  /* Duplicate & reduce the stream for cc */

  for(i=0; i<accessExp; ++i) {
    eaExpCC[i] = eaExp[i];
    bcExpCC[i] = bcExp[i];
  }

  reduceCC(ccRed);

  /* Order of reductions matters! It must be right to left. */

  caps = amd_features();
  if(!(caps & CAP_3DNOW))
    reduce(amdRed);
  caps = ia32features();
  if(!(caps & CAP_SSE2))
    reduce(sse2Red);
  if(!(caps & CAP_SSE))
    reduce(sseRed);
  if(!(caps & CAP_MMX))
    reduce(mmxRed);
}
#endif /* defined(i386_unknown_linux2_0) || defined(i386_unknown_nt4_0) */

#ifdef x86_64_unknown_linux2_4
unsigned int loadExp = 73;
unsigned int storeExp = 25;
unsigned int prefeExp = 2;
unsigned int accessExp = 98;
unsigned int accessExpCC = 97;

int eaExpOffset[] =    { 0,0,0,0,0,0,0,                             /* 7 initial stack pushes (EA not checked) */
			 0,0,0,0,0,0,0,0,0,0,0,0,0,                 /* 13 mod=0 loads */
			 4,8,-4,-8,4,8,-4,-8,4,8,-4,-8,127,-128,    /* 14 mod=1 loads */
			 12,0,8,8,8,0,4,8,4,                        /* 9 SIB tests (same as x86) */
			 4,4,4,0,4,0,4,8,0,4,0,0,                   /* 11 semantic tests (one has two accesses) */
			 0,8,0,                                     /* 3 MMX tests */
			 0,0,0,                                     /* 3 SSE tests */
			 0,0,                                       /* 2 SSE2 tests */
			 0,8,0,                                     /* 3 3DNow! tests */
			 0,12,0,0,0,44,25,                          /* 5 REP tests (two have two accesses each) */
			 0,0,0,0,4,8,                               /* x87 */
			 0,0,0,2,4,8,
			 0,0,
			 0,0,
			 0,4,8,                                     /* conditional moves */
			 0,0,0,0,0,0                                /* 6 final stack pops */			 
};

unsigned int bcExp[] = { 8,8,8,8,8,8,8,                  /* 7 initial stack pushes */
			 4,8,4,8,4,8,4,8,4,8,4,8,4,      /* 13 mod=0 loads */
			 4,8,4,8,4,8,4,8,4,8,4,8,4,8,    /* 14 mod=1 loads */
			 4,8,4,8,4,8,4,8,4,              /* 9 SIB tests */
			 4,4,1,1,4,4,4,4,4,4,4,4,        /* 11 semantic tests (one has two accesses) */
			 8,8,8,                          /* 3 MMX tests */                      
			 16,4,0,                         /* 3 SSE tests */
			 16,8,                           /* 2 SSE2 tests */
			 8,8,0,                          /* 3 3DNow! tests */
			 12,16,16,16,49,4,4,             /* 5 REP tests (two have two accesses each) */
			 4,8,10,2,4,8,                   /* x87 */
			 4,8,10,2,4,8,
			 2,2,
                         28,28,
			 4,4,4,                          /* conditional moves */
                         8,8,8,8,8,8                     /* 6 final stack pops */
};

int divarw;
float dfvars;
double dfvard;
long double dfvart;
char dlarge[512] = "keep the interface small and easy to understand.";

void init_test_data()
{
  int i;

  dprintf("&divarw = %p\n", &divarw);
  dprintf("&dfvars = %p\n", &dfvars);
  dprintf("&dfvard = %p\n", &dfvard);
  dprintf("&dfvart = %p\n", &dfvart);
  dprintf("&dlarge = %p\n", &dlarge);

  // we do not check the effective address for stack accesses,
  // since it depends on the stack pointer,
  // so we skip the initial 6 pushes
  i = 7;

  // ModRM and SIB loads and semantic tests (there are 54, but one has two accesses)
  for (; i < 55; i++)
      eaExp[i] = (void *)((unsigned long)&divarw + eaExpOffset[i]);
  
  // the 12th is a load from [RIP + 1]
  eaExp[11] = rip_relative_load_address;

  // the 36th access uses RSP
  eaExp[35] = 0;

  // MMX
  assert(i == 55);
  for (; i < 58; i++)
      eaExp[i] = (void *)((unsigned long)&divarw + eaExpOffset[i]);

  // SSE
  assert(i == 58);
  for (; i < 60; i++)
      eaExp[i] = (void *)((unsigned long)&dfvart + eaExpOffset[i]);
  assert(i == 60);
  eaExp[i] = (void *)((unsigned long)&divarw + eaExpOffset[i]); i++; // the prefetch

  // SSE2
  assert(i == 61);
  for (; i < 63; i++)
      eaExp[i] = (void *)((unsigned long)&dfvart + eaExpOffset[i]);

  // 3DNow!
  assert(i == 63);
  for (; i < 65; i++)
      eaExp[i] = (void *)((unsigned long)&dfvard + eaExpOffset[i]);
  assert(i == 65);
  eaExp[i] = (void *)((unsigned long)&divarw + eaExpOffset[i]); i++;

  // REP prefixes
  assert(i == 66);
  for (; i < 69; i++)
      eaExp[i] = (void *)((unsigned long)&divarw + eaExpOffset[i]);
  assert(i == 69);
  eaExp[i] = (void *)((unsigned long)&dfvars + eaExpOffset[i]); i++;
  for (; i < 73; i++)
      eaExp[i] = (void *)((unsigned long)&dlarge + eaExpOffset[i]);

  // x87
  assert(i == 73);
  eaExp[i] = (void *)((unsigned long)&dfvars + eaExpOffset[i]); i++;
  eaExp[i] = (void *)((unsigned long)&dfvard + eaExpOffset[i]); i++;
  eaExp[i] = (void *)((unsigned long)&dfvart + eaExpOffset[i]); i++;
  eaExp[i] = (void *)((unsigned long)&divarw + eaExpOffset[i]); i++;
  eaExp[i] = (void *)((unsigned long)&divarw + eaExpOffset[i]); i++;
  eaExp[i] = (void *)((unsigned long)&divarw + eaExpOffset[i]); i++;

  eaExp[i] = (void *)((unsigned long)&dfvars + eaExpOffset[i]); i++;
  eaExp[i] = (void *)((unsigned long)&dfvard + eaExpOffset[i]); i++;
  eaExp[i] = (void *)((unsigned long)&dfvart + eaExpOffset[i]); i++;
  eaExp[i] = (void *)((unsigned long)&divarw + eaExpOffset[i]); i++;
  eaExp[i] = (void *)((unsigned long)&divarw + eaExpOffset[i]); i++;
  eaExp[i] = (void *)((unsigned long)&divarw + eaExpOffset[i]); i++;

  eaExp[i] = (void *)((unsigned long)&divarw + eaExpOffset[i]); i++;
  eaExp[i] = (void *)((unsigned long)&divarw + eaExpOffset[i]); i++;

  eaExp[i] = (void *)((unsigned long)&dlarge + eaExpOffset[i]); i++;
   eaExp[i] = (void *)((unsigned long)&dlarge + eaExpOffset[i]); i++;

  // conditional moves
  assert(i == 89);
  for (; i < 92; i++)
      eaExp[i] = (void *)((unsigned long)&divarw + eaExpOffset[i]);

  // duplicate stream for CC (except the second-to-last item)
  for(i = 0; i < 90 ; i++) {
    eaExpCC[i] = eaExp[i];
    bcExpCC[i] = bcExp[i];
  }
  assert(i == 90);
  eaExpCC[i] = eaExp[i+1];
  bcExpCC[i] = bcExp[i+1];
  for(i = 91; i < 97; i++)
      bcExpCC[i] = bcExp[i+1];
}
#endif /* defined(x86_64_unknown_linux2_4) */

#ifdef ia64_unknown_linux2_4
unsigned int bcExp[] = { 8, 8, 8,  8, 8, 8,  8, 16, 16, 0, 0, 0 };
unsigned int bcExpCC[] = { 8, 8, 8,  8, 8, 8,  8, 16, 16, 0, 0, 0 };

void init_test_data()
{
}
#endif /* defined(ia64_unknown_linux2_4) */

#ifdef mips_sgi_irix6_4
long loadsnstores(long x, long y, long z)
{
  return x + y + z;
}

unsigned int bcExp[] = { 0 };

void init_test_data()
{
}
#endif /* defined(mips_sgi_irix6_4) */

#ifdef alpha_dec_osf4_0
long loadsnstores(long x, long y, long z)
{
  return x + y + z;
}

unsigned int bcExp[] = { 0 };

void init_test_data()
{
}
#endif /* defined(alpha_dec_osf4_0) */

#if defined(arch_power) && defined(os_linux)

long loadsnstores(long x, long y, long z)
{
  return x + y + z;
}

unsigned int bcExp[] = { 0 };

void init_test_data()
{
}
#endif /* power linux */

/* functions called by the simple instrumentation points */
void countLoad() {
  ++loadCnt;
}

void countStore()
{
  ++storeCnt;
}

void countPrefetch()
{
  ++prefeCnt;
}

void countAccess()
{
  ++accessCnt;
}

int validateEA(void* ea1[], void* ea2[], unsigned int n)
{
  int ok = 1;
  unsigned int i=0;

  for(; i<n; ++i) {
    ok = (ok && ((ea1[i] == ea2[i]) || ea1[i] == NULL));
    if(!ok) {
      logerror("EA Validation failed at access #%u. Expecting: %p. Got: %p.\n",
	      i+1, ea1[i], ea2[i]);
      return 0;
    }
  }
  return 1;
}

int validateBC(unsigned int bc1[], unsigned int bc2[], unsigned int n)
{
  int ok = 1;
  unsigned int i=0;

  for(; i<n; ++i) {
    ok = (ok && (bc1[i] == bc2[i]));
    if(!ok) {
      logerror("BC Validation failed at access #%d. Expecting: %d. Got: %d.\n",
	      i+1, bc1[i], bc2[i]);
      return 0;
    }
  }
  return 1;
}

/* functions called by the effective address/byte count instrumentation points */
void listEffAddr(void* addr)
{
  if(accessCntEA < accessExp)
    eaList[accessCntEA] = addr;
  else
    doomEA = 1;
  accessCntEA++;
  dprintf("EA[%d]:%p ", accessCntEA, addr);
}

void listByteCnt(unsigned int count)
{
  if(accessCntBC < accessExp)
    bcList[accessCntBC] = count;
  else
    doomBC = 1;
  accessCntBC++;
  dprintf("BC[%d]:%d ", accessCntBC, count);
}


void listEffAddrCC(void* addr)
{
  if(accessCntEAcc < accessExpCC)
    eaListCC[accessCntEAcc] = addr;
  else
    doomEAcc = 1;
  accessCntEAcc++;
  dprintf("?A[%d]:%p ", accessCntEAcc, addr);
}

void listByteCntCC(unsigned int count)
{
  if(accessCntBCcc < accessExpCC)
    bcListCC[accessCntBCcc] = count;
  else
    doomBCcc = 1;
  accessCntBCcc++;
  dprintf("?C[%d]:%d ", accessCntBCcc, count);
}
