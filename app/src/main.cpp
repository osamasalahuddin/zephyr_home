/*
 * Copyright (c) 2025 Osama Salah-ud-Din.
*/

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <app_version.h>

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_event.h>

/* For Ping */
#include <zephyr/net/socket.h>
#include <zephyr/net/icmp.h>

#include "myLogger.h"
#include "handlers.h"
#include "wifi.h"
#include "wifiManager.hpp"
#include "pingManager.hpp"

wifiManager wifi;

int main(void)
{
    /* Main Function */
    MYLOG("Hello World!");

    /* Prechecks for Connecting with Wifi */
    // struct wifi_iface_status status = {0};

    wifi.init();

    wifi.tick();
    int64_t start = k_uptime_get();
    uint8_t ticks = 0;

    bool isConnectRequested = false;
    bool isNewConnect = false;
    wifiStateEnum state;

    pingManager& ping = pingManager::instance();

    ping.init();

    while (true)
    {
        wifi.tick();
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
                ping.send_ping("8.8.8.8", wifi.get_wifi_iface());

                /* Send Ping to the Local server */
                ping.send_ping("192.168.0.223", wifi.get_wifi_iface());

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

    return 0;
}
