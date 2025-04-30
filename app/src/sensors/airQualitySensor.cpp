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

#include "airQualitySensor.hpp"
#include "myLogger.hpp"

airQualitySensor::airQualitySensor()
{
    k_mutex_init(&sensor_mutex);

    const struct device* dev;

    // dev = DEVICE_DT_GET(DT_NODELABEL(air_quality_sensor));

    dev = device_get_binding("air_quality_sensor");
    if (NULL == dev)
    {
        MYLOG(" Air Quality Sensor Device not found");
    }

    if (!::device_is_ready(dev))
    {
        MYLOG(" Air Quality Sensor Device not ready");
    }
    else
    {
        is_initialized = true;
        MYLOG("✅ Air Quality Sensor initialized");
    }
}

const char* airQualitySensor::get_id() const
{
    return "air_quality";
}

float airQualitySensor::get_value() const
{
    float value = last_value;
    return value;
}

void airQualitySensor::tick()
{
    if (!is_initialized)
    {
        return;
    }

    k_mutex_lock(&sensor_mutex, K_FOREVER);

    float new_value = read_value();
    if (validate_value(new_value))
    {
        last_value = new_value;
        has_error  = false;
    }
    else
    {
        has_error = true;
        MYLOG("❌ Invalid air quality value: %f", (double) new_value);
    }

    k_mutex_unlock(&sensor_mutex);
}

float airQualitySensor::read_value()
{
    /* Mock sensor value, replace with actual driver read */
    return 50.0f; /* Typical air quality index value */
}

bool airQualitySensor::validate_value(float value) const
{
    /* Air Quality Index typically ranges from 0 to 500 */
    return value >= 0.0f && value <= 500.0f;
}