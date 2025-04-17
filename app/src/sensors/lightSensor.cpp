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

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

#include "lightSensor.hpp"
#include "myLogger.hpp"

lightSensor::lightSensor() : lux(0.0f), dev(nullptr)
{
    // dev = DEVICE_DT_GET(DT_N_NODELABEL_mylight_sensor);
    // dev = DEVICE_DT_GET(DT_NODELABEL(light_sensor));

    dev = device_get_binding("light_sensor");
    if (NULL == dev)
    {
        MYLOG(" Light Sensor Device not found");
    }

    if (!::device_is_ready(dev))
    {
        MYLOG(" Light Sensor Device not ready");
    }
    else
    {
        MYLOG(" Light Sensor Initialized");
    }
}

const char* lightSensor::get_id() const
{
    return "lightSensor";
}

void lightSensor::tick()
{
    lux = read_value();
    if (lux >= 0.0f)
    {
        MYLOG("📸 Light: %.2f lux", (double) lux);
    }
}

float lightSensor::get_value() const
{
    return lux;
}

float lightSensor::read_value()
{
    float return_value = -1.0f;
    if (::device_is_ready(dev))
    {
        /* Sample the data from the device */
        int err_code = ::sensor_sample_fetch(dev);

        if (err_code < 0)
        {
            MYLOG("Failed to fetch light sensor data. Error: %d", err_code);
        }
        else
        {
            struct sensor_value value;

            /* Get the sampled data */
            err_code = ::sensor_channel_get(dev, SENSOR_CHAN_LIGHT, &value);

            if (err_code < 0)
            {
                MYLOG("Failed to read light sensor. Error: %d", err_code);
            }
            else
            {
                /* Convert the Sensor value to double precision float */
                return_value = ::sensor_value_to_double(&value);
            }
        }
    }
    else
    {
        MYLOG(" Light Sensor Device Error");
    }

    return return_value;
}
