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

#include "networkManager.hpp"
#include "myLogger.hpp"

/**
 * @brief Get the singleton instance of the networkManager class.
 * @return Reference to the singleton instance.
 */
networkManager& networkManager::getInstance()
{
    /* Singleton instance */
    static networkManager instance;
    return instance;
}

/**
 * @brief Constructor for the networkManager class.
 */
networkManager::networkManager():
                WIFI_START_DELAY(1500U),
                WIFI_CONNECT_TIMEOUT(120000U),
                CONFIG_MY_LOCAL(MY_LOCAL),
                CONFIG_MY_REMOTE(MY_REMOTE),
                start(0),
                ticks(0),
                isConnectRequested(false),
                isNewConnect(false),
                isLanConnected(false),
                isWanConnected(false),
                wifi(wifiManager::getInstance()),
                ping(pingManager::getInstance())
{
}

/**
 * @brief Initialize the network.
 */
void networkManager::init()
{
    /* Initialize Wi-Fi */
    wifi.init();
    wifi.tick();

    ping.init();

    start = k_uptime_get();
    ticks = 0;

    MYLOG("NetworkManager initialized");
}

/**
 * @brief Tick the network.
 */
void networkManager::tick()
{
    wifi.tick();
    ping.tick();
    wifiState = wifi.getWifiState();

    if (wifiStateEnum::IDLE == wifiState)
    {
        /* If 1500ms have passed we can connect using Wifi */
        if ((k_uptime_get() - start > WIFI_START_DELAY) && !isConnectRequested)
        {
            MYLOG("Waiting for Wifi to connect");
            wifi.connect();
            isConnectRequested = true;
            isNewConnect = true;
        }
    }
    else if (wifiStateEnum::CONNECTING == wifiState)
    {
        /* Wait for Connection to be established */
        if (k_uptime_get() - start > WIFI_CONNECT_TIMEOUT)
        {
            MYLOG("❌ Failed to connect to Wifi ");

            /* Disconnect from Wifi */
            wifi.disconnect();

            /* Get New start of time for reconnection */
            start = k_uptime_get();
        }
    }
    else if (wifiStateEnum::CONNECTED == wifiState)
    {
        /* Do WiFi Stuff after this */
        isConnectRequested = false;

        if (true == isNewConnect)
        {
            start = k_uptime_get();
            isNewConnect = false;
        }

        /* Reset the time and check after every 10 seconds */
        if ((k_uptime_get() - start > 10000) && !isNewConnect)
        {
            /* Send ping to Remote Server */
            ping.send_ping(CONFIG_MY_REMOTE.c_str(), wifi.get_wifi_iface(), setIsConnectedWAN);

            /* Send Ping to the Local server */
            ping.send_ping(CONFIG_MY_LOCAL.c_str(), wifi.get_wifi_iface(), setIsConnectedLAN);

            start = k_uptime_get();

            ticks++;
        }
    }
    else if (wifiStateEnum::ERROR == wifiState)
    {
        if (k_uptime_get() - start > 10000)
        {
            MYLOG("❌ Error in Wifi Initialization. ReInitializing");
            wifi.connect();
            start = k_uptime_get();
        }
    }
    else if (wifiStateEnum::DISCONNECTED == wifiState)
    {
        /* Connect after 10 seconds */
        if (k_uptime_get() - start > 10000)
        {
            MYLOG("❌ Wifi Reconnecting");
            wifi.connect();

            /* Get New start of time for reconnection */
            start = k_uptime_get();
        }
    }
}


/**
 * @brief Get the name of the manager.
 * @return Name of the manager.
 */
const char* networkManager::name() const
{
    return "networkManager";
}

/**
 * @brief Get the address of the local server.
 * @return std::string name of the local server.
 */
std::string networkManager::getLocalServer()
{
    return CONFIG_MY_LOCAL;
}

/**
 * @brief Get if the network is connected to LAN.
 * @return True or False if connected to LAN.
 */
bool networkManager::isConnectedLAN()
{
    return isLanConnected;
}

/**
 * @brief Get if the network is connected to WAN.
 * @return True or False if connected to WAN.
 */
 bool networkManager::isConnectedWAN()
 {
    return isWanConnected;
 }


/**
 * @brief Function to check if the network is up or not.
 * @return True if network is up, false otherwise.
 */
bool networkManager::isNetworkUp()
{
    return (wifi.getWifiState() == wifiStateEnum::CONNECTED);
}

/**
 * @brief Callback that will be called by pingManager for Local Server Request.
 */
void networkManager::setIsConnectedLAN(bool value)
{
    getInstance().isLanConnected = value;
}

/**
 * @brief Callback that will be called by pingManager for Remote Server Request.
 */
void networkManager::setIsConnectedWAN(bool value)
{
    getInstance().isWanConnected = value;
}
