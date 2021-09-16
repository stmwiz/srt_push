
#include "semaphore.hpp"

namespace xlab {

Semaphore::Semaphore(int init_val) : curCount(init_val) {

}

Semaphore::~Semaphore() = default;

void Semaphore::Wait() {
    while (true) {
        std::unique_lock<std::mutex> lock{mutex};
        if (curCount > 0) {
            --curCount;
            break;
        }
        condVar.wait(lock);
        if (curCount > 0) {
            --curCount;
            break;
        }
    }
}

bool Semaphore::TryWait() {
    std::unique_lock<std::mutex> lock{mutex};
    if (curCount > 0) {
        --curCount;
        return true;
    }

    condVar.wait_for(lock, std::chrono::nanoseconds(0));
    if (curCount > 0) {
        --curCount;
        return true;
    }

    return false;
}

bool Semaphore::TimedWait(const std::chrono::time_point<std::chrono::steady_clock> &abs_time) {
    std::unique_lock<std::mutex> lock{mutex};
    if (curCount > 0) {
        --curCount;
        return true;
    }

    condVar.wait_until(lock, abs_time);
    if (curCount > 0) {
        --curCount;
        return true;
    }

    return false;
}

void Semaphore::Post() {
    std::lock_guard<std::mutex> lock{mutex};
    ++curCount;
    condVar.notify_one();
}

}

