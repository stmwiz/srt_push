//
// Created by x on 2021/8/20.
//
#pragma once

#include <vector>

#include "calc_sample.hpp"

namespace xlab::base {

class FPS final : public CalcSample<> {
public:
    bool update(float &fps, CalcSample::TICK_TYPE tick = Sample::now().tick){
        if (lastTick == CalcSample::TICK_TYPE::zero()) {
            hasUpdateTick = lastTick = tick;
            fps = currentFPS;
            return false;
        }

        auto dSample = updateSample({tick - lastTick});
        lastTick = tick;
        if (tick - hasUpdateTick >= CalcSample::TIME_INTERVAL) {
            hasUpdateTick = tick;
            currentFPS = float(double(CalcSample::TICK_TYPE::period::den) / double(dSample.tick.count()));
            fps = currentFPS;
            return true;
        }

        fps = currentFPS;
        return false;
    }

private:
    float currentFPS = 0;
};


}

