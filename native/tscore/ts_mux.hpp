#pragma once

#include <memory>

#include "tstream.hpp"

namespace xlab
{
    class TSMux
    {
    public:
        explicit TSMux() = delete;

        explicit TSMux(TStream::OutputCallback outCb);

        ~TSMux();

    public:
        bool write(std::shared_ptr<base::Packet> frame);

    private:
        std::shared_ptr<TStream> ts;
    };
}