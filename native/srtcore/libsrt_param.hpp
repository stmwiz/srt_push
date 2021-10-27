#pragma once

#include <srt/srt.h>

namespace xlab {

enum class SRTMode {
    CALLER = 0,
    LISTENER = 1,
    RENDEZVOUS = 2
};

enum class SRTIOFlag {
    READ = 1,                    /**< read-only */
    WRITE = 2,                   /**< write-only */
    READ_WRITE = (READ | WRITE), /**< read-write pseudo flag */
};

struct SRTParam {
    int64_t rw_timeout = -1;
    int64_t listen_timeout = -1;
    int recv_buffer_size = -1;
    int send_buffer_size = -1;

    int64_t maxbw = -1;
    int pbkeylen = -1;
    char *passphrase = nullptr;
#if SRT_VERSION_VALUE >= 0x010302
    int enforced_encryption = -1;
    int kmrefreshrate = -1;
    int kmpreannounce = -1;
    int64_t snddropdelay = -2;
#endif
    int mss = -1;
    int ffs = -1;
    int ipttl = -1;
    int iptos = -1;
    int64_t inputbw = -1;
    int oheadbw = -1;
    int64_t latency = -1;
    int tlpktdrop = -1;
    int nakreport = -1;
    int64_t connect_timeout = -1;
    int payload_size = -1;
    int64_t rcvlatency = -1;
    int64_t peerlatency = -1;
    enum SRTMode mode = SRTMode::CALLER;
    int sndbuf = -1;
    int rcvbuf = -1;
    int lossmaxttl = -1;
    int minversion = -1;
    std::string streamid;
    std::string smoother;
    int messageapi = -1;
    SRT_TRANSTYPE transtype = SRT_TRANSTYPE::SRTT_INVALID;
    int linger = -1;
    int tsbpd = -1;
};

}  // namespace xlab
