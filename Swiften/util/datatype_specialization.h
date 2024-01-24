#pragma once

#include "../config/config.h"

#include <type_traits>
#include <stdint.h>

namespace Pagoda {
namespace Swiften {
namespace Util {

template<class d, class D>
struct size_less {
    SWIFTEN_SC bool value = sizeof(d) < sizeof(D);
}; //struct int_size_lesss

template<class d, class D>
SWIFTEN_SC bool size_less_v = size_less<d, D>::value;


template<class d, class D>
struct size_leq {
    SWIFTEN_SC bool value = sizeof(d) <= sizeof(D);
}; //struct int_size_leq

template<class d, class D>
SWIFTEN_SC bool size_leq_v = size_leq<d, D>::value;


template<class d, class D>
struct size_greater {
    SWIFTEN_SC bool value = sizeof(d) > sizeof(D);
}; //struct int_size_greater

template<class d, class D>
SWIFTEN_SC bool size_greater_v = size_greater<d, D>::value;


template<class d, class D>
struct size_greateq {
    SWIFTEN_SC bool value = sizeof(d) >= sizeof(D);
}; //struct int_size_greateq

template<class d, class D>
SWIFTEN_SC bool size_greateq_v = size_greateq<d, D>::value;



template<uint64_t D>
struct smallest_datatype 
{
    using value = typename std::conditional<(D + 1 < 0xFF), 
                                                uint8_t,
                                                typename std::conditional<(D + 1 < 0xFFFF),
                                                uint16_t,
                                                typename std::conditional<(D + 1 < 0xFFFFFFFF),
                                                uint32_t,
                                                uint64_t>::type >::type >::type;
};

template<uint64_t D>
using smallest_datatype_v = typename smallest_datatype<D>::value;

template<uint64_t D>
auto smallest_cast = static_cast<smallest_datatype_v<D>>(D);

template<typename T>
auto upper_bound = T(~0);

template<bool...> struct bool_pack;

template<bool... bools> 
using all_true_bools = std::is_same<bool_pack<bools..., true>, bool_pack<true, bools...>>;


template<typename T, typename... Ts>
using all_same = all_true_bools<std::is_same<T, Ts>::value...>;


} //namespace util
} //namespace Swiften
} //namespace Pagoda