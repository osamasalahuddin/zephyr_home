/*
 * Copyright (c) 2025 Osama Salah-ud-Din.
*/

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <app_version.h>

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#include "myLogger.h"
#include "networkManager.hpp"
#include "networkTimeManager.hpp"

#define STACK_SIZE      (4096)
#define TASK_PRIORITY   (-1)

K_THREAD_STACK_DEFINE(ntp_stack,  STACK_SIZE);

static struct k_thread ntp_thread;

K_MUTEX_DEFINE(mutex);
K_CONDVAR_DEFINE(condvar);


static void ntp_sync_thread(void*, void*, void*)
{
    networkManager& network = networkManager::getInstance();
    networkTimeManager& ntp = networkTimeManager::getInstance();

    while (true)
    {
        if (network.isConnectedWAN())
        {
            ntp.sync(nullptr, 5000);
            MYLOG("⏰ System Time Synced");
        }
        k_sleep(K_SECONDS(10));
    }
}

int main(void)
{
    /* Main Function */
    MYLOG("Hello World!");

    /* Initialize Network Manager */
    networkManager& network = networkManager::getInstance();
    network.init();
    networkTimeManager& ntp = networkTimeManager::getInstance();

    uint64_t start = k_uptime_get();

    /* Create a Thread for SNTP Issue */
    k_thread_create(&ntp_thread, ntp_stack, K_THREAD_STACK_SIZEOF(ntp_stack),
                    ntp_sync_thread, NULL, NULL, NULL,
                    TASK_PRIORITY, 0, K_NO_WAIT);

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
