//
// Copyright (c) 2012-2015 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_DI_AUX_TYPE_TRAITS_HPP
#define BOOST_DI_AUX_TYPE_TRAITS_HPP

#include "boost/di/aux_/utility.hpp"
#include "boost/di/fwd.hpp"

#define BOOST_DI_HAS_TYPE(name, call_name)                              \
    template<class, class = int>                                        \
    struct name : ::boost::di::aux::false_type { };                     \
                                                                        \
    template<class T>                                                   \
    struct name<T, ::boost::di::aux::valid_t<typename T::call_name>>    \
        : ::boost::di::aux::true_type                                   \
    { }

#define BOOST_DI_HAS_METHOD(name, call_name)                            \
    template<class T, class... TArgs>                                   \
    decltype(::boost::di::aux::declval<T>().call_name(                  \
             ::boost::di::aux::declval<TArgs>()...)                     \
           , ::boost::di::aux::true_type())                             \
    name##_impl(int);                                                   \
                                                                        \
    template<class, class...>                                           \
    ::boost::di::aux::false_type name##_impl(...);                      \
                                                                        \
    template<class T, class... TArgs>                                   \
    struct name : decltype(name##_impl<T, TArgs...>(0)) { }

#define BOOST_DI_REQUIRES(...) typename ::boost::di::aux::enable_if<__VA_ARGS__, int>::type // __pph__
#define BOOST_DI_REQUIRES_MSG(...) typename ::boost::di::aux::concept_check<__VA_ARGS__>::type // __pph__

namespace boost { namespace di { inline namespace v1 { namespace aux {

template<class T> T&& declval();

template<class T, T V>
struct integral_constant {
    using type = integral_constant;
    static constexpr T value = V;
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

template<bool B, class T, class F> struct conditional { using type = T; };
template<class T, class F> struct conditional<false, T, F> { using type = F; };
template<bool B, class T, class F> using conditional_t = typename conditional<B, T, F>::type;

template<bool B, class T = void> struct enable_if { };
template<class T> struct enable_if<true, T> { using type = T; };
template<bool B, class T = void> using enable_if_t = typename enable_if<B, T>::type;

template<class T> struct concept_check { static_assert(T::value, "constraint not satisfied"); };
template<> struct concept_check<true_type> { using type = int; };

template<class T> struct remove_reference { using type = T;};
template<class T> struct remove_reference<T&> { using type = T;};
template<class T> struct remove_reference<T&&> { using type = T;};
template<class T> using remove_reference_t = typename remove_reference<T>::type;

template<class T> struct remove_qualifiers { using type = T; };
template<class T> struct remove_qualifiers<const T> { using type = T; };
template<class T> struct remove_qualifiers<T&> { using type = T; };
template<class T> struct remove_qualifiers<const T&> { using type = T; };
template<class T> struct remove_qualifiers<T*> { using type = T; };
template<class T> struct remove_qualifiers<const T*> { using type = T; };
template<class T> struct remove_qualifiers<T* const &> { using type = T; };
template<class T> struct remove_qualifiers<T* const> { using type = T; };
template<class T> struct remove_qualifiers<const T* const> { using type = T; };
template<class T> struct remove_qualifiers<T&&> { using type = T; };
template<class T> using remove_qualifiers_t = typename remove_qualifiers<T>::type;

template<class T>
struct deref_type { using type = T; };

template<class T, class TDeleter>
struct deref_type<std::unique_ptr<T, TDeleter>> {
    using type = remove_qualifiers_t<typename deref_type<T>::type>;
};

template<class T>
struct deref_type<std::shared_ptr<T>> {
    using type = remove_qualifiers_t<typename deref_type<T>::type>;
};

template<class T>
struct deref_type<boost::shared_ptr<T>> {
    using type = remove_qualifiers_t<typename deref_type<T>::type>;
};

template<class T>
struct deref_type<std::weak_ptr<T>> {
    using type = remove_qualifiers_t<typename deref_type<T>::type>;
};

template<class T, class TAllocator>
struct deref_type<std::vector<T, TAllocator>> {
    using type = core::array<remove_qualifiers_t<typename deref_type<T>::type>*[]>;
};

template<class TKey, class TCompare, class TAllocator>
struct deref_type<std::set<TKey, TCompare, TAllocator>> {
    using type = core::array<remove_qualifiers_t<typename deref_type<TKey>::type>*[]>;
};

template<class T>
using decay_t = typename deref_type<remove_qualifiers_t<T>>::type;

template<class, class> struct is_same : false_type { };
template<class T> struct is_same<T, T> : true_type { };

template<class T, class U> struct is_base_of : integral_constant<bool, __is_base_of(T, U)> { };

template<class T> struct is_class : integral_constant<bool, __is_class(T)> { };
template<class T> struct is_abstract : integral_constant<bool, __is_abstract(T)> { };
template<class T> struct is_polymorphic : integral_constant<bool, __is_polymorphic(T)> { };
template<class...> using is_valid_expr = true_type;

template<class T, class... TArgs>
decltype(void(T(declval<TArgs>()...)), true_type{}) test_is_constructible(int);

template<class, class...>
false_type test_is_constructible(...);

#if defined(_MSC_VER) // __pph__
    template<class T, class... TArgs>
    struct is_constructible : decltype(test_is_constructible<T, TArgs...>(0)) { };
#else // __pph__
    template<class T, class... TArgs>
    using is_constructible = decltype(test_is_constructible<T, TArgs...>(0));
#endif // __pph__

template<class T, class... TArgs>
using is_constructible_t = typename is_constructible<T, TArgs...>::type;

template<class T, class... TArgs>
decltype(void(T{declval<TArgs>()...}), true_type{}) test_is_braces_constructible(int);

template<class, class...>
false_type test_is_braces_constructible(...);

template<class T, class... TArgs>
using is_braces_constructible = decltype(test_is_braces_constructible<T, TArgs...>(0));

template<class T, class... TArgs>
using is_braces_constructible_t = typename is_braces_constructible<T, TArgs...>::type;

#if defined(_MSC_VER) // __pph__
    template<class T>
    struct is_copy_constructible : integral_constant<bool, __is_constructible(T, const T&)> { };
    template<class T>
    struct is_default_constructible : integral_constant<bool, __is_constructible(T)> { };
#else // __pph__
    template<class T>
    using is_copy_constructible = is_constructible<T, const T&>;
    template<class T>
    using is_default_constructible = is_constructible<T>;
#endif // __pph__

#if defined(__clang__) || defined(_MSC_VER) // __pph__
    template<class T, class U>
    struct is_convertible : integral_constant<bool, __is_convertible_to(T, U)> { };
#else // __pph__
    struct test_is_convertible__ {
        template<class T> static void test(T);
    };

    template<class T, class U, class = decltype(test_is_convertible__::test<U>(declval<T>()))>
    true_type test_is_convertible(int);

    template<class, class>
    false_type test_is_convertible(...);

    template<class T, class U>
    using is_convertible = decltype(test_is_convertible<T, U>(0));
#endif // __pph__

template<class TSrc, class TDst, class U = remove_qualifiers_t<TDst>>
using is_narrowed = integral_constant<bool,
    !is_class<TSrc>::value && !is_class<U>::value && !is_same<TSrc, U>::value
>;

template<class, class...> struct is_array : false_type { };
template<class T, class... Ts> struct is_array<T[], Ts...> : true_type { };

template<class T, class = decltype(sizeof(T))>
true_type is_complete_impl(int);

template<class T>
false_type is_complete_impl(...);

template<class T>
struct is_complete : decltype(is_complete_impl<T>(0)) { };

template<class, class...>
struct is_unique_impl;

template<class...>
struct not_unique : false_type {
    using type = not_unique;
};

template<>
struct not_unique<> : true_type {
    using type = not_unique;
};

template<class T>
struct is_unique_impl<T> : not_unique<> { };

template<class T1, class T2, class... Ts>
struct is_unique_impl<T1, T2, Ts...>
    : conditional_t<
          is_base_of<type<T2>, T1>::value
        , not_unique<T2>
        , is_unique_impl<inherit<T1, type<T2>>, Ts...>
      >
{ };

template<class... Ts>
using is_unique = is_unique_impl<none_type, Ts...>;

BOOST_DI_HAS_METHOD(is_callable_with, operator());

struct callable_base_impl { void operator()(...) { } };

template<class T>
struct callable_base
    : callable_base_impl, aux::conditional_t<aux::is_class<T>::value, T, aux::none_type>
{ };

template<typename T>
aux::false_type is_callable_impl(T*, aux::non_type<void(callable_base_impl::*)(...), &T::operator()>* = 0);
aux::true_type is_callable_impl(...);

template<class T>
struct is_callable : decltype(is_callable_impl((callable_base<T>*)0)) { };

template<class>
struct function_traits;

template<class R, class... TArgs>
struct function_traits<R(*)(TArgs...)> {
    using result_type = R;
    using args = type_list<TArgs...>;
};

template<class R, class... TArgs>
struct function_traits<R(TArgs...)> {
    using result_type = R;
    using args = type_list<TArgs...>;
};

template<class R, class T, class... TArgs>
struct function_traits<R(T::*)(TArgs...)> {
    using result_type = R;
    using args = type_list<TArgs...>;
};

template<class R, class T, class... TArgs>
struct function_traits<R(T::*)(TArgs...) const> {
    using result_type = R;
    using args = type_list<TArgs...>;
};

template<class T>
using function_traits_t = typename function_traits<T>::args;

}}}} // boost::di::v1::aux

#endif

