//
// Created by x on 2021/8/23.
//

#pragma once

#include "time.hpp"

namespace xlab ::base {

///TIME_TYPE 时间类型
///TIME_NUM 样本计算间隔时间
///NB_SAMPLE 样本缓存个数

template<class TIME_TYPE = us, uint64_t TIME_NUM = us::period::den, uint32_t NB_SAMPLE = 100>
class CalcSample {
public:
    using TICK_TYPE = TIME_TYPE;
    static constexpr auto TIME_INTERVAL = TICK_TYPE(TIME_NUM);

    struct Sample {
        static inline Sample zero() { return {}; }

        static inline Sample now() { return {Time::nowclock::duration<TICK_TYPE>(), 0}; };

        TICK_TYPE tick = TICK_TYPE::zero();
        uint64_t size = 0;
    };

protected:
    virtual Sample updateSample(Sample sample) {
        if (samples.size() == NB_SAMPLE) {
            sampleSum.tick -= sample.tick;
            sampleSum.size -= sample.size;
            samples[index] = sample;
        } else {
            samples.push_back(sample);
        }

        sampleSum.tick += sample.tick;
        sampleSum.size += sample.size;
        index = (index + 1) % NB_SAMPLE;

        return {sampleSum.tick / samples.size(), sampleSum.size / samples.size()};
    }

protected:
    uint32_t index = 0;
    TICK_TYPE lastTick = TICK_TYPE::zero();
    TICK_TYPE hasUpdateTick = TICK_TYPE::zero();
    Sample sampleSum = Sample::zero();
    std::vector<Sample> samples;

};
}
