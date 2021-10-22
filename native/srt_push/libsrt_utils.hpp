#pragma once

#include <srt/srt.h>

#include <climits>

#include "base/time.hpp"
#include "base/xlog.hpp"
#include "libsrt_param.hpp"

namespace xlab {
static inline int libsrt_neterrno() {
    int os_errno;
    int err = srt_getlasterror(&os_errno);
    if (err == SRT_EASYNCRCV || err == SRT_EASYNCSND) {
        return -(EAGAIN);
    }

    xlogd("{}", srt_getlasterror_str());

    return os_errno ? -(os_errno) : INT_MIN;
}

static inline int libsrt_setsockopt(SRTSOCKET fd, SRT_SOCKOPT optname, const char *optnamestr, const void *optval, int optlen) {
    if (srt_setsockopt(fd, 0, optname, optval, optlen) < 0) {
        xloge("failed to set option {} on socket: {}", optnamestr, srt_getlasterror_str());
        return -(EIO);
    }

    return 0;
}

static inline int libsrt_getsockopt(SRTSOCKET fd, SRT_SOCKOPT optname, const char *optnamestr, void *optval, int *optlen) {
    if (srt_getsockopt(fd, 0, optname, optval, optlen) < 0) {
        xloge("failed to get option {} on socket: {}", optnamestr, srt_getlasterror_str());
        return -(EIO);
    }

    return 0;
}

/* - The "POST" options can be altered any time on a connected socket.
     They MAY have also some meaning when set prior to connecting; such
     option is SRTO_RCVSYN, which makes connect/accept call asynchronous.
     Because of that this option is treated special way in this app. */
static inline int libsrt_set_options_post(SRTSOCKET fd, int64_t inputbw, int oheadbw) {
    if ((inputbw >= 0 && libsrt_setsockopt(fd, SRTO_INPUTBW, "SRTO_INPUTBW", &inputbw, sizeof(inputbw)) < 0) ||
        (oheadbw >= 0 && libsrt_setsockopt(fd, SRTO_OHEADBW, "SRTO_OHEADBW", &oheadbw, sizeof(oheadbw)) < 0)) {
        return -(EIO);
    }

    return 0;
}

/* - The "PRE" options must be set prior to connecting and can't be altered
     on a connected socket, however if set on a listening socket, they are
     derived by accept-ed socket. */
static inline int libsrt_set_options_pre(SRTSOCKET fd, SRTParam *par, SRTIOFlag flag) {
    const SRTParam *s = par;

    int yes = 1;
    int latency = s->latency / 1000;
    int rcvlatency = s->rcvlatency / 1000;
    int peerlatency = s->peerlatency / 1000;
    int connect_timeout = s->connect_timeout;

    if ((s->mode == SRTMode::RENDEZVOUS && libsrt_setsockopt(fd, SRTO_RENDEZVOUS, "SRTO_RENDEZVOUS", &yes, sizeof(yes)) < 0) ||
        (s->transtype != SRTT_INVALID && libsrt_setsockopt(fd, SRTO_TRANSTYPE, "SRTO_TRANSTYPE", &s->transtype, sizeof(s->transtype)) < 0) ||
        (s->maxbw >= 0 && libsrt_setsockopt(fd, SRTO_MAXBW, "SRTO_MAXBW", &s->maxbw, sizeof(s->maxbw)) < 0) ||
        (s->pbkeylen >= 0 && libsrt_setsockopt(fd, SRTO_PBKEYLEN, "SRTO_PBKEYLEN", &s->pbkeylen, sizeof(s->pbkeylen)) < 0) ||
        (s->passphrase && libsrt_setsockopt(fd, SRTO_PASSPHRASE, "SRTO_PASSPHRASE", s->passphrase, strlen(s->passphrase)) < 0) ||
#if SRT_VERSION_VALUE >= 0x010302
#if SRT_VERSION_VALUE >= 0x010401
        (s->enforced_encryption >= 0 && libsrt_setsockopt(fd, SRTO_ENFORCEDENCRYPTION, "SRTO_ENFORCEDENCRYPTION", &s->enforced_encryption, sizeof(s->enforced_encryption)) < 0) ||
#else
        /* SRTO_STRICTENC == SRTO_ENFORCEDENCRYPTION (53), but for compatibility, we used SRTO_STRICTENC */
        (s->enforced_encryption >= 0 && libsrt_setsockopt(fd, SRTO_STRICTENC, "SRTO_STRICTENC", &s->enforced_encryption, sizeof(s->enforced_encryption)) < 0) ||
#endif
        (s->kmrefreshrate >= 0 && libsrt_setsockopt(fd, SRTO_KMREFRESHRATE, "SRTO_KMREFRESHRATE", &s->kmrefreshrate, sizeof(s->kmrefreshrate)) < 0) ||
        (s->kmpreannounce >= 0 && libsrt_setsockopt(fd, SRTO_KMPREANNOUNCE, "SRTO_KMPREANNOUNCE", &s->kmpreannounce, sizeof(s->kmpreannounce)) < 0) ||
#endif
        (s->mss >= 0 && libsrt_setsockopt(fd, SRTO_MSS, "SRTO_MSS", &s->mss, sizeof(s->mss)) < 0) ||
        (s->ffs >= 0 && libsrt_setsockopt(fd, SRTO_FC, "SRTO_FC", &s->ffs, sizeof(s->ffs)) < 0) ||
        (s->ipttl >= 0 && libsrt_setsockopt(fd, SRTO_IPTTL, "SRTO_IPTTL", &s->ipttl, sizeof(s->ipttl)) < 0) ||
        (s->iptos >= 0 && libsrt_setsockopt(fd, SRTO_IPTOS, "SRTO_IPTOS", &s->iptos, sizeof(s->iptos)) < 0) ||
        (s->latency >= 0 && libsrt_setsockopt(fd, SRTO_LATENCY, "SRTO_LATENCY", &latency, sizeof(latency)) < 0) ||
        (s->rcvlatency >= 0 && libsrt_setsockopt(fd, SRTO_RCVLATENCY, "SRTO_RCVLATENCY", &rcvlatency, sizeof(rcvlatency)) < 0) ||
        (s->peerlatency >= 0 && libsrt_setsockopt(fd, SRTO_PEERLATENCY, "SRTO_PEERLATENCY", &peerlatency, sizeof(peerlatency)) < 0) ||
        (s->tlpktdrop >= 0 && libsrt_setsockopt(fd, SRTO_TLPKTDROP, "SRTO_TLPKTDROP", &s->tlpktdrop, sizeof(s->tlpktdrop)) < 0) ||
        (s->nakreport >= 0 && libsrt_setsockopt(fd, SRTO_NAKREPORT, "SRTO_NAKREPORT", &s->nakreport, sizeof(s->nakreport)) < 0) ||
        (connect_timeout >= 0 && libsrt_setsockopt(fd, SRTO_CONNTIMEO, "SRTO_CONNTIMEO", &connect_timeout, sizeof(connect_timeout)) < 0) ||
        (s->sndbuf >= 0 && libsrt_setsockopt(fd, SRTO_SNDBUF, "SRTO_SNDBUF", &s->sndbuf, sizeof(s->sndbuf)) < 0) ||
        (s->rcvbuf >= 0 && libsrt_setsockopt(fd, SRTO_RCVBUF, "SRTO_RCVBUF", &s->rcvbuf, sizeof(s->rcvbuf)) < 0) ||
        (s->lossmaxttl >= 0 && libsrt_setsockopt(fd, SRTO_LOSSMAXTTL, "SRTO_LOSSMAXTTL", &s->lossmaxttl, sizeof(s->lossmaxttl)) < 0) ||
        (s->minversion >= 0 && libsrt_setsockopt(fd, SRTO_MINVERSION, "SRTO_MINVERSION", &s->minversion, sizeof(s->minversion)) < 0) ||
        (s->streamid && libsrt_setsockopt(fd, SRTO_STREAMID, "SRTO_STREAMID", s->streamid, strlen(s->streamid)) < 0) ||
#if SRT_VERSION_VALUE >= 0x010401
        (s->smoother && libsrt_setsockopt(fd, SRTO_CONGESTION, "SRTO_CONGESTION", s->smoother, strlen(s->smoother)) < 0) ||
#else
        (s->smoother && libsrt_setsockopt(fd, SRTO_SMOOTHER, "SRTO_SMOOTHER", s->smoother, strlen(s->smoother)) < 0) ||
#endif
        (s->messageapi >= 0 && libsrt_setsockopt(fd, SRTO_MESSAGEAPI, "SRTO_MESSAGEAPI", &s->messageapi, sizeof(s->messageapi)) < 0) ||
        (s->payload_size >= 0 && libsrt_setsockopt(fd, SRTO_PAYLOADSIZE, "SRTO_PAYLOADSIZE", &s->payload_size, sizeof(s->payload_size)) < 0) ||
        (((int)(flag) & (int)(SRTIOFlag::WRITE)) && libsrt_setsockopt(fd, SRTO_SENDER, "SRTO_SENDER", &yes, sizeof(yes)) < 0) ||
        (s->tsbpd >= 0 && libsrt_setsockopt(fd, SRTO_TSBPDMODE, "SRTO_TSBPDMODE", &s->tsbpd, sizeof(s->tsbpd)) < 0)) {
        return -(EIO);
    }

    if (s->linger >= 0) {
        struct linger lin;
        lin.l_linger = s->linger;
        lin.l_onoff = lin.l_linger > 0 ? 1 : 0;
        if (libsrt_setsockopt(fd, SRTO_LINGER, "SRTO_LINGER", &lin, sizeof(lin)) < 0)
            return -(EIO);
    }
    return 0;
}

static inline int libsrt_socket_nonblock(SRTSOCKET fd, int enable) {
    int ret, blocking = enable ? 0 : 1;
    /* Setting SRTO_{SND,RCV}SYN options to 1 enable blocking mode, setting them to 0 enable non-blocking mode. */
    ret = srt_setsockopt(fd, 0, SRTO_SNDSYN, &blocking, sizeof(blocking));
    if (ret < 0) {
        return ret;
    }
    return srt_setsockopt(fd, 0, SRTO_RCVSYN, &blocking, sizeof(blocking));
}

static inline int libsrt_epoll_create(SRTSOCKET fd, int write) {
    int modes = SRT_EPOLL_ERR | (write ? SRT_EPOLL_OUT : SRT_EPOLL_IN);
    int eid = srt_epoll_create();
    if (eid < 0)
        return libsrt_neterrno();
    if (srt_epoll_add_usock(eid, fd, &modes) < 0) {
        srt_epoll_release(eid);
        return libsrt_neterrno();
    }
    return eid;
}

static inline int libsrt_network_wait_fd(int eid, int write) {
    int ret, len = 1, errlen = 1;
    SRTSOCKET ready[1];
    SRTSOCKET error[1];

    const int POLLING_TIME = 100;  /// Time in milliseconds between interrupt check
    if (write) {
        ret = srt_epoll_wait(eid, error, &errlen, ready, &len, POLLING_TIME, 0, 0, 0, 0);
    } else {
        ret = srt_epoll_wait(eid, ready, &len, error, &errlen, POLLING_TIME, 0, 0, 0, 0);
    }
    if (ret < 0) {
        if (srt_getlasterror(NULL) == SRT_ETIMEOUT)
            ret = -(EAGAIN);
        else
            ret = libsrt_neterrno();
    } else {
        ret = errlen ? -(EIO) : 0;
    }

    return ret;
}

/* TODO de-duplicate code from ff_network_wait_fd_timeout() */

static inline int libsrt_network_wait_fd_timeout(int eid, int write, int64_t timeout, bool *int_quit) {
    using namespace base::Time;
    const auto wait_start = nowclock::point();

    while (true) {
        if (*int_quit) {
            return INT_MIN;
        }

        const auto ret = libsrt_network_wait_fd(eid, write);
        if (ret != -(EAGAIN)) {
            return ret;
        }

        if (timeout > 0) {
            if (nowclock::point() - wait_start >= us(timeout)) {
                return -ETIMEDOUT;
            }
        }
    }
}

static inline int libsrt_listen(int eid, SRTSOCKET fd, const struct sockaddr *addr, socklen_t addrlen, int64_t timeout, bool *int_quit) {
    int ret;
    int reuse = 1;
    /* Max streamid length plus an extra space for the terminating null character */
    char streamid[513];
    int streamid_len = sizeof(streamid);
    if (srt_setsockopt(fd, SOL_SOCKET, SRTO_REUSEADDR, &reuse, sizeof(reuse))) {
        xlogw("setsockopt(SRTO_REUSEADDR) failed");
    }

    if (srt_bind(fd, addr, addrlen)) {
        return libsrt_neterrno();
    }

    if (srt_listen(fd, 1)) {
        return libsrt_neterrno();
    }

    ret = libsrt_network_wait_fd_timeout(eid, 1, timeout, int_quit);
    if (ret < 0) {
        return ret;
    }

    auto sock = srt_accept(fd, nullptr, nullptr);
    if (sock < 0) {
        return libsrt_neterrno();
    }

    if (libsrt_socket_nonblock(sock, 1) < 0) {
        xlogd("libsrt_socket_nonblock failed");
    }

    if (!libsrt_getsockopt(sock, SRTO_STREAMID, "SRTO_STREAMID", streamid, &streamid_len)) {
        /* Note: returned streamid_len doesn't count the terminating null character */
        xlogt("accept streamid [{}], length {}", streamid, streamid_len);
    }

    return ret;
}

static inline int libsrt_listen_connect(int eid, int fd, const struct sockaddr *addr, socklen_t addrlen, int64_t timeout, int will_try_next, bool *int_quit) {
    if (srt_connect(fd, addr, addrlen) < 0) {
        return libsrt_neterrno();
    }

    auto ret = libsrt_network_wait_fd_timeout(eid, 1, timeout, int_quit);
    if (ret < 0) {
        if (will_try_next) {
            xlogd("Connection to failed {},trying next address", ret);
        } else {
            xlogd("Connection to failed {}", ret);
        }
    }

    return ret;
}

}  // namespace xlab