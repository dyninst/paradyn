/*
 * Copyright (c) 1996-1999 Barton P. Miller
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

// tvFocus.h
// Ariel Tamches

/*
 * $Log: tvFocus.h,v $
 * Revision 1.6  1999/03/13 15:24:07  pcroth
 * Added support for building under Windows NT
 *
 * Revision 1.5  1996/08/16 21:37:06  tamches
 * updated copyright for release 1.1
 *
 * Revision 1.4  1996/08/05 07:12:12  tamches
 * tkclean.h --> tk.h
 *
 * Revision 1.3  1995/12/22 22:39:28  tamches
 * added visiLibId
 *
 * Revision 1.2  1995/11/08 21:48:12  tamches
 * moved implementation of constructor to .C file
 *
 * Revision 1.1  1995/11/04 00:46:39  tamches
 * First version of new table visi
 *
 */

#ifndef _TV_FOCUS_H_
#define _TV_FOCUS_H_

#include "util/h/String.h"
#include "tk.h"

class tvFocus {
 private:
   unsigned visiLibId; // what unique-id has the visi-lib assigned to us?
   string longName, shortName;
   unsigned longNamePixWidth, shortNamePixWidth;

 public:
   tvFocus(){} // needed by class Vector (nuts)
   tvFocus(unsigned iVisiLibId, const string &iLongName, Tk_Font nameFont);

   tvFocus(const tvFocus &src) : longName(src.longName), shortName(src.shortName) {
      visiLibId = src.visiLibId;
      longNamePixWidth = src.longNamePixWidth;
      shortNamePixWidth = src.shortNamePixWidth;
   }
  ~tvFocus() {}

   unsigned getVisiLibId() const {return visiLibId;}

   bool less_than(const tvFocus &other, bool useLongName) {
      if (useLongName)
         return (longName < other.longName);
      else
         return (shortName < other.shortName);
   }

   bool greater_than(const tvFocus &other, bool useLongName) {
      if (useLongName)
         return (longName > other.longName);
      else
         return (shortName > other.shortName);
   }

   const string &getLongName() const {return longName;}
   const string &getShortName() const {return shortName;}
   unsigned getLongNamePixWidth() const {return longNamePixWidth;}
   unsigned getShortNamePixWidth() const {return shortNamePixWidth;}
};

#endif
