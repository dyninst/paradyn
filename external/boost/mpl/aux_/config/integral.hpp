
#ifndef BOOST_MPL_AUX_CONFIG_INTEGRAL_HPP_INCLUDED
#define BOOST_MPL_AUX_CONFIG_INTEGRAL_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Source: /home/jaw/paradyn_2008_10_14/CVSROOT/core/external/boost/mpl/aux_/config/integral.hpp,v $
// $Date: 2007/12/04 21:47:54 $
// $Revision: 1.1 $

#include <boost/mpl/aux_/config/msvc.hpp>
#include <boost/mpl/aux_/config/workaround.hpp>

#if    !defined(BOOST_MPL_CFG_BCC_INTEGRAL_CONSTANTS) \
    && !defined(BOOST_MPL_PREPROCESSING_MODE) \
    && BOOST_WORKAROUND(__BORLANDC__, < 0x600)

#   define BOOST_MPL_CFG_BCC_INTEGRAL_CONSTANTS

#endif

#if    !defined(BOOST_MPL_CFG_NO_NESTED_VALUE_ARITHMETIC) \
    && !defined(BOOST_MPL_PREPROCESSING_MODE) \
    && ( BOOST_WORKAROUND(BOOST_MSVC, <= 1300) \
        || BOOST_WORKAROUND(__EDG_VERSION__, <= 238) \
        )

#   define BOOST_MPL_CFG_NO_NESTED_VALUE_ARITHMETIC

#endif

#endif // BOOST_MPL_AUX_CONFIG_INTEGRAL_HPP_INCLUDED
