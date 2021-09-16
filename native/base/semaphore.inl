//
// Created by TBD on 1/18/21.
//

#ifndef XLIVE_SEMAPHORE_INL
#define XLIVE_SEMAPHORE_INL

namespace xlab {

template<typename D>
bool Semaphore::TimedWait(const D &duration) {
    auto nano = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
    if (nano < std::chrono::nanoseconds::zero()) {
        return false;
    }
    std::unique_lock<std::mutex> lock{mutex};
    if (curCount > 0) {
        --curCount;
        return true;
    }

    condVar.wait_for(lock, nano);
    if (curCount > 0) {
        --curCount;
        return true;
    }

    return false;
}

}

#endif //XLIVE_SEMAPHORE_INL
