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

#include "sensorManager.hpp"
#include "temperatureSensor.hpp"
#include "myLogger.hpp"

temperatureSensor::temperatureSensor()
{
    // Initialize the sensor
    const struct device* dev;
    // dev = DEVICE_DT_GET(DT_NODELABEL(temperature_sensor));

    if (NULL == dev)
    {
        MYLOG(" Temperature Sensor Device not found");
    }

    if (!::device_is_ready(dev))
    {
        MYLOG(" Temperature Sensor Device not ready");
    }
    else
    {
        MYLOG(" Temperature Sensor Initialized");
    }
}

const char* temperatureSensor::get_id() const
{
    return "temperature";
}

float temperatureSensor::get_value() const
{
    // Mock sensor value, replace with actual driver read
    return 22.5f;
}

float temperatureSensor::read_value()
{
    // Mock sensor value, replace with actual driver read
    return 22.5f;
}

void temperatureSensor::tick()
{
    // no-op for polling in this basic implementation
}
