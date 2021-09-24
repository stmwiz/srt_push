#pragma once

#include <memory>
#include <string>

#include <srt/srt.h>

#include "libsrt_param.hpp"

namespace xlab
{

    class LibSrt
    {
    public:
        explicit LibSrt(const SRTParam &param, const std::string &ipAddr, int port, SRTIOFlag flag = SRTIOFlag::WRITE);

        ~LibSrt();

    public:
        void read();

        void write();

    private:
        int open();

        void close();

    private:
        int getAddrInfo(addrinfo *&curAi);

        int openSocket(addrinfo *&curAi);

    private:
        SRTIOFlag ioFlag;
        SRTParam par;
        int fd;
        int eid;
        std::string ip;
        int port;
    };

}