//
// Copyright (c) 2012-2015 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <type_traits>
#include "boost/di/concepts/configurable.hpp"
#include "boost/di/providers/heap.hpp"
#include "boost/di/config.hpp"

namespace boost { namespace di { inline namespace v1 { namespace concepts {

test none = [] {
    class test_config { };
    expect(std::is_same<config<test_config>::requires_<provider<providable_type(...)>, policies<callable_type(...)>>, configurable<test_config>>::value);
};

class config_just_policies {
public:
    template<class T>
    static auto policies(const T&) noexcept { return di::make_policies(); }
};

test just_policies = [] {
    expect(std::is_same<config<config_just_policies>::requires_<provider<providable_type(...)>, policies<callable_type(...)>>, configurable<config_just_policies>>::value);
};

class config_just_provider {
public:
    template<class T>
    static auto provider(const T&) noexcept { return providers::heap{}; }
};

test just_provider = [] {
    expect(std::is_same<config<config_just_provider>::requires_<provider<providable_type(...)>, policies<callable_type(...)>>, configurable<config_just_provider>>::value);
};

class config_private_access {
private:
    template<class T>
    static auto policies(const T&) noexcept { return di::make_policies(); }

    template<class T>
    static auto provider(const T&) noexcept { return providers::heap{}; }
};

#if !defined(_MSC_VER)
    test private_access = [] {
        expect(std::is_same<config<config_private_access>::requires_<provider<providable_type(...)>, policies<callable_type(...)>>, configurable<config_private_access>>::value);
    };
#endif

class config_inheritance_impl {
public:
    template<class T>
    static auto policies(const T&) noexcept { return di::make_policies(); }

    template<class T>
    static auto provider(const T&) noexcept { return providers::heap{}; }
};

class config_inheritance : public config_inheritance_impl { };

test inheritance = [] {
    expect(configurable<config_inheritance>::value);
};

class config_okay {
public:
    template<class T>
    static auto policies(const T&) noexcept { return di::make_policies(); }

    template<class T>
    static auto provider(const T&) noexcept { return providers::heap{}; }
};

test okay = [] {
    expect(configurable<config_okay>::value);
};

}}}} // boost::di::v1::concepts

