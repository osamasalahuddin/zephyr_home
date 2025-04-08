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

#pragma once

#include <zephyr/net/socket.h>
#include <string>

class socketStrategy
{
public:
    virtual bool connect(const std::string& host, uint16_t port) = 0;
    virtual ssize_t send(const void* data, size_t len) = 0;
    virtual ssize_t receive(void* buffer, size_t maxLen) = 0;
    virtual void disconnect() = 0;
    virtual ~socketStrategy() = default;
};

class tcpSocketStrategy : public socketStrategy
{
private:
    int sock = -1;

public:
    bool connect(const std::string& host, uint16_t port) override;
    ssize_t send(const void* data, size_t len) override;
    ssize_t receive(void* buffer, size_t maxLen) override;
    void disconnect() override;
};

class udpSocketStrategy : public socketStrategy
{
private:
    int sock = -1;
    struct sockaddr_in dest = {};

public:
    bool connect(const std::string& host, uint16_t port) override;
    ssize_t send(const void* data, size_t len) override;
    ssize_t receive(void* buffer, size_t maxLen) override;
    void disconnect() override;
};

class tlsSocketStrategy : public socketStrategy
{
private:
    int sock = -1;

public:
    bool connect(const std::string& host, uint16_t port) override;
    ssize_t send(const void* data, size_t len) override;
    ssize_t receive(void* buffer, size_t maxLen) override;
    void disconnect() override;
};
