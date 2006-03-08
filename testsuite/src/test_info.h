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

// $Id: test_info.h,v 1.11 2006/03/08 16:44:06 bpellin Exp $
#ifndef TEST_INFO_H
#define TEST_INFO_H

const unsigned int num_tests = 109;
//const unsigned int num_tests = 80;
//const unsigned int num_tests = 20;
mutatee_list_t test1_mutatee;
mutatee_list_t test2_mutatee;
mutatee_list_t test3_mutatee;
mutatee_list_t test4_mutatee;
mutatee_list_t test5_mutatee;
mutatee_list_t test6_mutatee;
mutatee_list_t test7_mutatee;
mutatee_list_t test8_mutatee;
mutatee_list_t test9_mutatee;
mutatee_list_t test10_mutatee;
mutatee_list_t test12_mutatee;
mutatee_list_t test13_mutatee;
mutatee_list_t test14_mutatee;
mutatee_list_t none;

/* A little bit of macro magic to get the text of the extensions out */
#define xstr(x) str(x)
#define str(x) #x
void initialize_mutatees()
{
   dprintf("Initializing mutatees\n");
#ifdef gnu_cc
   dprintf("Initializing gnu_cc mutatees\n");
   test1_mutatee.push_back("test1.mutatee" xstr(gnu_cc));
   test2_mutatee.push_back("test2.mutatee" xstr(gnu_cc));
   test3_mutatee.push_back("test3.mutatee" xstr(gnu_cc));
   test4_mutatee.push_back("test4a.mutatee" xstr(gnu_cc));
   test6_mutatee.push_back("test6.mutatee" xstr(gnu_cc));
   test7_mutatee.push_back("test7.mutatee" xstr(gnu_cc));
   test8_mutatee.push_back("test8.mutatee" xstr(gnu_cc));
   test9_mutatee.push_back("test9.mutatee" xstr(gnu_cc));
   test10_mutatee.push_back("test10.mutatee" xstr(gnu_cc));
   test12_mutatee.push_back("test12.mutatee" xstr(gnu_cc));
   test13_mutatee.push_back("test13.mutatee" xstr(gnu_cc));
   test14_mutatee.push_back("test14.mutatee" xstr(gnu_cc));
#endif

#ifdef gnu_cxx
   test1_mutatee.push_back("test1.mutatee" xstr(gnu_cxx));
   test2_mutatee.push_back("test2.mutatee" xstr(gnu_cxx));
   test3_mutatee.push_back("test3.mutatee" xstr(gnu_cxx));
   test4_mutatee.push_back("test4a.mutatee" xstr(gnu_cxx));
   test5_mutatee.push_back("test5.mutatee" xstr(gnu_cxx));
   test7_mutatee.push_back("test7.mutatee" xstr(gnu_cxx));
   test8_mutatee.push_back("test8.mutatee" xstr(gnu_cxx));
   test9_mutatee.push_back("test9.mutatee" xstr(gnu_cxx));
   test12_mutatee.push_back("test12.mutatee" xstr(gnu_cxx));
   test13_mutatee.push_back("test13.mutatee" xstr(gnu_cxx));
   test14_mutatee.push_back("test14.mutatee" xstr(gnu_cxx));
#endif
   
#ifdef gnu_fc
   test1_mutatee.push_back("test1.mutatee" xstr(gnu_fc));
#endif

#ifdef native_fc
   test1_mutatee.push_back("test1.mutatee" xstr(native_fc));
#endif
   
#ifdef native_cc
   test1_mutatee.push_back("test1.mutatee" xstr(native_cc));
   test2_mutatee.push_back("test2.mutatee" xstr(native_cc));
   test3_mutatee.push_back("test3.mutatee" xstr(native_cc));
   test4_mutatee.push_back("test4a.mutatee" xstr(native_cc));
   test6_mutatee.push_back("test6.mutatee" xstr(native_cc));
   test7_mutatee.push_back("test7.mutatee" xstr(native_cc));
   test8_mutatee.push_back("test8.mutatee" xstr(native_cc));
   test9_mutatee.push_back("test9.mutatee" xstr(native_cc));
   test12_mutatee.push_back("test12.mutatee" xstr(native_cc));
   test13_mutatee.push_back("test13.mutatee" xstr(native_cc));
   test14_mutatee.push_back("test14.mutatee" xstr(native_cc));
#endif

#ifdef native_cxx
   test1_mutatee.push_back("test1.mutatee" xstr(native_cxx));
   test2_mutatee.push_back("test2.mutatee" xstr(native_cxx));
   test3_mutatee.push_back("test3.mutatee" xstr(native_cxx));
   test4_mutatee.push_back("test4a.mutatee" xstr(native_cxx));
   test5_mutatee.push_back("test5.mutatee" xstr(native_cxx));
   test7_mutatee.push_back("test7.mutatee" xstr(native_cxx));
   test8_mutatee.push_back("test8.mutatee" xstr(native_cxx));
   test9_mutatee.push_back("test9.mutatee" xstr(native_cxx));
   test12_mutatee.push_back("test12.mutatee" xstr(native_cxx));
   test13_mutatee.push_back("test13.mutatee" xstr(native_cxx));
   test14_mutatee.push_back("test14.mutatee" xstr(native_cxx));
#endif

#ifdef gnu_abi_cc
   test1_mutatee.push_back("test1.mutatee" xstr(gnu_abi_cc));
   test2_mutatee.push_back("test2.mutatee" xstr(gnu_abi_cc));
   test3_mutatee.push_back("test3.mutatee" xstr(gnu_abi_cc));
   test4_mutatee.push_back("test4a.mutatee" xstr(gnu_abi_cc));
   test6_mutatee.push_back("test6.mutatee" xstr(gnu_abi_cc));
   test7_mutatee.push_back("test7.mutatee" xstr(gnu_abi_cc));
   test8_mutatee.push_back("test8.mutatee" xstr(gnu_abi_cc));
   test9_mutatee.push_back("test9.mutatee" xstr(gnu_abi_cc));
   test10_mutatee.push_back("test10.mutatee" xstr(gnu_abi_cc));
   test12_mutatee.push_back("test12.mutatee" xstr(gnu_abi_cc));
#endif

#ifdef gnu_abi_cxx
   test1_mutatee.push_back("test1.mutatee" xstr(gnu_abi_cxx));
   test2_mutatee.push_back("test2.mutatee" xstr(gnu_abi_cxx));
   test3_mutatee.push_back("test3.mutatee" xstr(gnu_abi_cxx));
   test4_mutatee.push_back("test4a.mutatee" xstr(gnu_abi_cxx));
   test7_mutatee.push_back("test7.mutatee" xstr(gnu_abi_cxx));
   test8_mutatee.push_back("test8.mutatee" xstr(gnu_abi_cxx));
   test9_mutatee.push_back("test9.mutatee" xstr(gnu_abi_cxx));
   test12_mutatee.push_back("test12.mutatee" xstr(gnu_abi_cxx));
#endif

   none.push_back("");
}

platforms_t all_platforms =
   { true, true, true, true, true, true, true, true};

platforms_t test5_12_platforms =
   { /*alpha_dec_osf5_1        =*/ false,
     /*i386_unknown_linux2_4   =*/ true, 
     /*i386_unknown_nt4_0      =*/ true, 
     /*ia64_unknown_linux2_4   =*/ true,
     /*x86_64_unknown_linux2_4 =*/ true,
     /*mips_sgi_irix6_5        =*/ false,
     /*rs6000_ibm_aix5_1       =*/ true,
     /*sparc_sun_solaris2_8    =*/ true,
   };

platforms_t test7_8_platforms =
   { /*alpha_dec_osf5_1        =*/ false,
     /*i386_unknown_linux2_4   =*/ true, 
     /*i386_unknown_nt4_0      =*/ true, 
     /*ia64_unknown_linux2_4   =*/ true,
     /*x86_64_unknown_linux2_4 =*/ true,
     /*mips_sgi_irix6_5        =*/ true,
     /*rs6000_ibm_aix5_1       =*/ true,
     /*sparc_sun_solaris2_8    =*/ true,
   };

platforms_t test9_platforms =
   { /*alpha_dec_osf5_1        =*/ false,
     /*i386_unknown_linux2_4   =*/ true, 
     /*i386_unknown_nt4_0      =*/ true, 
     /*ia64_unknown_linux2_4   =*/ false,
     /*x86_64_unknown_linux2_4 =*/ true,
     /*mips_sgi_irix6_5        =*/ false,
     /*rs6000_ibm_aix5_1       =*/ true,
     /*sparc_sun_solaris2_8    =*/ true,
   };

platforms_t test10_11_platforms =
   { /*alpha_dec_osf5_1        =*/ false,
     /*i386_unknown_linux2_4   =*/ false, 
     /*i386_unknown_nt4_0      =*/ false, 
     /*ia64_unknown_linux2_4   =*/ false,
     /*x86_64_unknown_linux2_4 =*/ false,
     /*mips_sgi_irix6_5        =*/ false,
     /*rs6000_ibm_aix5_1       =*/ false,
     /*sparc_sun_solaris2_8    =*/ true,
   };

test_data_t tests[] = {
   test_data_t("test1_1", "./test1_1.so", test1_mutatee, all_platforms, STOPPED, 1, 1, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_2", "./test1_2.so", test1_mutatee, all_platforms, STOPPED, 1, 2, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_3", "./test1_3.so", test1_mutatee, all_platforms, STOPPED, 1, 3, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_4", "./test1_4.so", test1_mutatee, all_platforms, STOPPED, 1, 4, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_5", "./test1_5.so", test1_mutatee, all_platforms, STOPPED, 1, 5, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_6", "./test1_6.so", test1_mutatee, all_platforms, STOPPED, 1, 6, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_7", "./test1_7.so", test1_mutatee, all_platforms, STOPPED, 1, 7, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_8", "./test1_8.so", test1_mutatee, all_platforms, STOPPED, 1, 8, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_9", "./test1_9.so", test1_mutatee, all_platforms, STOPPED, 1, 9, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_10", "./test1_10.so", test1_mutatee, all_platforms, STOPPED, 1, 10, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_11", "./test1_11.so", test1_mutatee, all_platforms, STOPPED, 1, 11, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_12", "./test1_12.so", test1_mutatee, all_platforms, STOPPED, 1, 12, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_13", "./test1_13.so", test1_mutatee, all_platforms, STOPPED, 1, 13, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_14", "./test1_14.so", test1_mutatee, all_platforms, STOPPED, 1, 14, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_15", "./test1_15.so", test1_mutatee, all_platforms, STOPPED, 1, 15, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_16", "./test1_16.so", test1_mutatee, all_platforms, STOPPED, 1, 16, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_17", "./test1_17.so", test1_mutatee, all_platforms, STOPPED, 1, 17, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_18", "./test1_18.so", test1_mutatee, all_platforms, STOPPED, 1, 18, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_19", "./test1_19.so", test1_mutatee, all_platforms, RUNNING, 1, 19, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_20", "./test1_20.so", test1_mutatee, all_platforms, STOPPED, 1, 20, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_21", "./test1_21.so", test1_mutatee, all_platforms, STOPPED, 1, 21, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_22", "./test1_22.so", test1_mutatee, all_platforms, STOPPED, 1, 22, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_23", "./test1_23.so", test1_mutatee, all_platforms, STOPPED, 1, 23, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_24", "./test1_24.so", test1_mutatee, all_platforms, STOPPED, 1, 24, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_25", "./test1_25.so", test1_mutatee, all_platforms, STOPPED, 1, 25, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_26", "./test1_26.so", test1_mutatee, all_platforms, STOPPED, 1, 26, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_27", "./test1_27.so", test1_mutatee, all_platforms, STOPPED, 1, 27, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_28", "./test1_28.so", test1_mutatee, all_platforms, STOPPED, 1, 28, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_29", "./test1_29.so", test1_mutatee, all_platforms, STOPPED, 1, 29, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_30", "./test1_30.so", test1_mutatee, all_platforms, STOPPED, 1, 30, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_31", "./test1_31.so", test1_mutatee, all_platforms, STOPPED, 1, 31, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_32", "./test1_32.so", test1_mutatee, all_platforms, STOPPED, 1, 32, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_33", "./test1_33.so", test1_mutatee, all_platforms, STOPPED, 1, 33, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_34", "./test1_34.so", test1_mutatee, all_platforms, STOPPED, 1, 34, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_35", "./test1_35.so", test1_mutatee, all_platforms, STOPPED, 1, 35, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_36", "./test1_36.so", test1_mutatee, all_platforms, STOPPED, 1, 36, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_37", "./test1_37.so", test1_mutatee, all_platforms, STOPPED, 1, 37, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_38", "./test1_38.so", test1_mutatee, all_platforms, STOPPED, 1, 38, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_39", "./test1_39.so", test1_mutatee, all_platforms, STOPPED, 1, 39, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test1_40", "./test1_40.so", test1_mutatee, all_platforms, STOPPED, 1, 40, COLLECT_EXITCODE, BOTH, ENABLED),
   test_data_t("test2_1", "./test2_1.so", none, all_platforms, SELFSTART, 2, 1, NONE, BOTH, ENABLED),
   test_data_t("test2_2", "./test2_2.so", none, all_platforms, SELFSTART, 2, 2, NONE, BOTH, ENABLED),
   test_data_t("test2_3", "./test2_3.so", none, all_platforms, SELFSTART, 2, 3, NONE, USEATTACH, ENABLED),
   test_data_t("test2_4", "./test2_4.so", none, all_platforms, SELFSTART, 2, 4, NONE, USEATTACH, ENABLED),
   test_data_t("test2_5", "./test2_5.so", test2_mutatee, all_platforms, STOPPED, 2, 5, KILL_MUTATEE, BOTH, ENABLED),
   test_data_t("test2_6", "./test2_6.so", test2_mutatee, all_platforms, STOPPED, 2, 6, KILL_MUTATEE, BOTH, ENABLED),
   test_data_t("test2_7", "./test2_7.so", test2_mutatee, all_platforms, STOPPED, 2, 7, KILL_MUTATEE, BOTH, ENABLED),
   test_data_t("test2_8", "./test2_8.so", test2_mutatee, all_platforms, STOPPED, 2, 8, KILL_MUTATEE, BOTH, ENABLED),
   test_data_t("test2_9", "./test2_9.so", test2_mutatee, all_platforms, STOPPED, 2, 9, KILL_MUTATEE, BOTH, ENABLED),
   test_data_t("test2_10", "./test2_10.so", test2_mutatee, all_platforms, STOPPED, 2, 10, KILL_MUTATEE, BOTH, ENABLED),
   test_data_t("test2_11", "./test2_11.so", test2_mutatee, all_platforms, STOPPED, 2, 11, KILL_MUTATEE, BOTH, ENABLED),
   test_data_t("test2_12", "./test2_12.so", test2_mutatee, all_platforms, STOPPED, 2, 12, KILL_MUTATEE, BOTH, ENABLED),
   test_data_t("test2_13", "./test2_13.so", test2_mutatee, all_platforms, STOPPED, 2, 13, KILL_MUTATEE, BOTH, ENABLED),
   test_data_t("test2_14", "./test2_14.so", test2_mutatee, all_platforms, STOPPED, 2, 14, NONE, BOTH, ENABLED),
   test_data_t("test3_1", "./test3_1.so", test3_mutatee, all_platforms, SELFSTART, 3, 1, NONE, CREATE, ENABLED),
   test_data_t("test3_2", "./test3_2.so", test3_mutatee, all_platforms, SELFSTART, 3, 2, NONE, CREATE, ENABLED),
   test_data_t("test3_3", "./test3_3.so", test3_mutatee, all_platforms, SELFSTART, 3, 3, NONE, CREATE, ENABLED),
   test_data_t("test3_4", "./test3_4.so", test3_mutatee, all_platforms, SELFSTART, 3, 4, NONE, CREATE, ENABLED),
   test_data_t("test3_5", "./test3_5.so", test3_mutatee, all_platforms, SELFSTART, 3, 5, NONE, CREATE, ENABLED),
   test_data_t("test4_1", "./test4_1.so", test4_mutatee, all_platforms, SELFSTART, 4, 1, NONE, CREATE, ENABLED),
   test_data_t("test4_2", "./test4_2.so", test4_mutatee, all_platforms, SELFSTART, 4, 2, NONE, CREATE, ENABLED),
   test_data_t("test4_3", "./test4_3.so", test4_mutatee, all_platforms, SELFSTART, 4, 3, NONE, CREATE, ENABLED),
   test_data_t("test4_4", "./test4_4.so", test4_mutatee, all_platforms, SELFSTART, 4, 4, NONE, CREATE, ENABLED),
   test_data_t("test5_1", "./test5_1.so", test5_mutatee, test5_12_platforms, STOPPED, 5, 1, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test5_2", "./test5_2.so", test5_mutatee, test5_12_platforms, STOPPED, 5, 2, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test5_3", "./test5_3.so", test5_mutatee, test5_12_platforms, STOPPED, 5, 3, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test5_4", "./test5_4.so", test5_mutatee, test5_12_platforms, STOPPED, 5, 4, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test5_5", "./test5_5.so", test5_mutatee, test5_12_platforms, STOPPED, 5, 5, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test5_6", "./test5_6.so", test5_mutatee, test5_12_platforms, STOPPED, 5, 6, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test5_7", "./test5_7.so", test5_mutatee, test5_12_platforms, STOPPED, 5, 7, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test5_8", "./test5_8.so", test5_mutatee, test5_12_platforms, STOPPED, 5, 8, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test5_9", "./test5_9.so", test5_mutatee, test5_12_platforms, STOPPED, 5, 9, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test6_1", "./test6_1.so", test6_mutatee, all_platforms, STOPPED, 6, 1, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test6_2", "./test6_2.so", test6_mutatee, all_platforms, STOPPED, 6, 2, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test6_3", "./test6_3.so", test6_mutatee, all_platforms, STOPPED, 6, 3, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test6_4", "./test6_4.so", test6_mutatee, all_platforms, STOPPED, 6, 4, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test6_5", "./test6_5.so", test6_mutatee, all_platforms, STOPPED, 6, 5, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test6_6", "./test6_6.so", test6_mutatee, all_platforms, STOPPED, 6, 6, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test6_7", "./test6_7.so", test6_mutatee, all_platforms, STOPPED, 6, 7, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test6_8", "./test6_8.so", test6_mutatee, all_platforms, STOPPED, 6, 8, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test7_1", "./test7_1.so", test7_mutatee, test7_8_platforms, STOPPED, 7, 1, NONE, CREATE, ENABLED),
   test_data_t("test7_2", "./test7_2.so", test7_mutatee, test7_8_platforms, STOPPED, 7, 2, NONE, CREATE, ENABLED),
   test_data_t("test7_3", "./test7_3.so", test7_mutatee, test7_8_platforms, STOPPED, 7, 3, NONE, CREATE, ENABLED),
   test_data_t("test7_4", "./test7_4.so", test7_mutatee, test7_8_platforms, STOPPED, 7, 4, NONE, CREATE, ENABLED),
   test_data_t("test7_5", "./test7_5.so", test7_mutatee, test7_8_platforms, STOPPED, 7, 5, NONE, CREATE, ENABLED),
   test_data_t("test7_6", "./test7_6.so", test7_mutatee, test7_8_platforms, STOPPED, 7, 6, NONE, CREATE, ENABLED),
   test_data_t("test7_7", "./test7_7.so", test7_mutatee, test7_8_platforms, STOPPED, 7, 7, NONE, CREATE, ENABLED),
   test_data_t("test7_8", "./test7_8.so", test7_mutatee, test7_8_platforms, STOPPED, 7, 8, NONE, CREATE, ENABLED),
   test_data_t("test7_9", "./test7_9.so", test7_mutatee, test7_8_platforms, STOPPED, 7, 8, NONE, CREATE, ENABLED),
   test_data_t("test8_1", "./test8_1.so", test8_mutatee, test7_8_platforms, STOPPED, 8, 1, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test8_2", "./test8_2.so", test8_mutatee, test7_8_platforms, STOPPED, 8, 2, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test8_3", "./test8_3.so", test8_mutatee, test7_8_platforms, STOPPED, 8, 3, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test9_1", "./test9_1.so", test9_mutatee, test9_platforms, SELFSTART, 9, 1, NONE, CREATE, ENABLED),
   test_data_t("test9_2", "./test9_2.so", test9_mutatee, test9_platforms, SELFSTART, 9, 2, NONE, CREATE, ENABLED),
   test_data_t("test9_3", "./test9_3.so", test9_mutatee, test9_platforms, SELFSTART, 9, 3, NONE, CREATE, ENABLED),
   test_data_t("test9_4", "./test9_4.so", test9_mutatee, test9_platforms, SELFSTART, 9, 4, NONE, CREATE, ENABLED),
   test_data_t("test9_5", "./test9_5.so", test9_mutatee, test9_platforms, SELFSTART, 9, 5, NONE, CREATE, ENABLED),
   test_data_t("test9_6", "./test9_6.so", test9_mutatee, test9_platforms, SELFSTART, 9, 6, NONE, CREATE, ENABLED),
   test_data_t("test9_7", "./test9_7.so", test9_mutatee, test9_platforms, SELFSTART, 9, 7, NONE, CREATE, ENABLED),
   test_data_t("test10_1", "./test10_1.so", test10_mutatee, test10_11_platforms, STOPPED, 10, 1, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test10_2", "./test10_2.so", test10_mutatee, test10_11_platforms, STOPPED, 10, 2, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test10_3", "./test10_3.so", test10_mutatee, test10_11_platforms, STOPPED, 10, 3, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test10_4", "./test10_4.so", test10_mutatee, test10_11_platforms, STOPPED, 10, 4, COLLECT_EXITCODE, CREATE, ENABLED),
   test_data_t("test12_1", "./test12_1.so", test12_mutatee, test5_12_platforms, STOPPED, 12, 1, KILL_MUTATEE, CREATE, DISABLED),
   test_data_t("test12_2", "./test12_2.so", test12_mutatee, test5_12_platforms, STOPPED, 12, 2, KILL_MUTATEE, CREATE, DISABLED),
   test_data_t("test12_3", "./test12_3.so", test12_mutatee, test5_12_platforms, STOPPED, 12, 3, KILL_MUTATEE, CREATE, DISABLED),
   test_data_t("test12_4", "./test12_4.so", test12_mutatee, test5_12_platforms, STOPPED, 12, 4, KILL_MUTATEE, CREATE, DISABLED),
   test_data_t("test13_1", "./test13_1.so", test13_mutatee, all_platforms, SELFSTART, 13, 1, NONE, CREATE, ENABLED),
   test_data_t("test14_1", "./test14_1.so", test14_mutatee, all_platforms, SELFSTART, 14, 1, NONE, CREATE, ENABLED)
};
#endif /* TEST_INFO_H */
