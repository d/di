//
// Copyright (c) 2012-2015 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

//[module_view_hpp
//<-
#ifndef MODULE_VIEW_HPP
#define MODULE_VIEW_HPP
//->

#include <boost/di.hpp>

namespace di = boost::di;

class view;

di::injector<view> module_view() noexcept;

//<-
#endif
//->

//]

