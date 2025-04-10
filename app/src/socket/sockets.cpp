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

#include "sockets.hpp"

sockets::sockets()
{
    pSocketManager = &socketManager::getInstance();
    host = "0";
    port = 0;
    proto = socketManager::protocol::UDP;
}

sockets::~sockets()
{
    close();
}

bool sockets::open(std::string server, uint16_t _port, socketManager::protocol protocol)
{
    /* Save the parameters */
    host = server;
    port = _port;
    proto = protocol;
    return pSocketManager->open(proto, host, port);
}

void sockets::close()
{

}

uint32_t sockets::send(const char* data, size_t len)
{
    return pSocketManager->send(host, proto, port, data, len);
}
