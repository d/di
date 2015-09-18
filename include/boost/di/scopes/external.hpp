//
// Copyright (c) 2012-2015 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_DI_SCOPES_EXTERNAL_HPP
#define BOOST_DI_SCOPES_EXTERNAL_HPP

#include "boost/di/aux_/type_traits.hpp"
#include "boost/di/wrappers/shared.hpp"
#include "boost/di/wrappers/unique.hpp"

namespace boost { namespace di { inline namespace v1 { namespace scopes {

namespace detail {
struct base_impl { void operator()(...) { } };

template<class T>
struct base : base_impl, std::conditional_t<std::is_class<T>::value, T, aux::none_type> { };

template<typename U>
std::false_type is_callable_impl(U*, aux::non_type<void (base_impl::*)(...), &U::operator()>* = 0);
std::true_type is_callable_impl(...);

template<class T>
using is_callable = decltype(is_callable_impl((base<T>*)0));

template<class T, class TExpected, class TGiven>
struct arg {
    using type = T;
    using expected = TExpected;
    using given = TGiven;
};

template<class T>
struct wrapper_traits {
    using type = wrappers::unique<T>;
};

template<class T>
struct wrapper_traits<std::shared_ptr<T>> {
    using type = wrappers::shared<T>;
};

template<class T>
using wrapper_traits_t = typename wrapper_traits<T>::type;

BOOST_DI_HAS_TYPE(has_result_type, result_type);

} // detail

class external {
public:
    template<class TExpected, class, class = int>
    struct scope {
        template<class>
        using is_referable = std::false_type;

        explicit scope(const TExpected& object)
            : object_{object}
        { }

        template<class, class TProvider>
        static TExpected try_create(const TProvider&);

        template<class, class TProvider>
        auto create(const TProvider&) const noexcept {
            return wrappers::unique<TExpected>{object_};
        }

        TExpected object_;
    };

    template<class TExpected, class TGiven>
    struct scope<TExpected, std::shared_ptr<TGiven>> {
        template<class T>
        using is_referable =
            typename wrappers::shared<TGiven>::template is_referable<aux::remove_specifiers_t<T>>;

        explicit scope(const std::shared_ptr<TGiven>& object)
            : object_{object}
        { }

        template<class, class TProvider>
        static wrappers::shared<TGiven> try_create(const TProvider&);

        template<class, class TProvider>
        auto create(const TProvider&) const noexcept {
            return wrappers::shared<TGiven>{object_};
        }

        std::shared_ptr<TGiven> object_;
    };

    template<class TExpected, class TGiven>
    struct scope<TExpected, TGiven&, BOOST_DI_REQUIRES(!detail::is_callable<TGiven>::value)> {
        template<class>
        using is_referable = std::true_type;

        explicit scope(TGiven& object)
            : object_{object}
        { }

        template<class, class TProvider>
        static wrappers::shared<TGiven&> try_create(const TProvider&);

        template<class, class TProvider>
        auto create(const TProvider&) const noexcept {
            return object_;
        }

        wrappers::shared<TGiven&> object_;
    };

    template<class TExpected, class TGiven>
    struct scope<TExpected, TGiven,
        BOOST_DI_REQUIRES(!detail::is_callable<TExpected>::value &&
                           detail::is_callable<TGiven>::value &&
                          !detail::has_result_type<TGiven>::value)> {
        template<class>
        using is_referable = std::false_type;

        explicit scope(const TGiven& object)
            : object_(object)
        { }

        template<class T, class TProvider>
        T static try_create(const TProvider&);

        template<class, class TProvider,
            BOOST_DI_REQUIRES(!aux::is_callable_with<TGiven, decltype(std::declval<TProvider>().injector_)>::value &&
                               aux::is_callable_with<TGiven>::value) = 0>
        auto create(const TProvider&) const noexcept {
            using wrapper = detail::wrapper_traits_t<decltype(std::declval<TGiven>()())>;
            return wrapper{object_()};
        }

        template<class, class TProvider,
            BOOST_DI_REQUIRES(aux::is_callable_with<TGiven, decltype(std::declval<TProvider>().injector_)>::value) = 0>
        auto create(const TProvider& provider) noexcept {
            using wrapper = detail::wrapper_traits_t<decltype((object_)(provider.injector_))>;
            return wrapper{(object_)(provider.injector_)};
        }

        template<class T, class TProvider,
            BOOST_DI_REQUIRES(aux::is_callable_with<TGiven, decltype(std::declval<TProvider>().injector_), const detail::arg<T, TExpected, TGiven>&>::value) = 0>
        auto create(const TProvider& provider) noexcept {
            using wrapper = detail::wrapper_traits_t<decltype((object_)(provider.injector_, detail::arg<T, TExpected, TGiven>{}))>;
            return wrapper{(object_)(provider.injector_, detail::arg<T, TExpected, TGiven>{})};
        }

        TGiven object_;
    };
};

}}}} // boost::di::v1::scopes

#endif

