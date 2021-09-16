//
// Created by x on 2021/9/7.
//

#pragma once

#include <functional>
#include "computing.hpp"

namespace xlab::base {

class XTask {
public:
    enum class Plan {
        CountingAndTicking,
        CountingOrTicking,
    };

public:
    XTask(Counting::type maxCnt = Counting::type(1), Ticking<>::type maxTime = Ticking<>::zero()) : cnt(maxCnt), tick(maxTime) {}

    ~XTask() { reset(); }

    void setMaxRunCount(Counting::type maxCnt) {
        cnt.setMaxVal(maxCnt);
    }

    void setMaxRunTime(Ticking<>::type maxTime) {
        tick.setMaxVal(maxTime);
    }

    Counting::type getMaxRunCount() const {
        return cnt.getMaxVal();
    }

    Ticking<>::type getMaxRunTime() const {
        return tick.getMaxVal();
    }

    Counting::type getCurRunCount() const {
        return cnt.getVal();
    }

    Ticking<>::type getCurRunTime() const {
        return tick.getVal();
    }

    void pauseOn() {
        isPause = true;
    }

    void pauseOff() {
        isPause = false;
    }

    template<Plan PLAN = Plan::CountingAndTicking>
    bool run(const std::function<void(void)> &taskFunc) {
        if (isPause) {
            return false;
        }

        bool done = false;
        if (PLAN == Plan::CountingAndTicking) {
            done = (cnt.done() && tick.done());
        } else if (PLAN == Plan::CountingOrTicking) {
            done = (cnt.done() || tick.done());
        }

        if (done) {
            return false;
        }

        if (!cnt.done()) {
            cnt.selfInc();
        }

        if (!tick.done()) {
            tick.selfInc();
        }

        if (PLAN == Plan::CountingAndTicking) {
            done = (cnt.done() && tick.done());
        } else if (PLAN == Plan::CountingOrTicking) {
            done = (cnt.done() || tick.done());
        }

        if (!done) {
            return false;
        }

        taskFunc();
        return true;
    }

    void reset() {
        cnt.reset();
        tick.reset();
        pauseOff();
    }

private:
    std::atomic<bool> isPause = false;
    Counting cnt{Counting::type(1)};
    Ticking<> tick{Ticking<>::zero()};
};

}
