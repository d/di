//
// Copyright (c) 2012-2015 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <memory>
#include "boost/di/providers/heap.hpp"
#include "boost/di/aux_/type_traits.hpp"
#include "boost/di/type_traits/memory_traits.hpp"

namespace boost { namespace di { inline namespace v1 { namespace providers {

template<class T, class TInit, class TMemory, class... TArgs>
auto test_heap(const aux::type<T>&, const TInit& init, const TMemory& memory, const TArgs&... args) {
    std::unique_ptr<T> object{heap{}.get<T, T>(init, memory, args...)};
    expect(object.get());
}

test get_no_args = [] {
    struct c { };
    test_heap(aux::type<int>{}, type_traits::direct{}, type_traits::stack{});
    test_heap(aux::type<int>{}, type_traits::direct{}, type_traits::heap{});
    test_heap(aux::type<int>{}, type_traits::uniform{}, type_traits::stack{});
    test_heap(aux::type<int>{}, type_traits::uniform{}, type_traits::heap{});
    test_heap(aux::type<c>{}, type_traits::direct{}, type_traits::stack{});
    test_heap(aux::type<c>{}, type_traits::direct{}, type_traits::heap{});
    test_heap(aux::type<c>{}, type_traits::uniform{}, type_traits::stack{});
    test_heap(aux::type<c>{}, type_traits::uniform{}, type_traits::heap{});
};

test get_with_args = [] {
    struct direct { direct(int, double) { } };
    struct uniform { int i = 0; double d = 0.0; };
    test_heap(aux::type<direct>{}, type_traits::direct{}, type_traits::stack{}, int{}, double{});
    test_heap(aux::type<direct>{}, type_traits::direct{}, type_traits::heap{}, int{}, double{});
    test_heap(aux::type<direct>{}, type_traits::uniform{}, type_traits::stack{}, int{}, double{});
    test_heap(aux::type<direct>{}, type_traits::uniform{}, type_traits::heap{}, int{}, double{});
#if !defined(_MSC_VER)
    test_heap(aux::type<uniform>{}, type_traits::uniform{}, type_traits::stack{}, int{}, double{});
    test_heap(aux::type<uniform>{}, type_traits::uniform{}, type_traits::heap{}, int{}, double{});
#endif
};

}}}} // boost::di::v1::providers

