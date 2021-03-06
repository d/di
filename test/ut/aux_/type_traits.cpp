//
// Copyright (c) 2012-2015 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <memory>
#include <string>
#include <vector>
#include <set>
#include "boost/di/aux_/type_traits.hpp"
#include "boost/di/fwd.hpp"
#include "common/common.hpp"

namespace boost { namespace di { inline namespace v1 { namespace aux {

BOOST_DI_HAS_TYPE(has_has, has);

test has_type = [] {
    struct a { };
    struct c { using has = void; };

    static_expect(has_has<c>::value);
    static_expect(!has_has<void>::value);
    static_expect(!has_has<a>::value);
};

BOOST_DI_HAS_METHOD(has_call, call);

test has_method = [] {
    struct a { };
    struct call1 { void call(int) { }; };
    struct call2 { int call(const double&) { return {}; }; };
    struct call3 { int call(int, double = 0.0) const noexcept { return {}; }; };

    static_expect(!has_call<a, int>::value);
    static_expect(!has_call<call1>::value);
    static_expect(has_call<call1, int>::value);
    static_expect(has_call<call2, double>::value);
    static_expect(has_call<call2, const double&>::value);
    static_expect(!has_call<call2, std::string>::value);
    static_expect(has_call<call2, float>::value); // convertible to double
    static_expect(!has_call<call3>::value);
    static_expect(has_call<call3, int>::value);
    static_expect(has_call<call3, int, double>::value);
    static_expect(!has_call<call3, int, double, float>::value);
};

test is_braces_constructible_types = [] {
    struct c { };
    struct ctor { ctor(int) { } };
    struct ctor_def_value { ctor_def_value(int = 0) { } };
    struct ctor_def { ctor_def() noexcept = default; };
    struct agg1 { int i = {}; };
    struct agg2 { int& i; double d = {}; };
    struct agg3 { int i; double d; float f; };

    static_expect(is_braces_constructible<int>::value);
    static_expect(is_braces_constructible<c>::value);
    static_expect(is_braces_constructible<ctor, int>::value);
    static_expect(!is_braces_constructible<ctor>::value);
    static_expect(is_braces_constructible<ctor_def_value>::value);
    static_expect(is_braces_constructible<ctor_def_value, int>::value);
    static_expect(is_braces_constructible<ctor_def>::value);
    static_expect(!is_braces_constructible<ctor_def, int>::value);
    static_expect(is_braces_constructible<agg1>::value);

    static_expect(!is_braces_constructible<agg1, int, double>::value);
#if !defined(_MSC_VER)
    static_expect(is_braces_constructible<agg1, int>::value);
    static_expect(is_braces_constructible<agg2, int&>::value);
    static_expect(is_braces_constructible<agg2, int&, double>::value);
#endif
    static_expect(is_braces_constructible<agg3, int, double, float>::value);
};

test is_narrowed_types = [] {
    static_expect(!is_narrowed<int, int>::value);
    static_expect(!is_narrowed<float, float>::value);
    static_expect(!is_narrowed<double, double>::value);
    static_expect(is_narrowed<int, double>::value);
    static_expect(is_narrowed<int, float>::value);
    static_expect(is_narrowed<float, int>::value);
    static_expect(is_narrowed<double, int>::value);
    static_expect(is_narrowed<float, double>::value);
    struct c {};
    static_expect(!is_narrowed<int, c>::value);
};

test remove_qualifiers_types = [] {
    static_expect(std::is_same<int, remove_qualifiers_t<int>>::value);
    static_expect(std::is_same<int, remove_qualifiers_t<int&>>::value);
    static_expect(std::is_same<int, remove_qualifiers_t<int*>>::value);
    static_expect(std::is_same<int, remove_qualifiers_t<const int*>>::value);
};

test deref_types = [] {
    static_expect(std::is_same<typename deref_type<void>::type, void>::value);
    static_expect(std::is_same<typename deref_type<int>::type, int>::value);
    static_expect(std::is_same<typename deref_type<std::unique_ptr<int>>::type, int>::value);
    static_expect(std::is_same<typename deref_type<std::unique_ptr<int, deleter<int>>>::type, int>::value);
    static_expect(std::is_same<typename deref_type<std::shared_ptr<int>>::type, int>::value);
    static_expect(std::is_same<typename deref_type<boost::shared_ptr<int>>::type, int>::value);
    static_expect(std::is_same<typename deref_type<std::weak_ptr<int>>::type, int>::value);
};

test decay_types = [] {
    auto test = [] (auto type) {
        using T = decltype(type);
        static_expect(std::is_same<T, decay_t<T>>::value);
        static_expect(std::is_same<T, decay_t<T*>>::value);
        static_expect(std::is_same<T, decay_t<const T*>>::value);
        static_expect(std::is_same<T, decay_t<const T>>::value);
        static_expect(std::is_same<T, decay_t<const T&>>::value);
        static_expect(std::is_same<T, decay_t<T&>>::value);
        static_expect(std::is_same<T, decay_t<std::shared_ptr<T>>>::value);
        static_expect(std::is_same<T, decay_t<boost::shared_ptr<T>>>::value);
        static_expect(std::is_same<T, decay_t<const std::shared_ptr<T>&>>::value);
        static_expect(std::is_same<T, decay_t<const boost::shared_ptr<T>&>>::value);
        static_expect(std::is_same<T, decay_t<std::shared_ptr<T>&>>::value);
        static_expect(std::is_same<T, decay_t<boost::shared_ptr<T>&>>::value);
        static_expect(std::is_same<T, decay_t<T&&>>::value);
        static_expect(std::is_same<core::array<T*[]>, decay_t<std::vector<std::shared_ptr<T>>>>::value);
        static_expect(std::is_same<core::array<T*[]>, decay_t<std::shared_ptr<std::vector<std::shared_ptr<T>>>>>::value);
        static_expect(std::is_same<core::array<T*[]>, decay_t<std::set<std::shared_ptr<T>>>>::value);
        static_expect(std::is_same<core::array<T*[]>, decay_t<std::shared_ptr<std::set<std::shared_ptr<T>>>>>::value);
    };

    struct c { };
    test(c{});
    test(int{});
};

void f1() { }
int f2(int) { return {}; }
int f3(int, const double&) { return {}; }

struct c1 {
    void f1() { }
    int f2(int) { return {}; }
    int f3(int, const double&) { return {}; }
};

struct c2 {
    void f1() const { }
    int f2(int) const { return {}; }
    int f3(int, const double&) const { return {}; }
};

test function_traits_parameters_type_functions = [] {
    static_expect(std::is_same<void, typename function_traits<decltype(&f1)>::result_type>::value);
    static_expect(std::is_same<type_list<>, typename function_traits<decltype(&f1)>::args>::value);

    static_expect(std::is_same<int, typename function_traits<decltype(&f2)>::result_type>::value);
    static_expect(std::is_same<type_list<int>, typename function_traits<decltype(&f2)>::args>::value);

    static_expect(std::is_same<int, typename function_traits<decltype(&f3)>::result_type>::value);
    static_expect(std::is_same<type_list<int, const double&>, typename function_traits<decltype(&f3)>::args>::value);
};

test function_traits_parameters_type_methods = [] {
    static_expect(std::is_same<void, typename function_traits<decltype(&c1::f1)>::result_type>::value);
    static_expect(std::is_same<type_list<>, typename function_traits<decltype(&c1::f1)>::args>::value);

    static_expect(std::is_same<int, typename function_traits<decltype(&c1::f2)>::result_type>::value);
    static_expect(std::is_same<type_list<int>, typename function_traits<decltype(&c1::f2)>::args>::value);

    static_expect(std::is_same<int, typename function_traits<decltype(&c1::f3)>::result_type>::value);
    static_expect(std::is_same<type_list<int, const double&>, typename function_traits<decltype(&c1::f3)>::args>::value);
};

test function_traits_parameters_type_const_methods = [] {
    static_expect(std::is_same<void, typename function_traits<decltype(&c2::f1)>::result_type>::value);
    static_expect(std::is_same<type_list<>, typename function_traits<decltype(&c2::f1)>::args>::value);

    static_expect(std::is_same<int, typename function_traits<decltype(&c2::f2)>::result_type>::value);
    static_expect(std::is_same<type_list<int>, typename function_traits<decltype(&c2::f2)>::args>::value);

    static_expect(std::is_same<int, typename function_traits<decltype(&c2::f3)>::result_type>::value);
    static_expect(std::is_same<type_list<int, const double&>, typename function_traits<decltype(&c2::f3)>::args>::value);
};

class fwd;
test is_complete_types = [] {
    struct c;
    static_expect(!is_complete<c>::value);
    static_expect(!is_complete<class Fwd>::value);
    static_expect(!is_complete<fwd>::value);
    struct complete { };
    struct i { virtual ~i() = 0; };
    static_expect(is_complete<i>::value);
    static_expect(is_complete<complete>::value);
    static_expect(is_complete<int>::value);
};

test is_unique_types = [] {
    static_expect(is_unique<>::value);
    static_expect(is_unique<int>::value);
    static_expect(is_unique<int, double, float>::value);
    static_expect(!is_unique<int, int>::value);
    static_expect(!is_unique<int, double, int>::value);
    static_expect(!is_unique<int, double, double, int, int>::value);
    static_expect(std::is_same<not_unique<int>, is_unique<int, int>::type>::value);
    static_expect(std::is_same<not_unique<int>, is_unique<float, int, double, int>::type>::value);
};

}}}} // boost::di::v1::aux

