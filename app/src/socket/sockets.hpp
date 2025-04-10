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
#include <string>
#include "socketManager.hpp"

/**
 * @brief Wrapper class to manage individual socket lifecycle and data flow.
 * @note Encapsulates socketManager interaction for ease of use.
 */
class sockets
{
public:
    /**
     * @brief Construct a new sockets object.
     * @note Initializes socketManager reference for internal operations.
     */
    sockets();

    /**
     * @brief Destroy the sockets object.
     * @note Automatically closes the socket if still open.
     */
    ~sockets();

    /**
     * @brief Open a socket for the given host, port, and protocol.
     *
     * @param host Target host IP or hostname.
     * @param port Port number to connect to.
     * @param proto Protocol to use (UDP, TCP, TLS).
     * @return true if socket opened successfully, false otherwise.
     */
    bool open(std::string host, uint16_t port, socketManager::protocol proto);

    /**
     * @brief Close the socket connection.
     * @note Removes socket from the socketManager registry.
     */
    void close();

    /**
     * @brief Send data through the socket.
     *
     * @param data Pointer to data buffer to send.
     * @param len Length of the buffer in bytes.
     * @return uint32_t Number of bytes sent or error code.
     */
    uint32_t send(const char* data, size_t len);

    /**
     * @brief Receive data from the socket.
     *
     * @param buffer Destination buffer to receive data.
     * @param maxLen Maximum number of bytes to read.
     */
    void receive(char* buffer, size_t maxLen);

private:
    /**
     * @brief Pointer to the singleton socketManager instance.
     */
    socketManager* pSocketManager;

    /**
     * @brief Protocol in use (UDP, TCP, TLS).
     */
    socketManager::protocol proto;

    /**
     * @brief Host address for the socket.
     */
    std::string host;

    /**
     * @brief Port number used by the socket.
     */
    uint16_t port;
};
