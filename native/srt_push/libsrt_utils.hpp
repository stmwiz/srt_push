#pragma once

#include <srt/srt.h>

#include "libsrt_param.hpp"
#include "base/xlog.hpp"

namespace xlab
{
    static inline int libsrt_neterrno()
    {
        int os_errno;
        int err = srt_getlasterror(&os_errno);
        if (err == SRT_EASYNCRCV || err == SRT_EASYNCSND)
        {
            return -(EAGAIN);
        }
        xloge("%s", srt_getlasterror_str());

        return os_errno ? -(os_errno) : -INT_MIN;
    }

    static inline void open_socket_fail_restart(addrinfo *curAi, int &Fd, int &ret)
    {
        if (curAi->ai_next)
        {
            /* Retry with the next sockaddr */
            curAi = curAi->ai_next;
            if (Fd >= 0)
            {
                srt_close(Fd);
            }
            ret = 0;
        }
    }

    static inline void open_socket_fail_exit(addrinfo *&ai, int &Fd)
    {
        if (Fd >= 0)
        {
            srt_close(Fd);
        }

        freeaddrinfo(ai);
    }

    static inline int libsrt_setsockopt(int fd, SRT_SOCKOPT optname, const char *optnamestr, const void *optval, int optlen)
    {
        if (srt_setsockopt(fd, 0, optname, optval, optlen) < 0)
        {
            xloge("failed to set option {} on socket: {}", optnamestr, srt_getlasterror_str());
            return -(EIO);
        }

        return 0;
    }

    static inline int libsrt_getsockopt(int fd, SRT_SOCKOPT optname, const char *optnamestr, void *optval, int *optlen)
    {
        if (srt_getsockopt(fd, 0, optname, optval, optlen) < 0)
        {
            xloge("failed to get option {} on socket: {}", optnamestr, srt_getlasterror_str());
            return -(EIO);
        }

        return 0;
    }

    /* - The "PRE" options must be set prior to connecting and can't be altered
     on a connected socket, however if set on a listening socket, they are
     derived by accept-ed socket. */
    static inline int libsrt_set_options_pre(int fd, SRTParam *par, SRTIOFlag flag)
    {
        SRTParam *s = par;
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
            (s->tsbpd >= 0 && libsrt_setsockopt(fd, SRTO_TSBPDMODE, "SRTO_TSBPDMODE", &s->tsbpd, sizeof(s->tsbpd)) < 0))
        {
            return -(EIO);
        }

        if (s->linger >= 0)
        {
            struct linger lin;
            lin.l_linger = s->linger;
            lin.l_onoff = lin.l_linger > 0 ? 1 : 0;
            if (libsrt_setsockopt(fd, SRTO_LINGER, "SRTO_LINGER", &lin, sizeof(lin)) < 0)
                return -(EIO);
        }
        return 0;
    }

    static inline int libsrt_socket_nonblock(int socket, int enable)
    {
        int ret, blocking = enable ? 0 : 1;
        /* Setting SRTO_{SND,RCV}SYN options to 1 enable blocking mode, setting them to 0 enable non-blocking mode. */
        ret = srt_setsockopt(socket, 0, SRTO_SNDSYN, &blocking, sizeof(blocking));
        if (ret < 0)
        {
            return ret;
        }
        return srt_setsockopt(socket, 0, SRTO_RCVSYN, &blocking, sizeof(blocking));
    }

    static inline int libsrt_epoll_create(int fd, int write)
    {
        int modes = SRT_EPOLL_ERR | (write ? SRT_EPOLL_OUT : SRT_EPOLL_IN);
        int eid = srt_epoll_create();
        if (eid < 0)
            return libsrt_neterrno();
        if (srt_epoll_add_usock(eid, fd, &modes) < 0)
        {
            srt_epoll_release(eid);
            return libsrt_neterrno();
        }
        return eid;
    }

}