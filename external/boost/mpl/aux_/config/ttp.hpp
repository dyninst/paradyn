
#ifndef BOOST_MPL_AUX_CONFIG_TTP_HPP_INCLUDED
#define BOOST_MPL_AUX_CONFIG_TTP_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Source: /home/jaw/paradyn_2008_10_14/CVSROOT/core/external/boost/mpl/aux_/config/ttp.hpp,v $
// $Date: 2007/12/04 21:48:02 $
// $Revision: 1.1 $

#include <boost/mpl/aux_/config/msvc.hpp>
#include <boost/mpl/aux_/config/gcc.hpp>
#include <boost/mpl/aux_/config/workaround.hpp>

#if !defined(BOOST_MPL_CFG_NO_TEMPLATE_TEMPLATE_PARAMETERS) \
    && defined(BOOST_NO_TEMPLATE_TEMPLATES)

#   define BOOST_MPL_CFG_NO_TEMPLATE_TEMPLATE_PARAMETERS

#endif


#if    !defined(BOOST_MPL_CFG_EXTENDED_TEMPLATE_PARAMETERS_MATCHING) \
    && !defined(BOOST_MPL_PREPROCESSING_MODE) \
    && (   BOOST_WORKAROUND(BOOST_MPL_CFG_GCC, BOOST_TESTED_AT(0x0302)) \
        || BOOST_WORKAROUND(__BORLANDC__, < 0x600) \
        )

#   define BOOST_MPL_CFG_EXTENDED_TEMPLATE_PARAMETERS_MATCHING

#endif

#endif // BOOST_MPL_AUX_CONFIG_TTP_HPP_INCLUDED
