/*
 * This file is part of the Zephyr Home project.
 *
 * Copyright (C) 2025 Osama Salah-ud-Din
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

#include "myLogger.hpp"
#include "networkManager.hpp"
#include "socketManager.hpp"

static networkManager& dbgNetwork = networkManager::getInstance();;
static socketManager& dbgSocket = socketManager::getInstance();

/**
 * @brief Get the singleton instance of the networkManager class.
 * @return Reference to the singleton instance.
 */
myLogger& myLogger::getInstance()
{
    /* Singleton instance */
    static myLogger instance;
    return instance;
}


void myLogger::init()
{
    /* If already connected to Wifi */

    isSocket = dbgSocket.init(socketManager::Protocol::UDP,
                            dbgNetwork.getLocalServer().c_str(),
                            portConfig::DEBUG_CONSOLE);
    if (!isSocket)
    {
        MYLOG("Network Logger Initialization Failed");
    }
}

void myLogger::send(const char* log_msg, size_t len)
{
    if (dbgNetwork.isNetworkUp())
    {
        dbgSocket.send(log_msg, len);
    }
}