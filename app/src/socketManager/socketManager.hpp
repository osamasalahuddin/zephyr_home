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

#include "socketStrategy.hpp"
#include <memory>
#include <string>
#include <map>

class socketManager
{
public:
    enum protocol
    {
        TCP,
        UDP,
        TLS
    };

    static socketManager& getInstance();

    // bool init(protocol proto, const std::string& host, uint16_t port);


    bool open(protocol proto, const std::string& host, uint16_t port);
    void close(protocol proto, uint16_t port);

    ssize_t send(uint16_t port, const void* data, size_t len);
    ssize_t receive(void* buffer, size_t maxLen);
    void shutdown();

private:
    socketManager() = default;

    std::map<std::tuple<protocol, uint16_t>, std::unique_ptr<socketStrategy>> sockets;

    std::unique_ptr<socketStrategy> createStrategy(protocol proto, uint16_t port);

    std::unique_ptr<socketStrategy> strategy;
};
