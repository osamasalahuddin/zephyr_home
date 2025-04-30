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

networkManager& networkManager::getInstance()
{
    static networkManager instance;
    return instance;
}

/**
 * @brief Constructor for the networkManager class.
 */
networkManager::networkManager()
    : WIFI_START_DELAY(1500U), WIFI_CONNECT_TIMEOUT(60000U), CONFIG_MY_LOCAL(MY_LOCAL), CONFIG_MY_REMOTE(MY_REMOTE),
      wifi(wifiManager::getInstance()), ping(pingManager::getInstance())
{
    k_mutex_init(&state_mutex);
    atomic_set(&connection_attempts, 0);
    atomic_set(&start_time, 0);
    atomic_set(&tick_count, 0);
    atomic_set(&is_connect_requested, false);
    atomic_set(&is_new_connection, false);
    atomic_set(&is_lan_connected, false);
    atomic_set(&is_wan_connected, false);
    atomic_set(&wifi_state, static_cast<int>(wifiStateEnum::IDLE));
}

/**
 * @brief Initialize the network.
 */
bool networkManager::init()
{
    bool ret = false;

    /* Initialize Wi-Fi */
    if (!wifi.init())
    {
        MYLOG("Failed to initialize WiFi");
        return ret;
    }

    /* Tick the WiFi Once atleast */
    wifi.tick();

    /* Initialize Ping Manager */
    if (!ping.init())
    {
        MYLOG("Failed to initialize Ping Manager");
        return ret;
    }

    atomic_set(&start_time, k_uptime_get());
    atomic_set(&tick_count, 0);

    MYLOG("NetworkManager initialized");
    ret = true;

    return ret;
}

/**
 * @brief Tick the network.
 */
void networkManager::tick()
{
    k_mutex_lock(&state_mutex, K_FOREVER);

    wifi.tick();
    ping.tick();

    wifiStateEnum current_state = static_cast<wifiStateEnum>(atomic_get(&wifi_state));
    wifiStateEnum new_state     = wifi.getWifiState();

    if (current_state != new_state)
    {
        handleNetworkStateChange(new_state);
    }

    switch (new_state)
    {
        case wifiStateEnum::IDLE:
            if ((k_uptime_get() - atomic_get(&start_time) > WIFI_START_DELAY) && !atomic_get(&is_connect_requested))
            {
                MYLOG("Waiting for Wifi to connect");
                wifi.connect();
                atomic_set(&is_connect_requested, true);
                atomic_set(&is_new_connection, true);
                atomic_inc(&connection_attempts);
            }
            break;

        case wifiStateEnum::CONNECTING:
            if (k_uptime_get() - atomic_get(&start_time) > WIFI_CONNECT_TIMEOUT)
            {
                MYLOG("❌ Failed to connect to Wifi");
                wifi.disconnect();
                atomic_set(&start_time, k_uptime_get());
                resetNetworkState();
            }
            break;

        case wifiStateEnum::CONNECTED:
            atomic_set(&is_connect_requested, false);

            if (atomic_get(&is_new_connection))
            {
                atomic_set(&start_time, k_uptime_get());
                atomic_set(&is_new_connection, false);
            }

            if ((k_uptime_get() - atomic_get(&start_time) > 10000) && !atomic_get(&is_new_connection))
            {
                ping.send_ping(CONFIG_MY_REMOTE.c_str(), wifi.get_wifi_iface(), setIsConnectedWAN);
                ping.send_ping(CONFIG_MY_LOCAL.c_str(), wifi.get_wifi_iface(), setIsConnectedLAN);
                atomic_set(&start_time, k_uptime_get());
                atomic_inc(&tick_count);
            }
            break;

        case wifiStateEnum::ERROR:
            if (shouldReconnect())
            {
                MYLOG("❌ Error in Wifi Initialization. ReInitializing");
                wifi.connect();
                atomic_set(&start_time, k_uptime_get());
                atomic_inc(&connection_attempts);
            }
            break;

        case wifiStateEnum::DISCONNECTED:
            if (shouldReconnect())
            {
                MYLOG("❌ Wifi Reconnecting");
                wifi.connect();
                atomic_set(&start_time, k_uptime_get());
                atomic_inc(&connection_attempts);
            }
            break;
    }

    k_mutex_unlock(&state_mutex);
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
    return atomic_get(&is_lan_connected);
}

/**
 * @brief Get if the network is connected to WAN.
 * @return True or False if connected to WAN.
 */
bool networkManager::isConnectedWAN()
{
    return atomic_get(&is_wan_connected);
}

/**
 * @brief Function to check if the network is up or not.
 * @return True if network is up, false otherwise.
 */
bool networkManager::isNetworkUp()
{
    return (static_cast<wifiStateEnum>(atomic_get(&wifi_state)) == wifiStateEnum::CONNECTED);
}

/**
 * @brief Callback that will be called by pingManager for Local Server Request.
 */
void networkManager::setIsConnectedLAN(bool value)
{
    networkManager& instance = getInstance();
    atomic_set(&instance.is_lan_connected, value);
}

/**
 * @brief Callback that will be called by pingManager for Remote Server Request.
 */
void networkManager::setIsConnectedWAN(bool value)
{
    networkManager& instance = getInstance();
    atomic_set(&instance.is_wan_connected, value);
}

wifiStateEnum networkManager::getNetworkState() const
{
    return static_cast<wifiStateEnum>(atomic_get(&wifi_state));
}

uint32_t networkManager::getConnectionAttempts() const
{
    return atomic_get(&connection_attempts);
}

void networkManager::handleNetworkStateChange(wifiStateEnum new_state)
{
    atomic_set(&wifi_state, static_cast<int>(new_state));
    MYLOG("Network state changed to: %d", static_cast<int>(new_state));
}

void networkManager::resetNetworkState()
{
    atomic_set(&is_connect_requested, false);
    atomic_set(&is_new_connection, false);
    atomic_set(&is_lan_connected, false);
    atomic_set(&is_wan_connected, false);
}

bool networkManager::shouldReconnect() const
{
    return (k_uptime_get() - atomic_get(&start_time) > 10000);
}
