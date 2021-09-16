#pragma once

#include <string>
namespace xlab
{
    class IPush
    {
    public:
        IPush(){};
        virtual ~IPush(){};

        virtual void write(const char *url,
                           const uint8_t *head,
                           const int headLen,
                           const uint8_t *body,
                           const int bodyLen,
                           const int64_t dts,
                           const bool isVideo,
                           const bool isKeyFrame) = 0;
    };
}
