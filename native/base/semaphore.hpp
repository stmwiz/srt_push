//
// Created by TBD on 2020/8/28.
//

#ifndef XLIVE_SEMAPHORE_HPP
#define XLIVE_SEMAPHORE_HPP

#include <mutex>
#include <condition_variable>
#include <atomic>

namespace xlab {

class Semaphore {
private:
    Semaphore(const Semaphore &) = delete;

    Semaphore(Semaphore &&) = delete;

    Semaphore &operator=(const Semaphore &) = delete;

    Semaphore &operator=(Semaphore &&) = delete;

public:
    Semaphore(int init_val = 0);

    ~Semaphore();

    void Wait();

    bool TryWait();

    template<typename D>
    bool TimedWait(const D &duration);

    bool TimedWait(const std::chrono::time_point<std::chrono::steady_clock> &abs_time);

    void Post();

private:
    int64_t curCount;
    std::mutex mutex;
    std::condition_variable condVar;
};

}

#include "semaphore.inl"

#endif //XLIVE_SEMAPHORE_HPP


