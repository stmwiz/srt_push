//
// Created by x on 2021/8/18.
//

#include "network.hpp"

#include <vector>
#include <sstream>
#include <cstring>

#include <sys/socket.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/select.h>
#include <ifaddrs.h>
#include <linux/wireless.h>

#include "system_cmd.hpp"

namespace xlab::base::Network {

int GetAvailableListenPort(std::string clientIP) {
    int tmpPort = 0;

    int port = 0;
    // 1. 创建一个socket
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // 2. 创建一个sockaddr，并将它的端口号设为0
    struct sockaddr_in addrto;
    ::memset(&addrto, 0, sizeof(struct sockaddr_in));
    addrto.sin_family = AF_INET;
    addrto.sin_addr.s_addr = inet_addr(clientIP.c_str());
    addrto.sin_port = 0;

    // 3. 绑定
    int ret = ::bind(sock, (struct sockaddr *) &(addrto), sizeof(struct sockaddr_in));
    if (0 != ret) {
        return tmpPort;
    }

    // 4. 利用getsockname获取
    struct sockaddr_in connAddr;
    memset(&connAddr, 0, sizeof(struct sockaddr_in));
    unsigned int len = sizeof(connAddr);
    ret = ::getsockname(sock, (sockaddr *) &connAddr, &len);

    if (0 != ret) {
        return tmpPort;
    }

    tmpPort = ntohs(connAddr.sin_port); // 获取端口号
    if (0 == close(sock)) {
        port = tmpPort;
    }

    return port;
}

std::string GetLocalIP(std::string networkSegment) {
    struct ifaddrs *ifaddr = nullptr;
    char host[256];
    memset(host, 0, 256);

    if (int s = getifaddrs(&ifaddr);s == -1) {
        //xloge("getifaddrs() failed :%s", gai_strerror(s));
        return std::string();
    }

    for (auto ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr || ifa->ifa_addr->sa_family != AF_INET) {
            continue;
        }

        int s = getnameinfo(ifa->ifa_addr,
                            sizeof(struct sockaddr_in),
                            host,
                            NI_MAXHOST,
                            nullptr,
                            0,
                            NI_NUMERICHOST);
        if (s != 0) {
            //xloge("getnameinfo() failed: %s", gai_strerror(s));
            return std::string();
        }

        if (std::string(host) == "127.0.0.1") {
            continue;
        }

        if (std::string(host) == "0.0.0.0") {
            continue;
        }

        if (networkSegment.empty()) {
            break;
        }

        if (std::string::npos != std::string(host).find(networkSegment)) {
            break;
        }

    }

    freeifaddrs(ifaddr);

    return host;
}

std::string GetLocalIPForNetworkInterface(std::string networkInterface) {
    if (networkInterface.empty()) {
        return GetLocalIP();
    }

    struct ifaddrs *ifaddr = nullptr;
    if (int s = getifaddrs(&ifaddr);s == -1) {
        return std::string();
    }

    char host[256];
    memset(host, 0, 256);
    std::vector<std::string> localIps;

    for (auto ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr || ifa->ifa_addr->sa_family != AF_INET || ifa->ifa_name == nullptr) {
            continue;
        }

        int s = getnameinfo(ifa->ifa_addr,
                            sizeof(struct sockaddr_in),
                            host,
                            NI_MAXHOST,
                            nullptr,
                            0,
                            NI_NUMERICHOST);
        if (s != 0) {
            continue;
        }

        localIps.push_back(host);
        if (std::string::npos != std::string(ifa->ifa_name).find(networkInterface)) {
            freeifaddrs(ifaddr);
            return std::string(host);
        }

    }

    std::string retIp;
    if (!localIps.empty()) {
        retIp = localIps[0];
    }

    for (const auto &lip : localIps) {
        if (lip == "127.0.0.1") {
            continue;
        }

        if (lip == "0.0.0.0") {
            continue;
        }

        retIp = lip;
        break;
    }

    freeifaddrs(ifaddr);

    return retIp;
}

bool CheckWireless(const std::string& networkInterface, std::string &protocol) {
    if (networkInterface.empty()) {
        return false;
    }

    struct iwreq wrq{};
    /* Set device name */
    strncpy(wrq.ifr_name, networkInterface.c_str(), IFNAMSIZ);

    int skfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (skfd < 0) {
        return false;
    }

    /* Get wireless name */
    int ret = ioctl(skfd, SIOCGIWNAME, &wrq);
    if (ret >= 0) {
        char protocolArry[IFNAMSIZ] = {0};
        strncpy(protocolArry, wrq.u.name, IFNAMSIZ);
        protocol = std::string(protocolArry);
    }

    close(skfd);
    /* If no wireless name : no wireless extensions */
    return ret >= 0;
}

bool CheckSocketPortAvilable(uint16_t port) {
    int socket_fd;
    struct sockaddr_in sin;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        return false;
    }
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    inet_pton(AF_INET, "0.0.0.0", &sin.sin_addr);

    if (bind(socket_fd, (struct sockaddr *) &sin, sizeof(struct sockaddr)) < 0) {
        //("The current sport is occupied !!");
        close(socket_fd);
        return false;
    }

    close(socket_fd);

    return false;
}

bool CheckSocketTimeWait(uint16_t port) {
    // netstat -aultnp | grep 0.0.0.0:5960 | awk '{print $6}'
    std::ostringstream oss;
    oss << "netstat -aultnp | grep 0.0.0.0:" << port << "| awk '{print $6}'";
    const int len = 32;
    char resultBuffer[len] = {0};
    memset(resultBuffer, 0, len);
    auto ret = xlab::base::SystemCmd::RunShell(oss.str().c_str(), "r", resultBuffer, len);
    if (ret <= 0) {
        return false;
    }

    if (::strstr(resultBuffer, "TIME_WAIT") == nullptr) {
        return false;
    }

    return true;
}

}
