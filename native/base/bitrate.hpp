//
// Created by x on 2021/8/20.
//

#pragma once

#include <vector>

#include "calc_sample.hpp"

namespace xlab::base {

class Bitrate final : public CalcSample<> {
public:
    bool update(uint64_t &bitrate, uint64_t size = 0, CalcSample::TICK_TYPE tick = Sample::now().tick){
        if (lastTick == CalcSample::TICK_TYPE::zero()) {
            hasUpdateTick = lastTick = tick;
            bitrate = currentBitrate;
            return false;
        }

        auto dSample = updateSample({tick - lastTick, size});
        lastTick = tick;
        if (tick - hasUpdateTick >= CalcSample::TIME_INTERVAL) {
            hasUpdateTick = tick;
            currentBitrate = dSample.size * 8 * CalcSample::TICK_TYPE::period::den / dSample.tick.count();
            bitrate = currentBitrate;
            return true;
        }

        bitrate = currentBitrate;
        return false;
    }

private:
    uint64_t currentBitrate = 0;
};

}
