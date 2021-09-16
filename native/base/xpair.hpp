//
// Created by x on 2021/9/1.
//

#pragma once

#include <ratio>

#include "utils.hpp"

namespace xlab::base {
template<intmax_t _First, intmax_t _Second = 1>
struct xpair {
    static constexpr decltype(_First) first = _First;
    static constexpr decltype(_Second) second = _Second;
    static constexpr decltype(_First) value = _Second == 0 ? 0 : _First / _Second;

    template<typename T = decltype(_First)>
    static constexpr T First = (T) (_First);

    template<typename T = decltype(_Second)>
    static constexpr T Second = (T) (_Second);

    using swap = xpair<_Second, _First>;
    using ratio = std::ratio<_First, _Second>;

    template<typename T = decltype(_First)>
    static constexpr T Value = _Second == 0 ? (T) (0) : (T) (_First) / (T) (_Second);

    template<typename T=decltype(_First), typename T1=decltype(_Second)>
    static constexpr std::pair<T, T1> std_pair{(T) (_First), (T1) (_Second)};

    template<typename T = decltype(_First)>
    static constexpr T max_value = _First > _Second ? (T) (_First) : (T) (_Second);

    template<typename T = decltype(_First)>
    static constexpr T min_value = _First < _Second ? (T) (_First) : (T) (_Second);

    template<typename T = decltype(_First)>
    static constexpr T mul_value = (T) (_First) * (T) (_Second);

    template<typename T = decltype(_First)>
    static constexpr T add_value = (T) (_First) + (T) (_Second);

    template<typename T = decltype(_First)>
    static constexpr T sub_value = (T) (_First) - (T) (_Second);

    static constexpr decltype(_First) mod_value = _Second == 0 ? 0 : _First % _Second;

    static constexpr decltype(_First) and_value = _First & _Second;

    static constexpr decltype(_First) or_value = _First | _Second;

    static constexpr bool is_equal = _First == _Second;

    static constexpr bool logic_and_value = (!!_First) && (!!_Second);

    static constexpr bool logic_or_value = (!!_First) || (!!_Second);

    static constexpr decltype(_First) move_left_value = _First << _Second;

    static constexpr decltype(_First) move_right_value = _First >> _Second;
};

}
