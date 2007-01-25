/***********************************************************************
 * Copyright � 2003-2004 Dorian C. Arnold, Philip C. Roth, Barton P. Miller *
 *                  Detailed MRNet usage rights in "LICENSE" file.     *
 **********************************************************************/

#if !defined(test_array_h)
#define test_array_h 1

typedef enum {PROT_EXIT=FIRST_APPL_TAG,
              PROT_CHAR, PROT_UCHAR,
              PROT_SHORT, PROT_USHORT,
              PROT_INT, PROT_UINT,
              PROT_LONG, PROT_ULONG,
              PROT_FLOAT, PROT_DOUBLE,
              PROT_STRING, PROT_ALL} Protocol;

#endif /* test_array_h */
