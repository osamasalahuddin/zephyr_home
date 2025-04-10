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

#include <memory>

#include "socketManager.hpp"
#include "socketStrategy.hpp"

#include "myLogger.hpp"

static socketManager* instance_ptr = nullptr;

socketManager& socketManager::getInstance()
{
    if (!instance_ptr)
    {
        static socketManager inst;
        instance_ptr = &inst;
    }
    return *instance_ptr;
}

bool socketManager::open(protocol proto, const std::string& host, uint16_t port)
{
    auto key = std::make_tuple(proto, host, port);
    bool ret = false;

    if (sockets.find(key) != sockets.end())
    {
        MYLOG("Socket already open for protocol %d port %d on Host %s",
                (int)proto, port, host.c_str());
    }
    else
    {
        auto strategy = createStrategy(proto, port);
        if (strategy && strategy->connect(host, port))
        {
            /* Store it in the socket list */
            sockets[key] = std::move(strategy);

            MYLOG("Opened %d socket on port %d", (int)proto, port);
            ret = true;
        }
        else
        {
            MYLOG("Failed to open %d socket on port %d", (int)proto, port);
        }
    }
    return ret;
}

ssize_t socketManager::send(std::string& host, protocol proto, uint16_t port ,const void* data, size_t len)
{
    int ret = -1;

    auto key = std::make_tuple(proto, host, port);

    auto it = sockets.find(key);
    if (it != sockets.end())
    {
        ret = it->second->send(data, len);
    }
    else
    {
        MYLOG("No socket open for port %d",  port);
    }

    return ret;
}

ssize_t socketManager::receive(void* buffer, size_t maxLen)
{
    if (strategy)
    {
        return strategy->receive(buffer, maxLen);
    }
    return -1;
}

void socketManager::shutdown()
{
    if (strategy)
    {
        strategy->disconnect();
        strategy.reset();
    }
}

std::unique_ptr<socketStrategy> socketManager::createStrategy(socketManager::protocol proto, uint16_t port)
{
    switch (proto)
    {
        case protocol::TCP:
            return std::make_unique<tcpSocketStrategy>();
        case protocol::UDP:
            return std::make_unique<udpSocketStrategy>();
        case protocol::TLS:
            return std::make_unique<tlsSocketStrategy>();
        default:
            MYLOG("Unknown protocol type: %d", proto);
            return nullptr;
    }
}
