/*
 * This file is part of the Zephyr Home project.
 *
 * Copyright (C) 2024 Osama Salah-ud-din
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "socketStrategy.hpp"
#include <zephyr/net/socket.h>
#include <zephyr/posix/arpa/inet.h>
#include <unistd.h>
#include <cstring>

#include"myLogger.hpp"


// ================= TCP =================
bool tcpSocketStrategy::connect(const std::string& host, uint16_t port)
{
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
        return false;

    int ret = ::connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0)
    {
        sock = -1;
        MYLOG("Failed to connect to %s:%d return Code:%d", host.c_str(), port, ret);
        return false;
    }
    else
    {
        return true;
    }
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
    if (sock >= 0)
    {
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
    if (sock >= 0)
    {
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
    if (sock < 0)
        return false;

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
    if (sock >= 0)
    {
        close(sock);
        sock = -1;
    }
}
