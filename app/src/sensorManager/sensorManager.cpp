/*
 * This file is part of the Zephyr Home project.
 *
 * Copyright (C) 2025 Osama Salah-ud-Din
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

#include "temperatureSensor.hpp"
#include "lightSensor.hpp"
// #include "humiditySensor.hpp"
// #include "pressureSensor.hpp"
#include "portConfig.hpp"

#include "sensorManager.hpp"
#include "socketManager.hpp"
#include "sockets.hpp"
#include "myLogger.hpp"

#include <zephyr/kernel.h>

/* Initialize static members */
struct k_mutex sensorManager::instance_mutex;
sensorManager* sensorManager::instance_ptr = nullptr;

sensorManager::sensorManager()
{
    k_mutex_init(&sensor_mutex);
    is_initialized = false;
}

sensorManager::~sensorManager()
{
    cleanup();
}

sensorManager& sensorManager::getInstance()
{
    if (instance_ptr == nullptr)
    {
        instance_ptr = new sensorManager();
    }
    return *instance_ptr;
}

bool sensorManager::init()
{
    if (is_initialized)
    {
        return true;
    }

    is_initialized = true;
    MYLOG("✅ SensorManager initialized");

    return true;
}

void sensorManager::tick()
{
    if (!is_initialized)
    {
        return;
    }

    k_mutex_lock(&sensor_mutex, K_FOREVER);

    for (_sensor it : sensors)
    {
        if (it._sensor && it._socket)
        {
            float       value   = it._sensor->get_value();
            std::string payload = std::string(it._sensor->get_id()) + ":" + std::to_string(value);
            it._socket->send(payload.c_str(), payload.length());
        }
    }

    k_mutex_unlock(&sensor_mutex);
}

const char* sensorManager::name() const
{
    return "sensorManager";
}

bool sensorManager::add_sensor(sensor* sensor, sockets* socket)
{
    if (!is_initialized)
    {
        MYLOG("❌ Sensor manager not initialized");
        return false;
    }

    if (!sensor || !socket)
    {
        MYLOG("❌ Invalid sensor or socket pointer");
        return false;
    }

    k_mutex_lock(&sensor_mutex, K_FOREVER);

    /* Check if sensor already exists */
    for (const auto& s : sensors)
    {
        if (s._sensor == sensor)
        {
            MYLOG("❌ Sensor already exists");
            k_mutex_unlock(&sensor_mutex);
            return false;
        }
    }

    sensors.push_back({sensor, socket});
    MYLOG("✅ Added sensor: %s", sensor->get_id());

    k_mutex_unlock(&sensor_mutex);
    return true;
}

void sensorManager::cleanup()
{
    if (!is_initialized)
    {
        return;
    }

    k_mutex_lock(&sensor_mutex, K_FOREVER);

    sensors.clear();
    is_initialized = false;

    MYLOG("Sensor manager cleaned up");
    k_mutex_unlock(&sensor_mutex);
}
