/*
 * Copyright (c) 2025 Osama Salah-ud-Din.
*/

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <app_version.h>

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#include "myLogger.h"
#include "networkManager.hpp"

int main(void)
{
    /* Main Function */
    MYLOG("Hello World!");

    /* Initialize Network Manager */
    networkManager& network = networkManager::getInstance();
    network.init();


    uint64_t start = k_uptime_get();

    while (true)
    {
        network.tick();

        if (network.isNetworkUp())
        {
            if (k_uptime_get() - start > 10000)
            {
                start = k_uptime_get();
                if (network.isConnectedLAN())
                {
                    MYLOG(" 💻 Connected to LAN");
                }
                else
                {
                    MYLOG("Not connected to LAN");
                }
                if (network.isConnectedWAN())
                {
                    MYLOG("🌐 Connected to WAN");
                }
                else
                {
                    MYLOG("Not connected to WAN");
                }
            }
        }
    }
    return 0;
}
