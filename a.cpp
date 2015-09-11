#include <boost/di.hpp>
#include <cassert>
#include <tuple>

namespace di = boost::di;

struct c {
   //template<class... T>
    //c(int, T... args) {
        //assert(sizeof...(T) == 2);
        //std::tuple<T...> t{args...};
        //assert(std::get<0>(t) == 42);
        //assert(std::get<1>(t) == 87.0);
    //}

    BOOST_DI_INJECT((template<class T>) c, int, T...) {
        //assert(sizeof...(T) == 2);
        //std::tuple<T...> t{args...};
        //assert(std::get<0>(t) == 42);
        //assert(std::get<1>(t) == 87.0);
    }
};

auto n1 = []{};
auto n2 = []{};

/*namespace boost { namespace di { inline namespace v1 {*/

//template<>
//struct ctor_traits<c> {
    ////using boost_di_inject__ = di::inject<int, di::detail::named_type<std::integral_constant<int, 0>, _>, di::detail::named_type<std::integral_constant<int, 1>, _>>;
    ////using boost_di_inject__ = di::inject<int, _(...)>;
//};

/*}}} // boost::di::v1*/

int main() {
    auto injector = di::make_injector(
            //di::make_injector(
                di::core::dependency<di::scopes::deduce, di::_(...), di::aux::type_list<di::detail::named_type<std::integral_constant<int, 0>, di::_>, di::detail::named_type<std::integral_constant<int, 1>, di::_>>>()
              , di::bind<di::_>.named(std::integral_constant<int, 0>{}).to(42)
              , di::bind<di::_>.named(std::integral_constant<int, 1>{}).to(87.0)
            //)
    );
    injector.create<c>();
}
