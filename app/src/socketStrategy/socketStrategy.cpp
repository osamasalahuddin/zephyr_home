// app/src/socketStrategy/socketStrategy.cpp
#include "socketStrategy.hpp"
#include <zephyr/net/socket.h>
#include <zephyr/logging/log.h>
#include <zephyr/posix/arpa/inet.h>
#include <unistd.h>
#include <cstring>

LOG_MODULE_REGISTER(socket_strategy, LOG_LEVEL_DBG);

// ================= TCP =================
bool tcpSocketStrategy::connect(const std::string& host, uint16_t port)
{
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) return false;
    return ::connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0;
}

ssize_t tcpSocketStrategy::send(const void* data, size_t len)
{
    return ::send(sock, data, len, 0);
}

ssize_t tcpSocketStrategy::receive(void* buffer, size_t maxLen)
{
    return ::recv(sock, buffer, maxLen, 0);
}

void tcpSocketStrategy::disconnect()
{
    if (sock >= 0) {
        close(sock);
        sock = -1;
    }
}

// ================= UDP =================
bool udpSocketStrategy::connect(const std::string& host, uint16_t port)
{
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &dest.sin_addr);

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    return sock >= 0;
}

ssize_t udpSocketStrategy::send(const void* data, size_t len)
{
    return sendto(sock, data, len, 0, (struct sockaddr*)&dest, sizeof(dest));
}

ssize_t udpSocketStrategy::receive(void* buffer, size_t maxLen)
{
    struct sockaddr_in src = {};
    socklen_t addrlen = sizeof(src);
    return recvfrom(sock, buffer, maxLen, 0, (struct sockaddr*)&src, &addrlen);
}

void udpSocketStrategy::disconnect()
{
    if (sock >= 0) {
        close(sock);
        sock = -1;
    }
}

// ================= TLS =================
bool tlsSocketStrategy::connect(const std::string& host, uint16_t port)
{
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TLS_1_2);
    if (sock < 0) return false;
    return ::connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0;
}

ssize_t tlsSocketStrategy::send(const void* data, size_t len)
{
    return ::send(sock, data, len, 0);
}

ssize_t tlsSocketStrategy::receive(void* buffer, size_t maxLen)
{
    return ::recv(sock, buffer, maxLen, 0);
}

void tlsSocketStrategy::disconnect()
{
    if (sock >= 0) {
        close(sock);
        sock = -1;
    }
}
