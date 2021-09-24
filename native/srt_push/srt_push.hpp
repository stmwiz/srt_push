#pragma once

#include <memory>

#include <srt/srt.h>

namespace xlab
{
    class SRTPush
    {
    public:
        enum class Status
        {

        };

    public:
        explicit SRTPush(const std::string uri);

        ~SRTPush();

    private:
    };
}
