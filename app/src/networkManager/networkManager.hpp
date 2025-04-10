
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

#include "iManager.hpp"
#include "wifiManager.hpp"
#include "pingManager.hpp"
#include "portConfig.hpp"

/**
 * @class networkManager
 * @brief Singleton class to manage network-related functionality.
 */
class networkManager  : public iManager
{
public:
    /**
     * @brief Get the singleton instance of the networkManager class.
     * @return Reference to the singleton instance.
     */
    static networkManager& getInstance();

    /**
     * @brief Initialize the network.
     */
    void init();

    /**
     * @brief Tick the network.
     */
    void tick() override;

    /**
     * @brief Get the name of the manager.
     * @return Name of the manager.
     */
    const char* name() const override;

    /**
     * @brief Get the address of the local server.
     * @return std::string name of the local server.
     */
    std::string getLocalServer();

    /**
     * @brief Get if the network is connected to LAN.
     * @return True or False if connected to LAN.
     */
    bool isConnectedLAN();

    /**
     * @brief Get if the network is connected to WAN.
     * @return True or False if connected to WAN.
     */
    bool isConnectedWAN();

    /**
     * @brief Function to check if the network is up or not.
     * @return True if network is up, false otherwise.
     */
    bool isNetworkUp();
private:

    /** Internal Variables */

    /**
     * @brief Flag to check if the network is requested to connect.
     * @note This is set to true when the network manager is requested to connect
     */
    bool isConnectRequested;

    /**
     * @brief Flag to check if the network is in a new connection state.
     * @note This is set to true when the network manager is requested to connect
     */
    bool isNewConnect;

    /**
     * @brief Flag to check if the network is connected to LAN or not.
     * @note This is set to true when the pingManager receives a successful response
     */
    bool isLanConnected;

    /**
     * @brief Flag to check if the network is connected to WAN or not.
     * @note This is set to true when the pingManager receives a successful response
     */
    bool isWanConnected;

    /**
     * @brief State of the WiFi connection.
     * @note This is set to the current state of the WiFi connection
     *       using the wifiManager class.
     */
    wifiStateEnum wifiState;

    int64_t start;
    uint8_t ticks;

    /**
     * @brief Wait time before starting of the Wifi SM
     * @note This is set to 1500ms
     */
    const uint16_t WIFI_START_DELAY;

    /**
     * @brief Wait time for letting Wifi to connect to the network
     * @note This is set to 120 seconds
     */
    const uint16_t WIFI_CONNECT_TIMEOUT;

    /**
     * @brief String Values gotten from build system for LAN Server
     */
    const std::string CONFIG_MY_LOCAL;

    /**
     * @brief String Values gotten from build system for WAN Server
     */
    const std::string CONFIG_MY_REMOTE;

    /** WiFi instance pointer */
    wifiManager& wifi;

    /** PingManager instance pointer */
    pingManager& ping;

    /**
     * @brief Private constructor for the singleton pattern.
     */
    networkManager();

    /**
     * @brief Disable copy constructor.
     */
    networkManager(const networkManager&) = delete;

    /**
     * @brief Disable assignment operator.
     */
    networkManager& operator=(const networkManager&) = delete;

    /**
     * @brief Callback that will be called by pingManager for Local Server Request.
     */
    static void setIsConnectedLAN(bool value);

    /**
     * @brief Callback that will be called by pingManager for Remote Server Request.
     */
    static void setIsConnectedWAN(bool value);
};
