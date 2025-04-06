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
    networkManager& network = networkManager::instance();
    network.init();

    while (true)
    {
        network.tick();
    }

    return 0;
}
