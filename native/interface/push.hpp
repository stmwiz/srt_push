#pragma once

#include <string>

#include "base/packet.hpp"

namespace xlab
{
    class IPush
    {
    public:
        IPush(){};
        virtual ~IPush(){};

        virtual void write(const base::Packet &pkt) = 0;
    };
}
