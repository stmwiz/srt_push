#include "srt_push.hpp"

#include "srt_push/libsrt_param.hpp"

namespace xlab {

SRTPush::SRTPush(const std::string ip_str, int port) {
    SRTParam param{0};
    param.payload_size = 1316;
    handle_ = std::make_shared<LibSrt>(param, ip_str, port, SRTIOFlag::WRITE);
    success_ = handle_->open();
}

SRTPush::~SRTPush() {
    release();
}

bool SRTPush::success() const {
    return success_;
}

bool SRTPush::write(const std::shared_ptr<base::Buffer> tsbuffer) {
    if (!success()) {
        return false;
    }

    return handle_->write(tsbuffer->start(), tsbuffer->len());
}

void SRTPush::release() {
    handle_->close();
    handle_ = nullptr;
}

}  // namespace xlab