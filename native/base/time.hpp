//
// Created by x on 2021/8/18.
//

#pragma once

#include <chrono>
#include <functional>

namespace xlab::base
{
    using namespace std::chrono;
    using namespace std::chrono_literals;
    namespace Time
    {
        using h = hours;
        using min = minutes;
        using s = seconds;
        using ms = milliseconds;
        using us = microseconds;
        using ns = nanoseconds;

        namespace nowclock
        {
            using tp = steady_clock::time_point;
            using dur = steady_clock::duration;

            static tp point()
            {
                return steady_clock::now();
            }

            template <typename T>
            static T duration()
            {
                return duration_cast<T>(point().time_since_epoch());
            }
        };

    } // namespace Time

    using h = Time::h;
    using min = Time::min;
    using s = Time::s;
    using ms = Time::ms;
    using us = Time::us;
    using ns = Time::ns;

    using tp = Time::nowclock::tp;
    using dur = Time::nowclock::dur;

}
