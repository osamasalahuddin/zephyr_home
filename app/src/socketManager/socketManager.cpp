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

#include "socketManager.hpp"
#include "socketStrategy.hpp"
#include <memory>

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

bool socketManager::init(Protocol protocol, const std::string& host, uint16_t port)
{
    switch (protocol)
    {
        case Protocol::TCP:
            strategy = std::make_unique<tcpSocketStrategy>();
            break;
        case Protocol::UDP:
            strategy = std::make_unique<udpSocketStrategy>();
            break;
        case Protocol::TLS:
            strategy = std::make_unique<tlsSocketStrategy>();
            break;
        default:
            return false;
    }
    return strategy->connect(host, port);
}

ssize_t socketManager::send(const void* data, size_t len)
{
    if (strategy)
    {
        return strategy->send(data, len);
    }
    return -1;
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
