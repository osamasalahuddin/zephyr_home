#include "networkManager.hpp"
#include "myLogger.h"

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
                ping(pingManager::getInstance()),
                wifi(wifiManager::getInstance()),
                CONFIG_MY_LOCAL(MY_LOCAL),
                CONFIG_MY_REMOTE(MY_REMOTE),
                isConnectRequested(false),
                isNewConnect(false),
                wifiState(wifiStateEnum::IDLE),
                start(0),
                ticks(0)
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

    int64_t start = k_uptime_get();
    uint8_t ticks = 0;

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
        /* If 500ms have passed we can connect using Wifi */
        if ((k_uptime_get() - start > 500) && !isConnectRequested)
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
        if (k_uptime_get() - start > 60000)
        {
            MYLOG("❌ Failed to connect to Wifi");
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
