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
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <app_version.h>

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#include "myLogger.hpp"

#include "networkManager.hpp"
#include "networkTimeManager.hpp"
#include "socketManager.hpp"
#include "sockets.hpp"

#include "sensorManager.hpp"
#include "airQualitySensor.hpp"
#include "lightSensor.hpp"
#include "temperatureSensor.hpp"

#define STACK_SIZE (4096)
#define TASK_PRIORITY (-1)

K_THREAD_STACK_DEFINE(ntp_stack, STACK_SIZE);

static struct k_thread ntp_thread;

K_MUTEX_DEFINE(mutex);
K_CONDVAR_DEFINE(condvar);

static void ntp_sync_thread(void*, void*, void*)
{
    networkManager&     network = networkManager::getInstance();
    networkTimeManager& ntp     = networkTimeManager::getInstance();

    while (true)
    {
        if (network.isConnectedWAN())
        {
            ntp.tick();
            MYLOG("⏰ System Time Synced");
        }
        k_sleep(K_MINUTES(1));
    }
}

int main(void)
{
    /* Main Function */
    MYLOG("Hello World!");

    lightSensor       lightSensor;
    airQualitySensor  airQualitySensor;
    temperatureSensor temperatureSensor;

    sensorManager& sensorMgr = sensorManager::getInstance();

    sockets socketAirQualitySensor;
    sockets socketLightSensor;
    sockets socketTempSensor;

    networkManager& network = networkManager::getInstance();
    myLogger&       logger  = myLogger::getInstance();

    /* Initialize Network Manager */
    network.init();
    logger.init();
    sensorMgr.init();

    sensorMgr.add_sensor(&lightSensor, &socketLightSensor);
    sensorMgr.add_sensor(&airQualitySensor, &socketAirQualitySensor);
    sensorMgr.add_sensor(&temperatureSensor, &socketTempSensor);

    // socketLightSensor.open(networkManager::getInstance().getLocalServer(), portConfig::PORT_AIR_QUALITY_SENSOR,
    //                        socketManager::protocol::UDP);

    // socketLightSensor.open(networkManager::getInstance().getLocalServer(), portConfig::PORT_LIGHT_SENSOR,
    //                        socketManager::protocol::UDP);

    // socketLightSensor.open(networkManager::getInstance().getLocalServer(), portConfig::PORT_TEMP_SENSOR,
    //                        socketManager::protocol::UDP);

    bool isSocket =
        socketTempSensor.open(network.getLocalServer(), portConfig::PORT_TEMP_SENSOR, socketManager::protocol::UDP);
    if (!isSocket)
    {
        MYLOG(" Temperature Sensor Socket Initialization Failed: %d", isSocket);
    }

    isSocket = socketAirQualitySensor.open(network.getLocalServer(), portConfig::PORT_AIR_QUALITY_SENSOR,
                                           socketManager::protocol::UDP);
    if (!isSocket)
    {
        MYLOG(" Air Quality Sensor Socket Initialization Failed: %d", isSocket);
    }

    isSocket =
        socketLightSensor.open(network.getLocalServer(), portConfig::PORT_LIGHT_SENSOR, socketManager::protocol::UDP);
    if (!isSocket)
    {
        MYLOG(" Light Sensor Socket Initialization Failed: %d", isSocket);
    }

    uint64_t start = k_uptime_get();

    /* Create a Thread for SNTP Issue */
    k_thread_create(&ntp_thread, ntp_stack, K_THREAD_STACK_SIZEOF(ntp_stack), ntp_sync_thread, NULL, NULL, NULL,
                    TASK_PRIORITY, 0, K_NO_WAIT);

    while (true)
    {
        network.tick();

        if (network.isNetworkUp())
        {
            if (k_uptime_get() - start > 10000)
            {
                sensorMgr.tick();
                start = k_uptime_get();
                if (network.isConnectedLAN())
                {
                    MYLOG(" 💻 Connected to LAN");
                    if (isSocket)
                    {
                        MYLOG("Sent Data to local server. Return: %d", socketTempSensor.send("LAN", 4));
                    }
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
