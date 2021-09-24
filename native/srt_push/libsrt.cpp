#include "libsrt.hpp"
#include "base/xlog.hpp"
#include "libsrt_utils.hpp"

namespace xlab
{
    LibSrt::LibSrt(const SRTParam &param, const std::string &ipAddr, int port, SRTIOFlag flag)
        : par(param), ip(ipAddr), port(port), ioFlag(flag)
    {
    }

    LibSrt::~LibSrt()
    {
    }

    int LibSrt::open()
    {
    }

    void LibSrt::close()
    {
    }

    int LibSrt::getAddrInfo(addrinfo *&curAi)
    {
        struct addrinfo *ai = nullptr;
        struct addrinfo hints = {0};

        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;
        const auto portstr = std::to_string(port);
        if (par.mode == SRTMode::LISTENER)
        {
            hints.ai_flags |= AI_PASSIVE;
        }

        auto ret = getaddrinfo(ip.c_str(), portstr.c_str(), &hints, &ai);
        if (ret != 0)
        {
            xloge("Failed to resolve hostname {}:{}", ip, gai_strerror(ret));
            return -EIO;
        }

        curAi = ai;

        return 0;
    }

    int LibSrt::openSocket(addrinfo *&curAi)
    {
        int ret = -1;
        bool isRestart;
        auto s = &par;
        do
        {
            isRestart = false;
            fd = srt_socket(curAi->ai_family, curAi->ai_socktype, 0);
            if (fd < 0)
            {
                ret = libsrt_neterrno();
                open_socket_fail_restart(curAi, fd, ret);
                isRestart = true;
                continue;
            }

            if ((ret = libsrt_set_options_pre(fd, s, ioFlag)) < 0)
            {
                isRestart = true;
            }

        } while (isRestart);

        /* Set the socket's send or receive buffer sizes, if specified.
       If unspecified or setting fails, system default is used. */
        if (s->recv_buffer_size > 0)
        {
            srt_setsockopt(fd, SOL_SOCKET, SRTO_UDP_RCVBUF, &s->recv_buffer_size, sizeof(s->recv_buffer_size));
        }

        if (s->send_buffer_size > 0)
        {
            srt_setsockopt(fd, SOL_SOCKET, SRTO_UDP_SNDBUF, &s->send_buffer_size, sizeof(s->send_buffer_size));
        }

        if (libsrt_socket_nonblock(fd, 1) < 0)
        {
            xlogd("libsrt_socket_nonblock failed");
        }

        int write_eid;
        ret = write_eid = libsrt_epoll_create(fd, 1);

        return 0;
    }

}