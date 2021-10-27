#pragma once

#include <srt/srt.h>

#include <memory>
#include <thread>

#include "base/buffer.hpp"
#include "libsrt.hpp"

namespace xlab {
class SRTPush {
   public:
    explicit SRTPush(const std::string ip_str, int port,const std::string streamid);

    ~SRTPush();

   public:
    bool success() const;

    bool write(const std::shared_ptr<base::Buffer> tsbuffer);

    void release();

   private:
    std::shared_ptr<LibSrt> handle_ = nullptr;
    bool success_ = false;
};
}  // namespace xlab
