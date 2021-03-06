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
#include "paradynd/src/mdld_data.h"



T_dyninstRPC::mdl_v_expr*
mdld_data::new_v_expr( int int_literal )
{
    return new mdld_v_expr( int_literal );
}


T_dyninstRPC::mdl_v_expr*
mdld_data::new_v_expr( pdstring a_str, bool is_literal )
{
    return new mdld_v_expr( a_str, is_literal );
}


T_dyninstRPC::mdl_v_expr*
mdld_data::new_v_expr( T_dyninstRPC::mdl_expr* expr,
                         pdvector<pdstring> fields )
{
    return new mdld_v_expr( expr, fields );
}


T_dyninstRPC::mdl_v_expr*
mdld_data::new_v_expr( pdstring func_name,
                         pdvector<T_dyninstRPC::mdl_expr*>* args )
{
    return new mdld_v_expr( func_name, args );
}


T_dyninstRPC::mdl_v_expr*
mdld_data::new_v_expr( u_int bin_op,
                         T_dyninstRPC::mdl_expr* left,
                         T_dyninstRPC::mdl_expr* right )
{
    return new mdld_v_expr( bin_op, left, right );
}


T_dyninstRPC::mdl_v_expr*
mdld_data::new_v_expr( pdstring var,
                        u_int assign_op,
                        T_dyninstRPC::mdl_expr* expr )
{
    return new mdld_v_expr( var, assign_op, expr );
}


T_dyninstRPC::mdl_v_expr*
mdld_data::new_v_expr( u_int u_op,
                        T_dyninstRPC::mdl_expr* expr,
                        bool is_preop )
{
    return new mdld_v_expr( u_op, expr, is_preop );
}


T_dyninstRPC::mdl_v_expr*
mdld_data::new_v_expr( pdstring var, T_dyninstRPC::mdl_expr* index_expr )
{
    return new mdld_v_expr( var, index_expr );    
}


#if READY
T_dyninstRPC::mdl_v_expr*
mdld_data::new_v_expr( u_int type,
                        int intLiteral,
                        pdstring strLiteral,
                        pdstring var,
                        u_int binOp,
                        u_int unOp,
                        T_dyninstRPC::mdl_expr* leftExpr,
                        T_dyninstRPC::mdl_expr* rightExpr,
                        pdvector<T_dyninstRPC::mdl_expr*>* args,
                        const pdvector<pdstring>& fields,
                        const pdvector<u_int>& type_walk,
                        bool useTypeWalk,
                        bool isOK )
{
    return new mdld_v_expr( type,
                        intLiteral,
                        strLiteral,
                        var,
                        binOp,
                        unOp,
                        leftExpr,
                        rightExpr,
                        args,
                        fields,
                        type_walk,
                        useTypeWalk,
                        isOK );
}
#endif // READY

T_dyninstRPC::mdl_icode*
mdld_data::new_icode( T_dyninstRPC::mdl_expr* if_expr,
                        T_dyninstRPC::mdl_expr* expr )
{
    return new mdld_icode( if_expr, expr );
}



T_dyninstRPC::mdl_list_stmt*
mdld_data::new_list_stmt( u_int type,
                            pdstring ident,
                            pdvector<pdstring>* elems,
                            bool is_lib,
                            pdvector<pdstring>* flavor )
{
    return new mdld_list_stmt( type,
                                ident,
                                elems,
                                is_lib,
                                flavor );
}



T_dyninstRPC::mdl_for_stmt*
mdld_data::new_for_stmt( pdstring index_name,
                            T_dyninstRPC::mdl_expr* list_exp,
                            T_dyninstRPC::mdl_stmt* body )
{
    return new mdld_for_stmt( index_name, list_exp, body );
}




T_dyninstRPC::mdl_if_stmt*
mdld_data::new_if_stmt( T_dyninstRPC::mdl_expr* expr,
                            T_dyninstRPC::mdl_stmt* body )
{
    return new mdld_if_stmt( expr, body );
}



T_dyninstRPC::mdl_seq_stmt*
mdld_data::new_seq_stmt( pdvector<T_dyninstRPC::mdl_stmt*>* stmts )
{
    return new mdld_seq_stmt( stmts );
}



T_dyninstRPC::mdl_instr_stmt*
mdld_data::new_instr_stmt( u_int pos,
                            T_dyninstRPC::mdl_expr* point_expr,
                            pdvector<T_dyninstRPC::mdl_icode*>* i_reqs,
                            unsigned where_instr,
                            bool constrained )
{
    return new mdld_instr_stmt( pos,
                                    point_expr,
                                    i_reqs,
                                    where_instr,
                                    constrained );
}




T_dyninstRPC::mdl_constraint*
mdld_data::new_constraint( pdstring id,
                            pdvector<pdstring>* path,
                            pdvector<T_dyninstRPC::mdl_stmt*>* stmts,
                            bool replace,
                            u_int d_type )
{
    bool error;
    T_dyninstRPC::mdl_constraint* ret = new mdld_constraint( id,
                                                                path,
                                                                stmts,
                                                                replace,
                                                                d_type,
                                                                error );
    if( error )
    {
        delete ret;
        ret = NULL;
    }
    return ret;
}



bool
mdld_data::new_metric(pdstring id,
                        pdstring name,
                        pdstring units,
			            u_int agg,
                        u_int sty,
                        u_int type,
                        pdstring hwcntr, 
			            pdvector<T_dyninstRPC::mdl_stmt*>* mv, 
			            pdvector<pdstring>* flavs,
			            pdvector<T_dyninstRPC::mdl_constraint*>* cons,
		                pdvector<pdstring>* temp_counters,
			            bool developerMode,
			            int normalized)
{
    T_dyninstRPC::mdl_metric* m = new mdld_metric(id,
                                                    name,
                                                    units,
                                                    agg, 
                                                    sty,
                                                    type,
                                                    hwcntr,
                                                    mv,
                                                    flavs,
                                                    cons,
                                                    temp_counters,
                                                    developerMode,
                                                    normalized);
    if( m != NULL )
    {
        unique_name( id );
        all_metrics += m;
    }
    return ( m != NULL );
}



