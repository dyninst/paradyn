/*
 * Put all the templates in one file
 *
 */

/*
 * $Log: templates.C,v $
 * Revision 1.25  1995/12/11 02:24:03  newhall
 * added vector of rlNameId
 *
 * Revision 1.24  1995/12/09 04:06:51  tamches
 * added some missing templates
 *
 * Revision 1.23  1995/12/03 21:32:29  newhall
 * added BufferPool
 *
 * Revision 1.22  1995/11/29 00:15:35  tamches
 * added stuff necessary for the new pdLogo.C file
 *
 * Revision 1.21  1995/11/20 03:19:08  tamches
 * changed vector<whereAxisRootNode *> to const
 *
 * Revision 1.20  1995/11/06 19:31:06  tamches
 * dictionary --> dictionary_lite changes in many classes used in the UI; results
 * in fewer necessary instantiations.
 * Got rid of some obsolete UI classes like pRec, resourceList, resource**
 *
 * Revision 1.19  1995/10/17 20:42:07  tamches
 * Commented out references to the now-obsolete classes dag & shgDisplay.
 * Added stuff for where axis changes (e.g. where4tree<whereAxisRootNode>
 * instead of where4tree<resourceHandle).
 * Added new search history graph.
 *
 * Revision 1.18  1995/10/05 04:43:52  karavan
 * added template classes for changes to UI::PC interface.
 * deleted obsolete (tokenRec class) and commented code.
 *
 * Revision 1.17  1995/09/20  01:13:21  tamches
 * Where axis changes -- added graphicalPath
 *
 * Revision 1.16  1995/08/24  15:02:11  hollings
 * AIX/SP-2 port (including option for split instruction/data heaps)
 * Tracing of rexec (correctly spawns a paradynd if needed)
 * Added rtinst function to read getrusage stats (can now be used in metrics)
 * Critical Path
 * Improved Error reporting in MDL sematic checks
 * Fixed MDL Function call statement
 * Fixed bugs in TK usage (strings passed where UID expected)
 *
 * Revision 1.15  1995/08/01  02:18:04  newhall
 * changes to support phases
 *
 * Revision 1.14  1995/07/24 21:25:51  tamches
 * removed list of resourceDisplayObj
 *
 * Revision 1.13  1995/07/17  05:04:23  tamches
 * Changes for new where axis code.
 *
 * Revision 1.12  1995/06/02  20:55:51  newhall
 * made code compatable with new DM interface
 *
 * Revision 1.11  1995/05/18  10:47:41  markc
 * Removed duplicate template definitions
 *
 * Revision 1.10  1995/03/24  04:49:46  krisna
 * added instantiations for lists within hash tables
 *
 * Revision 1.9  1995/03/15  20:12:22  jcargill
 * Added instantiation of templates used by TCthread; this caused link
 * problems only on HPUX for some strange reason.
 *
 * Revision 1.8  1995/02/27  18:40:05  tamches
 * Minor changes to reflect new TCthread (tunable constant header
 * files have moved)
 *
 * Revision 1.7  1995/02/26  02:27:51  newhall
 * added source file DMphase.C
 *
 * Revision 1.6  1995/02/16  08:05:27  markc
 * Added missing template instantiation requests.
 * Changed template instantiation requests to the correct form.
 *
 * Revision 1.5  1995/01/26  17:57:00  jcargill
 * Changed igen-generated include files to new naming convention; fixed
 * some bugs compiling with gcc-2.6.3.
 *
 * Revision 1.4  1994/11/01  05:45:17  karavan
 * UIthread changes to implement multiple focus selection on a single display
 *
 * Revision 1.3  1994/10/25  17:56:11  karavan
 * added resource Display Objects for multiple abstractions to UIthread code
 *
 * Revision 1.2  1994/10/09  01:29:13  karavan
 * added UIM templates connected with change to new UIM/visiThread metric-res
 * selection interface.
 *
 * Revision 1.1  1994/09/22  00:49:12  markc
 * Paradyn now uses one template code generating file.  All threads should use
 * this file to generate template code.
 *
 * Revision 1.2  1994/08/22  15:59:31  markc
 * Add List<daemonEntry*> which is the daemon definition dictionary.
 *
 * Revision 1.1  1994/05/19  00:02:18  hollings
 * added templates.
 *
 *
 */
#pragma implementation "list.h"
#include "util/h/list.h"

#include "util/h/String.h"

// Igen includes
#pragma implementation "Vector.h"
#include "util/h/Vector.h"
#pragma implementation "Queue.h"
#include "util/h/Queue.h"

#pragma implementation "BufferPool.h"
#include "paradyn/src/DMthread/BufferPool.h"

#pragma implementation "Dictionary.h"
#include "util/h/Dictionary.h"

#pragma implementation "DictionaryLite.h"
#include "util/src/DictionaryLite.C"

#pragma implementation "dyninstRPC.xdr.h"
#include "dyninstRPC.xdr.h"

#pragma implementation "visi.xdr.h"
#include "visi.xdr.h"


/* *********************************   
 * DMthread stuff
 */

#include "dataManager.thread.h"
#include "dyninstRPC.xdr.CLNT.h"
class cpContext;
#include "paradyn/src/DMthread/DMdaemon.h"
#include "paradyn/src/DMthread/DMmetric.h"
#include "paradyn/src/DMthread/DMresource.h"
#include "paradyn/src/DMthread/DMperfstream.h"
#include "paradyn/src/DMthread/DMinclude.h"
#include "paradyn/src/DMthread/DMabstractions.h"
#include "paradyn/src/DMthread/DVbufferpool.h"


template class vector<cpContext*>;
template class vector<unsigned>;
template class vector< vector<unsigned> >;
template class vector<int>;
template class vector< vector<string> >;
template class vector<phaseInfo *>;
template class vector<daemonEntry*>;
template class vector<paradynDaemon*>;
template class vector<executable*>;
template class vector<component*>;
template class vector<sampleInfo*>;
template class vector<bool>;
template class vector<metric_focus_pair>;
template class vector<met_name_id>;
template class vector<metric*>;
template class vector<resource*>;
template class vector<resourceList*>;
template class vector<abstraction*>;
template class vector<metricInstance*>;
template class vector<ArchiveType *>;
template class vector<rlNameIdType>;
template class dictionary<string,metric*>;
template class dictionary_hash<string,metric*>;
template class pair<string, metric*>;
template class vector< pair< string, metric*> >;
template class vector< dictionary_hash<string, metric *> :: hash_pair >;
template class vector< vector< dictionary_hash<string, metric *> :: hash_pair > >;
template class vector<dataValueType>;
template class BufferPool<dataValueType>;


template class dictionary<string, resource*>;
template class dictionary_hash<string, resource*>;
template class pair<string, resource*>;
template class vector< pair<string, resource*> >;
template class vector< dictionary_hash<string, resource*> :: hash_pair >;
template class vector< vector< dictionary_hash<string, resource*> :: hash_pair > >;

template class dictionary<string, resourceList*>;
template class dictionary_hash<string, resourceList*>;
template class pair<string, resourceList*>;
template class vector< pair<string, resourceList*> >;
template class vector< dictionary_hash<string, resourceList*> :: hash_pair >;
template class vector< vector< dictionary_hash<string, resourceList*> :: hash_pair > >;

template class dictionary<string, abstraction*>;
template class dictionary_hash<string, abstraction*>;
template class vector< dictionary_hash<string, abstraction*> :: hash_pair >;
template class vector< vector< dictionary_hash<string, abstraction*> :: hash_pair > >;
template class pair<string, abstraction*>;
template class vector< pair<string, abstraction*> >;
template class vector< vector< pair<string, abstraction*> > >;

template class vector<performanceStream *>;
template class pair< perfStreamHandle, performanceStream* >;
template class vector< pair< perfStreamHandle, performanceStream* > >;
template class dictionary<perfStreamHandle,performanceStream*>;
template class dictionary_hash<perfStreamHandle,performanceStream*>;
template class dictionary_hash_iter<perfStreamHandle,performanceStream*>;
template class vector< vector< dictionary_hash<perfStreamHandle, performanceStream *> :: hash_pair > >;
template class vector< dictionary_hash<perfStreamHandle, performanceStream *> :: hash_pair >;
template class dictionary_iter<unsigned, performanceStream *>;

template class dictionary<metricInstanceHandle,metricInstance*>;
template class pair<metricInstanceHandle, metricInstance*>;
template class vector< pair<metricInstanceHandle, metricInstance*> >; 
template class dictionary_hash<metricInstanceHandle,metricInstance*>;
template class dictionary_hash_iter<metricInstanceHandle,metricInstance*>;
template class vector< dictionary_hash<unsigned, metricInstance *> :: hash_pair >;
template class vector< vector< dictionary_hash<unsigned, metricInstance *> :: hash_pair > >;
template class dictionary_iter<unsigned, metricInstance *>;

template class List<sampleInfo*>;
template class ListItem<sampleInfo*>;
template class stack<T_dyninstRPC::buf_struct *>;
template class stack<T_visi::buf_struct *>;

/* ********************************
 * PCthread stuff
 */
#include "paradyn/src/PCthread/PCshg.h"
#include "paradyn/src/PCthread/PCevalTest.h"
#include "paradyn/src/PCthread/PCglobals.h"
#include "paradyn/src/PCthread/PCauto.h"
#include "paradyn/src/PCthread/PCwhen.h"
#include "paradyn/src/PCthread/PCwhere.h"
#include "paradyn/src/PCthread/PCwhy.h"

template class List<focus *>;
template class ListItem<focus *>;
template class List<focusList *>;
template class ListItem<focusList *>;
template class List<metricInstance *>;
template class ListItem<metricInstance *>;

template class HTable<PCmetric *>;
template class List<PCmetric *>;
template class ListItem<PCmetric *>;
template class HTable<datum *>;
template class HTable<focus *>;

template class List<datum *>;
template class ListItem<datum *>;
template class List<hint *>;
template class ListItem<hint *>;
template class List<hypothesis *>;
template class ListItem<hypothesis *>;
template class List<searchHistoryNode *>;
template class ListItem<searchHistoryNode *>;
template class List<test *>;
template class ListItem<test *>;
template class List<testResult *>;
template class ListItem<testResult *>;
template class List<timeInterval *>;
template class ListItem<timeInterval *>;

/* ******************************
 * TCthread stuff
 */
template class vector<tunableBooleanConstant>;
template class vector<tunableFloatConstant>;
template class dictionary_lite<string, tunableBooleanConstant>;
template class dictionary_lite<string, tunableFloatConstant>;
template class pair<string, tunableBooleanConstant>;
template class pair<string, tunableFloatConstant>;
template class vector< dictionary_lite<string, tunableBooleanConstant> :: hash_pair >;
template class vector< dictionary_lite<string, tunableFloatConstant> :: hash_pair >;
template class vector< vector< dictionary_lite<string, tunableBooleanConstant>::hash_pair > >;
template class vector< vector< dictionary_lite<string, tunableFloatConstant>::hash_pair > >;


/* *************************************
 * UIthread stuff
 */
// #include "paradyn/src/VMthread/metrespair.h"
#include "VM.thread.h"
#include "../src/UIthread/UIglobals.h"

template class List<metricInstInfo *>;
template class ListItem<metricInstInfo *>;
template class vector<VM_activeVisiInfo>;
template class vector<string*>;
template class dictionary<unsigned,string*>;
template class dictionary_hash<unsigned, string*>;
template class vector< pair<unsigned,string *> >;
template class pair<unsigned, string*>;
template class vector< vector<dictionary_hash<unsigned, string *>::hash_pair> >;
template class vector< dictionary_hash<unsigned, string *>::hash_pair >;

/* *************************************
 * UIthread WhereAxis stuff
 */

#include "paradyn/src/UIthread/where4tree.C"
#include "paradyn/src/UIthread/rootNode.h"
template class where4tree<whereAxisRootNode>;
template class vector<const whereAxisRootNode *>;
template class vector<where4tree<whereAxisRootNode>::childstruct>;
template class dictionary_lite< resourceHandle, where4tree<whereAxisRootNode> * >;
template class vector<where4tree<whereAxisRootNode> *>;
template class vector<dictionary_lite<resourceHandle,where4tree<whereAxisRootNode>*>::hash_pair>;
template class vector< vector< dictionary_lite<resourceHandle,where4tree<whereAxisRootNode>*>::hash_pair> >;
template class pair<resourceHandle, where4tree<whereAxisRootNode>* >;

#include "paradyn/src/UIthread/graphicalPath.C"
template class whereNodeGraphicalPath<whereAxisRootNode>;

#include "paradyn/src/UIthread/simpSeq.C"
template class simpSeq<unsigned>;

#include "paradyn/src/UIthread/abstractions.h"
template class vector<abstractions::whereAxisStruct>;

/* *************************************
 * UIthread Search History Graph stuff
 */

#include "paradyn/src/UIthread/shgRootNode.h"
template class where4tree<shgRootNode>;
template class vector<where4tree<shgRootNode>::childstruct>;
template class whereNodeGraphicalPath<shgRootNode>;
template class vector<const shgRootNode *>;

#include "paradyn/src/UIthread/shgPhases.h"
template class vector<shgPhases::shgStruct>;

template class dictionary_lite<unsigned, where4tree<shgRootNode> *>;
template class vector<where4tree<shgRootNode> *>;
template class vector<dictionary_lite<unsigned, where4tree<shgRootNode> *>::hash_pair>;
template class vector< vector< dictionary_lite<unsigned, where4tree<shgRootNode> *>::hash_pair> >;
template class pair<unsigned, where4tree<shgRootNode> *>;

template class dictionary_lite<where4tree<shgRootNode> *, where4tree<shgRootNode> *>;
template class vector<dictionary_lite<where4tree<shgRootNode> *, where4tree<shgRootNode> *>::hash_pair>;
template class vector< vector< dictionary_lite<where4tree<shgRootNode> *, where4tree<shgRootNode> *>::hash_pair> >;
template class pair<where4tree<shgRootNode> *, where4tree<shgRootNode> *>;

template class vector<Tk_3DBorder>; // shg consts

/* *************************************
 * UIthread Logo Stuff
 */

#include "paradyn/src/UIthread/pdLogo.h"
template class vector<pdLogo *>;
template class dictionary_lite<string, pdLogo *>;
template class pair<string, pdLogo *>;
template class vector<dictionary_lite<string, pdLogo *>::hash_pair>;
template class vector< vector<dictionary_lite<string,pdLogo*>::hash_pair> >;

template class dictionary_lite<string, pdLogo::logoStruct>;
template class vector<pdLogo::logoStruct>;
template class vector<dictionary_lite<string, pdLogo::logoStruct>::hash_pair>;
template class vector< vector<dictionary_lite<string, pdLogo::logoStruct>::hash_pair> >;
template class pair<string, pdLogo::logoStruct>;

/* *************************************
 * UIthread Misc Stuff
 */

#include "paradyn/src/UIthread/minmax.C"
template int min(int,int);
template int max(int,int);
template void ipmin(int &, int);
template void ipmax(int &, int);
template float min(float,float);
template float max(float,float);

/* ************************************
 * VMthread stuff
 */
#include "paradyn/src/VMthread/VMtypes.h"
#include "VM.thread.h"

template class vector<VMactiveStruct *>;
template class vector<VMvisisStruct *>;
template class vector<VM_visiInfo>;

/* ******************************
 * VISIthread stuff
 */
template class vector<metricInstInfo *>;

/* ***********************************
 * met stuff
 */
#include "paradyn/src/met/metParse.h" 
#include "paradyn/src/met/mdl.h" 

class stringList;
class daemonMet;
class processMet;
class visiMet;
class tunableMet;

template class vector<processMet *>;
template class vector<daemonMet*>;
template class vector<visiMet*>;
template class vector<tunableMet*>;
template class vector<string_list*>;
template class vector<pdFunction*>;
template class vector<module*>;
template class vector<instPoint*>;

// Igen - dyninstRPC stuff

template class vector<T_dyninstRPC::buf_struct*>;
template class queue<T_dyninstRPC::buf_struct*>;
template class vector<string>;
template class vector<T_dyninstRPC::mdl_expr*>;
template class vector<T_dyninstRPC::mdl_stmt*>;
template class vector<T_dyninstRPC::mdl_icode*>;
template class vector<T_dyninstRPC::mdl_constraint*>;
template class vector<T_dyninstRPC::metricInfo>;
template class vector<T_dyninstRPC::mdl_metric*>;
template bool_t T_dyninstRPC_P_xdr_stl(XDR*, vector<string>*, 
	bool_t (*)(XDR*, string*), string*);
template bool_t T_dyninstRPC_P_xdr_stl(XDR*, vector<T_dyninstRPC::mdl_expr*>*,
	bool_t (*)(XDR*, T_dyninstRPC::mdl_expr**), T_dyninstRPC::mdl_expr**);
template bool_t T_dyninstRPC_P_xdr_stl(XDR*, vector<u_int>*, 
	bool_t (*)(XDR*, u_int*), u_int*);
template bool_t T_dyninstRPC_P_xdr_stl(XDR*, vector<T_dyninstRPC::mdl_stmt*>*,
	bool_t (*)(XDR*, T_dyninstRPC::mdl_stmt**), T_dyninstRPC::mdl_stmt**);
template bool_t T_dyninstRPC_P_xdr_stl(XDR*, vector<T_dyninstRPC::mdl_icode*>*,
	bool_t (*)(XDR*, T_dyninstRPC::mdl_icode**), T_dyninstRPC::mdl_icode**);
template bool_t T_dyninstRPC_P_xdr_stl(XDR*, 
	vector<T_dyninstRPC::mdl_constraint*>*, 
	bool_t (*)(XDR*, T_dyninstRPC::mdl_constraint**), 
	T_dyninstRPC::mdl_constraint**);
template bool_t T_dyninstRPC_P_xdr_stl(XDR*, vector<T_dyninstRPC::metricInfo>*,
	bool_t (*)(XDR*, T_dyninstRPC::metricInfo*), T_dyninstRPC::metricInfo*);
template bool_t T_dyninstRPC_P_xdr_stl(XDR*, vector<T_dyninstRPC::mdl_metric*>*,
 	bool_t (*)(XDR*, T_dyninstRPC::mdl_metric**), 
	T_dyninstRPC::mdl_metric**);
template bool_t T_dyninstRPC_P_xdr_stl_PTR(XDR*, vector<string>**, 
	bool_t (*)(XDR*, string*), string*);
template bool_t T_dyninstRPC_P_xdr_stl_PTR(XDR*, 
	vector<T_dyninstRPC::mdl_expr*>**, 
	bool_t (*)(XDR*, T_dyninstRPC::mdl_expr**), T_dyninstRPC::mdl_expr**);
template bool_t T_dyninstRPC_P_xdr_stl_PTR(XDR*, 
	vector<u_int>**, bool_t (*)(XDR*, u_int*), u_int*);
template bool_t T_dyninstRPC_P_xdr_stl_PTR(XDR*, 
	vector<T_dyninstRPC::mdl_stmt*>**, 
	bool_t (*)(XDR*, T_dyninstRPC::mdl_stmt**), 
	T_dyninstRPC::mdl_stmt**);
template bool_t T_dyninstRPC_P_xdr_stl_PTR(XDR*, 
	vector<T_dyninstRPC::mdl_icode*>**, 
	bool_t (*)(XDR*, T_dyninstRPC::mdl_icode**), 
	T_dyninstRPC::mdl_icode**);
template bool_t T_dyninstRPC_P_xdr_stl_PTR(XDR*, 
	vector<T_dyninstRPC::mdl_constraint*>**, 
	bool_t (*)(XDR*, T_dyninstRPC::mdl_constraint**), 
	T_dyninstRPC::mdl_constraint**);
template bool_t T_dyninstRPC_P_xdr_stl_PTR(XDR*, 
	vector<T_dyninstRPC::metricInfo>**, 
	bool_t (*)(XDR*, T_dyninstRPC::metricInfo*), 
	T_dyninstRPC::metricInfo*);
template bool_t T_dyninstRPC_P_xdr_stl_PTR(XDR*, 
	vector<T_dyninstRPC::mdl_metric*>**, 
	bool_t (*)(XDR*, T_dyninstRPC::mdl_metric**), 
	T_dyninstRPC::mdl_metric**);


// MDL stuff 

template class vector<dataReqNode *>;
template class vector<mdl_var>;
template class vector<mdl_focus_element>;
template class vector<mdl_type_desc>;
template class pair< unsigned, vector<mdl_type_desc> >;
template class vector< pair< unsigned, vector<mdl_type_desc> > >;
template class dictionary<unsigned, vector<mdl_type_desc> >;
template class dictionary_hash<unsigned, vector<mdl_type_desc> >;
template class vector< vector< mdl_type_desc > >;
template class vector< dictionary_hash< unsigned, vector<mdl_type_desc> > :: hash_pair >;
template class vector< vector< dictionary_hash<unsigned, vector<mdl_type_desc> > :: hash_pair > >;

template bool_t T_dyninstRPC_P_xdr_stl(XDR*, vector<u_int>*,
			       bool_t (*)(XDR*, u_int*), u_int*);

template bool_t T_dyninstRPC_P_xdr_stl(XDR*, vector<T_dyninstRPC::mdl_expr*>*,
			       bool_t (*)(XDR*, T_dyninstRPC::mdl_expr**),
				       T_dyninstRPC::mdl_expr**);
template bool_t T_dyninstRPC_P_xdr_stl(XDR*, vector<T_dyninstRPC::mdl_stmt*>*,
			       bool_t (*)(XDR*, T_dyninstRPC::mdl_stmt**),
				       T_dyninstRPC::mdl_stmt**);
template bool_t T_dyninstRPC_P_xdr_stl(XDR*, vector<T_dyninstRPC::mdl_icode*>*,
			       bool_t (*)(XDR*, T_dyninstRPC::mdl_icode**),
			       T_dyninstRPC::mdl_icode**);
template bool_t T_dyninstRPC_P_xdr_stl(XDR*, vector<T_dyninstRPC::mdl_constraint*>*,
			       bool_t (*)(XDR*, T_dyninstRPC::mdl_constraint**),
			       T_dyninstRPC::mdl_constraint**);
template bool_t T_dyninstRPC_P_xdr_stl(XDR*, vector<T_dyninstRPC::mdl_metric*>*,
			       bool_t (*)(XDR*, T_dyninstRPC::mdl_metric**),
			       T_dyninstRPC::mdl_metric**);


template bool_t T_dyninstRPC_P_xdr_stl_PTR(XDR*, vector<T_dyninstRPC::mdl_expr*>**,
			   bool_t (*)(XDR*, T_dyninstRPC::mdl_expr**),
			   T_dyninstRPC::mdl_expr**);
template bool_t T_dyninstRPC_P_xdr_stl_PTR(XDR*, vector<T_dyninstRPC::mdl_stmt*>**,
				   bool_t (*)(XDR*, T_dyninstRPC::mdl_stmt**),
				   T_dyninstRPC::mdl_stmt**);
template bool_t T_dyninstRPC_P_xdr_stl_PTR(XDR*, vector<T_dyninstRPC::mdl_icode*>**,
				   bool_t (*)(XDR*, T_dyninstRPC::mdl_icode**),
				   T_dyninstRPC::mdl_icode**);
template bool_t T_dyninstRPC_P_xdr_stl_PTR(XDR*, vector<T_dyninstRPC::mdl_constraint*>**,
			   bool_t (*)(XDR*, T_dyninstRPC::mdl_constraint**),
			   T_dyninstRPC::mdl_constraint**);
template bool_t T_dyninstRPC_P_xdr_stl_PTR(XDR*, vector<T_dyninstRPC::mdl_metric*>**,
				   bool_t (*)(XDR*, T_dyninstRPC::mdl_metric**),
				   T_dyninstRPC::mdl_metric**);

// Igen - visi stuff

template class vector<T_visi::buf_struct*>;
template class queue<T_visi::buf_struct*>;
template class vector<T_visi::dataValue>;
template class vector<T_visi::visi_matrix>;
template class vector<T_visi::phase_info>;
template class vector<float>;
template bool_t T_visi_P_xdr_stl(XDR*, vector<string>*, 
	bool_t (*)(XDR*, string*), string*);
template bool_t T_visi_P_xdr_stl(XDR*, vector<T_visi::dataValue>*, 
	bool_t (*)(XDR*, T_visi::dataValue*), T_visi::dataValue*);
template bool_t T_visi_P_xdr_stl(XDR*, vector<T_visi::visi_matrix>*,
	bool_t (*)(XDR*, T_visi::visi_matrix*), T_visi::visi_matrix*);
template bool_t T_visi_P_xdr_stl(XDR*, vector<T_visi::phase_info>*, 
	bool_t (*)(XDR*, T_visi::phase_info*), T_visi::phase_info*);
template bool_t T_visi_P_xdr_stl(XDR*, vector<float>*, 
	bool_t (*)(XDR*, float*), float*);
template bool_t T_visi_P_xdr_stl_PTR(XDR*, vector<string>**, 
	bool_t (*)(XDR*, string*), string*);
template bool_t T_visi_P_xdr_stl_PTR(XDR*, vector<T_visi::dataValue>**, 
	bool_t (*)(XDR*, T_visi::dataValue*), T_visi::dataValue*);
template bool_t T_visi_P_xdr_stl_PTR(XDR*, vector<T_visi::visi_matrix>**,
	bool_t (*)(XDR*, T_visi::visi_matrix*), T_visi::visi_matrix*);
template bool_t T_visi_P_xdr_stl_PTR(XDR*, vector<T_visi::phase_info>**, 
	bool_t (*)(XDR*, T_visi::phase_info*), T_visi::phase_info*);
template bool_t T_visi_P_xdr_stl_PTR(XDR*, vector<float>**, 
	bool_t (*)(XDR*, float*), float*);

