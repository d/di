//
// Copyright (c) 2012-2015 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <memory>
#include <string>
#include <functional>
#include "boost/di/scopes/external.hpp"
#include "common/common.hpp"
#include "common/fakes/fake_provider.hpp"
#include "common/fakes/fake_injector.hpp"

namespace boost { namespace di { inline namespace v1 { namespace scopes {

struct interface { virtual ~interface() noexcept = default; virtual void dummy() = 0; };
struct implementation : public interface { virtual void dummy() { }; };

test from_arithmetic = [] {
    const int i = 42;
    expect(i == static_cast<int>(external::scope<int, int>{i}.create<void>(fake_provider<>{})));
};

test from_string = [] {
    const std::string s = "string";
    std::string object = external::scope<std::string, std::string>{s}.create<void>(fake_provider<>{});
    expect(s == object);
};

test from_ref = [] {
    struct c { } c_;
    c& c_ref_ = external::scope<c, c&>{c_}.create<void>(fake_provider<>{});
    expect(&c_ == &c_ref_);
};

test from_const_ref = [] {
    struct c { } c_;
    const c& c_ref_ = external::scope<c, c&>{c_}.create<void>(fake_provider<>{});
    expect(&c_ == &c_ref_);
};

test from_shared_ptr = [] {
    struct c { };
    auto c_ = std::make_shared<c>();
    std::shared_ptr<c> sp_c = external::scope<c, std::shared_ptr<c>>{c_}.create<void>(fake_provider<>{});
    expect(c_ == sp_c);
};

test from_context = [] {
    expect((
        static_cast<int>(external::scope<int, int>{87}.create<void>(fake_provider<>{}))
        !=
        static_cast<int>(external::scope<int, int>{42}.create<void>(fake_provider<>{}))
    ));

    struct c { };
    auto c1 = std::make_shared<c>();
    auto c2 = std::make_shared<c>();

    {
    std::shared_ptr<c> c1_ = external::scope<c, std::shared_ptr<c>>{c1}.create<void>(fake_provider<>{});
    std::shared_ptr<c> c2_ = external::scope<c, std::shared_ptr<c>>{c2}.create<void>(fake_provider<>{});
    expect(c1_ != c2_);
    }

    {
    std::shared_ptr<c> c1_ = external::scope<c, std::shared_ptr<c>>{c1}.create<void>(fake_provider<>{});
    std::shared_ptr<c> c2_ = external::scope<c, std::shared_ptr<c>>{c1}.create<void>(fake_provider<>{});
    expect(c1_ == c2_);
    }
};

test from_if_shared_ptr = [] {
    auto i = std::make_shared<implementation>();
    std::shared_ptr<interface> c = external::scope<interface, std::shared_ptr<interface>>{i}.create<void>(fake_provider<>{});
    expect(i == c);
};

test from_function_expr = [] {
    auto flag = false;
    auto expr = [&flag]() -> std::shared_ptr<interface> {
        if (!flag) {
            return std::make_shared<implementation>();
        }
        return nullptr;
    };

    external::scope<interface, decltype(expr)> external{expr};

    {
    std::shared_ptr<interface> sp = external.create<void>(fake_provider<>{});
    expect(dynamic_cast<implementation*>(sp.get()));
    }

    {
    flag = true;
    std::shared_ptr<interface> sp = external.create<void>(fake_provider<>{});
    expect(nullptr == sp);
    }
};

test from_function_expr_with_expected_function_expr = [] {
    constexpr auto i = 42;
    external::scope<function<int()>, function<int()>> external{[&]{ return i; }};
    function<int()> f = external.create<void>(fake_provider<>{});
    expect(i == f());
};

test from_function_expr_with_injector = [] {
    auto expr = [](const auto& injector) {
        return injector.template create<int>();
    };

    external::scope<int, decltype(expr)> external{expr};
    expect(0 == static_cast<int>(external.create<void>(fake_provider<int>{})));
};

}}}} // boost::di::v1::scopes

