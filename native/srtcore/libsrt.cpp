#include "libsrt.hpp"

#include "base/auto_guard.hpp"
#include "base/xlog.hpp"
#include "libsrt_utils.hpp"

namespace xlab {
LibSrt::LibSrt(const SRTParam &param, const std::string &ipAddr, int port,
               SRTIOFlag flag)
    : par(param), ip(ipAddr), port(port), ioFlag(flag) {
    srt_startup();
    srt_setloglevel(srt_logging::LogLevel::debug);
}

LibSrt::~LibSrt() {
    // use this function to release the UDT library
    srt_cleanup();
}

bool LibSrt::open() {
    quitIO = false;
    sockFd = -1;
    eid = -1;
    maxPacketSize = 0;

    addrinfo hints{0}, *res = nullptr;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    if (par.mode == SRTMode::LISTENER) {
        hints.ai_flags |= AI_PASSIVE;
    }

    if (0 != getaddrinfo(ip.empty() ? nullptr : ip.c_str(), std::to_string(port).c_str(), &hints, &res)) {
        xloge("illegal port number or port is busy.");
        return false;
    }

    auto_guard_void addrinfoCleanUp(nullptr, [&res] {
        freeaddrinfo(res);
        res = nullptr;
    });

    SRTSOCKET sock = -1;
    do {
        if (sock > 0) {
            srt_close(sock);
            sock = -1;
        }

        sock = srt_create_socket();
        if (sock < 0) {
            break;
        }

        if (libsrt_set_options_pre(sock, &par, ioFlag) < 0) {
            continue;
        }

        if (par.recv_buffer_size > 0) {
            srt_setsockopt(sock, SOL_SOCKET, SRTO_UDP_RCVBUF, &par.recv_buffer_size, sizeof(par.recv_buffer_size));
        }

        if (par.send_buffer_size > 0) {
            srt_setsockopt(sock, SOL_SOCKET, SRTO_UDP_SNDBUF, &par.send_buffer_size, sizeof(par.send_buffer_size));
        }

        if (libsrt_socket_nonblock(sock, 1) < 0) {
            xlogd("libsrt_socket_nonblock failed");
        }

        auto write_eid = libsrt_epoll_create(sock, 1);
        auto_guard_void writeEidCleanUp(nullptr, [&write_eid] {
            if (write_eid > 0) {
                srt_epoll_release(write_eid);
                write_eid = -1;
            }
        });

        if (write_eid < 0) {
            break;
        }

        if (par.mode == SRTMode::LISTENER) {
            // multi-client
            auto listenfd = libsrt_listen(write_eid, sock, res->ai_addr, res->ai_addrlen, par.listen_timeout, &quitIO);
            if (listenfd < 0) {
                break;
            }

            srt_close(sock);
            sock = listenfd;
        } else {
            if (par.mode == SRTMode::RENDEZVOUS) {
                if (srt_bind(sock, res->ai_addr, res->ai_addrlen) != 0) {
                    break;
                }
            }

            const int ret = libsrt_listen_connect(write_eid, sock, res->ai_addr, res->ai_addrlen, par.rw_timeout, !!res->ai_next, &quitIO);
            if (ret < 0) {
                if (ret == INT_MIN) {
                    break;
                }

                continue;
            }

            srt_epoll_release(write_eid);
            write_eid = -1;
        }

        if (libsrt_set_options_post(sock, par.inputbw, par.oheadbw) < 0) {
            continue;
        }

        if ((int)(ioFlag) & (int)(SRTIOFlag::WRITE)) {
            int packet_size = 0;
            int optlen = sizeof(packet_size);
            if (libsrt_getsockopt(sock, SRTO_PAYLOADSIZE, "SRTO_PAYLOADSIZE", &packet_size, &optlen) < 0) {
                break;
            }

            if (packet_size > 0) {
                maxPacketSize = packet_size;
            }
        }

        const int tmp_eid = libsrt_epoll_create(sock, (int)(ioFlag) & (int)(SRTIOFlag::WRITE));
        if (tmp_eid < 0) {
            break;
        }

        eid = tmp_eid;
        sockFd = sock;

        return true;
    } while (res = res->ai_next, res != nullptr);

    if (sock > 0) {
        srt_close(sock);
        sock = 0;
    }

    return false;
}

void LibSrt::close() {
    quitIO = true;

    if (eid > 0) {
        srt_epoll_release(eid);
        eid = -1;
    }

    if (sockFd > 0) {
        srt_close(sockFd);
        sockFd = -1;
    }
}

bool LibSrt::read(uint8_t *buf, int size) {
    int ret = libsrt_network_wait_fd_timeout(eid, 0, par.rw_timeout, &quitIO);
    if (ret != 0) {
        return false;
    }

    ret = srt_recvmsg(sockFd, (char *)(buf), size);
    if (ret < 0) {
        libsrt_neterrno();
        return false;
    }

    return true;
}

bool LibSrt::write(const uint8_t *buf, int size) {
    int ret = libsrt_network_wait_fd_timeout(eid, 0, par.rw_timeout, &quitIO);
    if (ret != 0) {
        return false;
    }

    ret = srt_sendmsg(sockFd, (char *)(buf), size, -1, 1);
    if (ret < 0) {
        libsrt_neterrno();
        return false;
    }

    return true;
}

void LibSrt::quit() {
    quitIO = true;
}

}  // namespace xlab
