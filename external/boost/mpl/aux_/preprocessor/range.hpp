
#ifndef BOOST_MPL_AUX_PREPROCESSOR_RANGE_HPP_INCLUDED
#define BOOST_MPL_AUX_PREPROCESSOR_RANGE_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2002-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Source: /home/jaw/paradyn_2008_10_14/CVSROOT/core/external/boost/mpl/aux_/preprocessor/range.hpp,v $
// $Date: 2008/07/14 21:54:49 $
// $Revision: 1.1 $

#include <boost/preprocessor/seq/subseq.hpp>

#define BOOST_MPL_PP_RANGE(first, length) \
    BOOST_PP_SEQ_SUBSEQ((0)(1)(2)(3)(4)(5)(6)(7)(8)(9), first, length) \
/**/

#endif // BOOST_MPL_AUX_PREPROCESSOR_RANGE_HPP_INCLUDED