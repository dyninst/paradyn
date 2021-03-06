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

/* uimpd.C
   this file contains implementation of "uimpd" tcl command.  This command
   is used internally by the UIM.
*/

/* $Id: uimpd.tcl.C,v 1.53 2004/03/23 01:12:31 eli Exp $ */
 
#include <stdlib.h>
#include "pdutil/h/odometer.h"
#include "pdutil/h/TclTools.h"

#include "UIglobals.h"
#include "../pdMain/paradyn.h"
#include "../DMthread/DMinclude.h"
#include "abstractions.h"
#include "ParadynTkGUI.h"


extern abstractions* theAbstractions;

/* globals for metric resource selection */
pdvector<metric_focus_pair> uim_VisiSelections; // keep this one



void printMFPlist (pdvector<metric_focus_pair> *list) 
{
  for (unsigned i = 0; i < list->size(); i++) {
    cout << "   metric: " <<  
      dataMgr->getMetricName (((*list)[i]).met) << "||| focus: ";
    for (unsigned j = 0; j < ((*list)[i]).res.size(); j++)
      cout << dataMgr->getResourceLabelName (((*list)[i]).res[j]);
    cout << endl;
  }
}

typedef pdvector<unsigned> numlist;
void printResSelectList (pdvector<numlist> *v, char *name)
{
  cout << "[Focus List " << name << "|" << v->size() << "] "; 
  for (unsigned i = 0; i < v->size(); i++) {
    numlist temp = (*v)[i];
    cout << "                    ";
    for (unsigned j = 0; j < temp.size(); j++) 
      cout << temp[j] << " ";
    cout << endl;
  }
  cout << endl;
}

/* parseSelections
 * takes a list with one resourceHandle vector per resource hierarchy; result 
 * is the cross product list of valid foci, represented as vectors of 
 * nodeID vectors; each element on resulting list can be converted into 
 * one focus.
 */
pdvector<numlist> parseSelections(pdvector<numlist> &theHierarchy,
				bool plusWholeProgram,
				numlist wholeProgramFocus) {
   // how many resources are in each hierarchy?:
   pdvector<unsigned> numResourcesByHierarchy(theHierarchy.size());
   for (unsigned hier=0; hier < theHierarchy.size(); hier++)
      numResourcesByHierarchy[hier] = theHierarchy[hier].size();

   odometer theOdometer(numResourcesByHierarchy);
   pdvector<numlist> result;

   while (!theOdometer.done()) {
      // create a focus using the odometer's current setting
      // Make a note if we have added the equivalent of "Whole Program"

      numlist theFocus(theHierarchy.size());

      bool addedEquivOfWholeProgram = plusWholeProgram; // so far...

      for (unsigned hier=0; hier < theHierarchy.size(); hier++) {
         theFocus[hier] = theHierarchy[hier][theOdometer[hier]];
         if (addedEquivOfWholeProgram)
	    addedEquivOfWholeProgram = (theFocus[hier] == wholeProgramFocus[hier]);
      }

      if (!addedEquivOfWholeProgram)
         result += theFocus;
//      else
//         cout << "Suppressing duplicate of whole program" << endl;

      theOdometer++;
   }

   // There is one final thing to check for: whole-program
   if (plusWholeProgram)
      result += wholeProgramFocus;

//   cout << "parseSelections: returning result of " << result.size() << " foci" << endl;
   return result;
}


/* 
   Sends metric-focus pair representation of menu selections to requesting
   visi thread.  Pairs are collected in global list uim_VisiSelections, 
   number of pairs is in uim_VisiSelectionsSize.
   arguments:
       1: msgID
       2: cancelFlag

   note: space allocated for chosenMets and localFocusList must be 
         freed by the visi thread which gets the callback.
*/
int
ParadynTkGUI::SendVisiSelectionsCmd( int /* argc */, TCLCONST char *argv[])
{
  Tcl_HashEntry *entry;
  UIMReplyRec *msgRec;

#if UIM_DEBUG
  cout << "processing " << uim_VisiSelections.size() << " visiselections...\n";
  if (uim_VisiSelections->size() > 0) {
    printMFPlist (uim_VisiSelections);
  }
#endif
  
  // get callback and thread id for this msg
  int msgID = atoi(argv[1]);
  if (!(entry = Tcl_FindHashEntry (&UIMMsgReplyTbl, (char *) msgID))) {
    // this case can occur if a thread has exited between making the
    // menuing request and choosing accept on the menu...ignore it
    return TCL_OK;
  }
  msgRec = (UIMReplyRec *) Tcl_GetHashValue(entry);

     /* set thread id for return */
  setTid(msgRec->tid);
  chooseMandRCBFunc mcb = (chooseMandRCBFunc) msgRec->cb;

  // if cancel was selected invoke callback with null list
  int cancelFlag = atoi(argv[2]);
  if (cancelFlag == 1) {
    chosenMetricsandResources(mcb, 0);
  }    
  else {
#ifdef n_def      
      printf("uim_VisiSelections.size() = %d\n",uim_VisiSelections->size());
      for(unsigned l = 0; l < uim_VisiSelections->size(); l++){
          printf("metric %d: %d\n",l,(*uim_VisiSelections)[l].met);
          for(unsigned blah =0; blah < (*uim_VisiSelections)[l].res.size(); 
	      blah++){
	      printf("resource %d:%d:  %d\n",
			l,blah,(*uim_VisiSelections)[l].res[blah]);
          }
      }
#endif

      // Since the following igen call is async, we must unfortunately make
      // a copy of uim_VisiSelections, and pass that in.  The consumer
      // will deallocate the memory.
      pdvector<metric_focus_pair> *temp_igen_vec = new pdvector<metric_focus_pair> (uim_VisiSelections);
      assert(temp_igen_vec);
      chosenMetricsandResources(mcb, temp_igen_vec);
  }

  // cleanup data structures
  Tcl_DeleteHashEntry (entry);   // cleanup hash table record

  return TCL_OK;
}

/* arguments:
       0: "processVisiSelection"
       1: list of selected metrics
*/
int 
ParadynTkGUI::ProcessVisiSelectionsCmd( int /* argc */,
                                        TCLCONST char *argv[])
{
   bool wholeProgram;
   numlist wholeProgramFocus;
   pdvector< pdvector<resourceHandle> > theHierarchySelections = theAbstractions->getCurrAbstractionSelections(wholeProgram, wholeProgramFocus);

#if UIM_DEBUG
   for (int i=0; i < theHierarchySelections.size(); i++) {
      cout << "ResHierarchy " << i << " selections: ";
      for (int j=0; j < theHierarchySelections[i].size(); j++)
         cout << " " << theHierarchySelections[i][j];
      cout << endl;
   }
#endif

  pdvector<numlist> fociList = parseSelections(theHierarchySelections,
					                            wholeProgram,
                                                wholeProgramFocus);

#if UIM_DEBUG
  printResSelectList(fociList, "list of selected focii");
#endif

//** and, list of metric indices from selections put into metlst

  assert(all_metrics_set_yet);

  int metcnt;
  TCLCONST char **metlst;
  // reminder: argv[1] is the list of selected metrics (each is an integer id)
  bool aflag;
  aflag = (TCL_OK == Tcl_SplitList (interp, argv[1], &metcnt, &metlst));
  assert(aflag);

//   cout << "Here are the selections (in metric-ids)" << endl;
//   for (unsigned i=0; i < metcnt; i++)
//      cout << metlst[i] << " ";
//   cout << endl;

   uim_VisiSelections.resize(0);
   
   for (int i = 0; i < metcnt; i++) {
      unsigned metric_id = atoi(metlst[i]);
      assert(all_metric_names.defines(metric_id)); // just a sanity check

      for (unsigned focuslcv = 0; focuslcv < fociList.size(); focuslcv++)
	 uim_VisiSelections += metric_focus_pair(metric_id, fociList[focuslcv]);
   }

   Tcl_Free ((char*)metlst);   // cleanup after Tcl_SplitList

   std::ostringstream resstr;

   resstr << 1 << std::ends;
   SetInterpResult(interp, resstr);
   return TCL_OK;
}


/*
 * argv[1] = error number
 * argv[2] = error string
 */
int
ParadynTkGUI::ShowErrorCmd( int /* argc */, TCLCONST char *argv[])
{
  int code = atoi(argv[1]);
  showError(code, argv[2]);
  return TCL_OK;
}


int
ParadynTkGUI::StartPhaseCmd(int argc, TCLCONST char **argv)
{
   assert(argc == 2);
   if (0==strcmp(argv[1], "plain")) {
      dataMgr->StartPhase(NULL, NULL, false, false);
      return TCL_OK;
   }
   else if (0==strcmp(argv[1], "pc")) {
      dataMgr->StartPhase(NULL, NULL, true, false);
      return TCL_OK;
   }
   else if (0==strcmp(argv[1], "visis")) {
      dataMgr->StartPhase(NULL, NULL, false, true);
      return TCL_OK;
   }
   else if (0==strcmp(argv[1], "both")) {
      dataMgr->StartPhase(NULL, NULL, true, true);
      return TCL_OK;
   }
   else {
      cerr << "uimpd: unknown cmd " << argv[1] << endl;
      return TCL_ERROR;
   }
}

