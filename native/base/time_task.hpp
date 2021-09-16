#pragma once

#include "time.hpp"

namespace xlab::base::Time::Task
{
    static inline auto Loop(dur waitTime, std::function<void(void)> taskFunc)
    {
        const auto start = nowclock::point();
        for (; nowclock::point() - start <= waitTime;)
        {
            if (taskFunc != nullptr)
            {
                taskFunc();
            }
        }

        return duration_cast<decltype(waitTime)>(nowclock::point() - start);
    }

    static inline auto Loop(dur waitTime, std::function<bool(void)> taskFunc)
    {
        const auto start = nowclock::point();
        for (; nowclock::point() - start <= waitTime;)
        {
            if (taskFunc != nullptr && taskFunc())
            {
                break;
            }
        }

        return duration_cast<decltype(waitTime)>(nowclock::point() - start);
    }

    static inline auto duration(std::function<void(void)> taskFunc)
    {
        const auto start = nowclock::point();
        if (taskFunc != nullptr)
        {
            taskFunc();
        }
        return duration_cast<dur>(nowclock::point() - start);
    }
}
