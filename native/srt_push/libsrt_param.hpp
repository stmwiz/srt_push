#pragma once

#include <srt/srt.h>

namespace xlab
{

    enum class SRTMode
    {
        CALLER = 0,
        LISTENER = 1,
        RENDEZVOUS = 2
    };

    enum class SRTIOFlag
    {
        READ = 1,                    /**< read-only */
        WRITE = 2,                   /**< write-only */
        READ_WRITE = (READ | WRITE), /**< read-write pseudo flag */
    };

    struct SRTParam
    {
        int64_t rw_timeout;
        int64_t listen_timeout;
        int recv_buffer_size;
        int send_buffer_size;

        int64_t maxbw;
        int pbkeylen;
        char *passphrase;
#if SRT_VERSION_VALUE >= 0x010302
        int enforced_encryption;
        int kmrefreshrate;
        int kmpreannounce;
#endif
        int mss;
        int ffs;
        int ipttl;
        int iptos;
        int64_t inputbw;
        int oheadbw;
        int64_t latency;
        int tlpktdrop;
        int nakreport;
        int64_t connect_timeout;
        int payload_size;
        int64_t rcvlatency;
        int64_t peerlatency;
        enum SRTMode mode;
        int sndbuf;
        int rcvbuf;
        int lossmaxttl;
        int minversion;
        char *streamid;
        char *smoother;
        int messageapi;
        SRT_TRANSTYPE transtype;
        int linger;
        int tsbpd;
    };

}