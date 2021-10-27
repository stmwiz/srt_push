#pragma once

#include <srt/srt.h>

#include <atomic>
#include <memory>
#include <string>

#include "libsrt_param.hpp"

namespace xlab {

class LibSrt {
   public:
    explicit LibSrt(const SRTParam &param, const std::string &ipAddr, int port, SRTIOFlag flag);

    ~LibSrt();

   public:
    bool read(uint8_t *buf, int size);

    bool write(const uint8_t *buf, int size);

    void quit();

    bool open();

    void close();

   private:
    SRTParam par;
    SRTIOFlag ioFlag;
    std::string ip;
    int port;

    bool quitIO;
    SRTSOCKET sockFd;
    int eid;
    int maxPacketSize;
};

}  // namespace xlab