//
// Copyright (c) 2012-2015 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <cassert>
#include <boost/di.hpp>

namespace di = boost::di;

struct c {
    int a; // = 0; MSVC_2015 doesn't support defaults
    double d; // = 0.0;
};

int main() {
    auto injector = di::make_injector(
        di::bind<int>().to(42)
      , di::bind<double>().to(87.0)
    );

    auto object = injector.create<c>();
    assert(42 == object.a);
    assert(87.0 == object.d);
}

