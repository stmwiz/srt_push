//
// Created by x on 2021/9/7.
//

#pragma once

#include "time.hpp"
#include "semaphore.hpp"

namespace xlab::base
{

    struct Counting
    {
        using type = int64_t;

        explicit Counting(type max) : maxVal(max) {}

        ~Counting() { reset(); }

        void setMaxVal(type max)
        {
            maxVal = max;
        }

        type getMaxVal() const
        {
            return maxVal;
        }

        static type zero()
        {
            return 0;
        }

        void selfInc()
        {
            ++val;
        }

        bool done() const
        {
            return val >= maxVal;
        }

        void reset()
        {
            val = zero();
        }

        type getVal() const
        {
            return val;
        }

    private:
        volatile type val = zero();
        type maxVal = zero();
    };

    template <typename T = us>
    struct Ticking
    {
        using type = T;

        explicit Ticking(type max) : maxVal(max) {}

        ~Ticking() { reset(); };

        void setMaxVal(type max)
        {
            maxVal = max;
        }

        type getMaxVal() const
        {
            return maxVal;
        }

        static type zero()
        {
            return type::zero();
        }

        void selfInc()
        {
            if (last_val == type::zero())
            {
                last_val = Time::nowclock::duration<type>();
            }

            val = Time::nowclock::duration<type>() - last_val;
        }

        bool done() const
        {
            return val >= maxVal;
        }

        void reset()
        {
            last_val = zero();
            val = zero();
        }

        type getVal() const
        {
            return val;
        }

    private:
        type val = zero();
        type last_val = zero();
        type maxVal = zero();
    };

    template <typename T = us>
    struct Timing
    {
        using type = T;

        explicit Timing(type max) : maxVal(max) {}

        ~Timing() { reset(); };

        type getMaxVal() const
        {
            return maxVal;
        }

        static type zero()
        {
            return type::zero();
        }

        bool done() const
        {
            return val >= maxVal;
        }

        type wait()
        {
            val = zero();
            val = Time::Task::duration([&]
                                       { nap.TimedWait(maxVal); });
            return val;
        }

        void reset()
        {
            nap.Post();
            val = zero();
        }

    private:
        type val = zero();
        type maxVal = zero();
        Semaphore nap;
    };

}
