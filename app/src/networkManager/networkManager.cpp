#include "networkManager.hpp"
#include "myLogger.h"

/**
 * @brief Get the singleton instance of the networkManager class.
 * @return Reference to the singleton instance.
 */
networkManager& networkManager::instance()
{
    static networkManager instance; /* Singleton instance */
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
                state(wifiStateEnum::IDLE),
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
    state = wifi.getWifiState();

    if (wifiStateEnum::IDLE == state)
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
    else if (wifiStateEnum::CONNECTING == state)
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
    else if (wifiStateEnum::CONNECTED == state)
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
            ping.send_ping(CONFIG_MY_REMOTE.c_str(), wifi.get_wifi_iface());

            /* Send Ping to the Local server */
            ping.send_ping(CONFIG_MY_LOCAL.c_str(), wifi.get_wifi_iface());

            start = k_uptime_get();

            ticks++;

            if (ticks == 6)
            {
                MYLOG("✅ Wifi still Connected");
                ticks = 0;
            }
        }
    }
    else if (wifiStateEnum::ERROR == state)
    {
        if (k_uptime_get() - start > 10000)
        {
            MYLOG("❌ Error in Wifi Initialization. ReInitializing");
            wifi.connect();
            start = k_uptime_get();
        }
    }
    else if (wifiStateEnum::DISCONNECTED == state)
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
