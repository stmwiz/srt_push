#include "srt_push.hpp"

#include "libsrt_param.hpp"

namespace xlab {

SRTPush::SRTPush(const std::string ip_str, int port, std::string streamid) {
    SRTParam param{};
    param.payload_size = 1316;
    param.streamid = streamid;
    param.tlpktdrop = 1;
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
    if (!success() || handle_ == nullptr) {
        return false;
    }

    return handle_->write(tsbuffer->start(), tsbuffer->len());
}

void SRTPush::release() {
    handle_->close();
    handle_ = nullptr;
}

}  // namespace xlab