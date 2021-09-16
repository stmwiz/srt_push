//
// Created by x on 2021/9/9.
//

#pragma once

#include <functional>
#include <thread>

#include "computing.hpp"
#include "xtask.hpp"

namespace xlab::base {

enum class XATaskPlan {
    CountingAndTiming,
    CountingOrTiming,
};

template<XATaskPlan PLAN = XATaskPlan::CountingAndTiming>
class XATask {
public:
    using CallbackType = std::function<void(void)>;

    explicit XATask(const CallbackType &callback, Counting::type max_cntout = Counting::type(1), Timing<>::type max_timeout = Timing<>::zero())
        : taskFunc(callback), counting(max_cntout), timing(max_timeout) {
        th = std::thread(&XATask::run, this);
    }

    ~XATask() {
        quit = true;
        timing.reset();
        counting.reset();
        if (th.joinable()) {
            th.join();
        }
    }

public:
    bool update() {
        bool isUpdate = false;
        if (!timing.done()) {
            timing.reset();
            isUpdate = true;
        }

        if (!counting.done()) {
            counting.selfInc();
            isUpdate = true;
        }

        return isUpdate;
    }

    bool hasTaskRun() const {
        return hasRun;
    }

private:
    void run() {
        while (!timing.done() && !quit) {
            timing.wait();
        }

        bool done = false;
        if (PLAN == XATaskPlan::CountingAndTiming) {
            done = timing.done() && counting.done();
        } else if (PLAN == XATaskPlan::CountingOrTiming) {
            done = timing.done() || counting.done();
        }

        if (done && !quit) {
            runCallback();
            hasRun = true;
        }
    }

    void runCallback() {
        if (taskFunc != nullptr) {
            taskFunc();
        }
    }

private:
    std::atomic<bool> quit = false;
    std::atomic<bool> hasRun = false;
    std::thread th;
    CallbackType taskFunc = nullptr;
    Counting counting{Counting::type(1)};
    Timing<> timing{Timing<>::zero()};

};

}
