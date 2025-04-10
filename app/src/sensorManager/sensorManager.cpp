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

#include "sensorManager.hpp"
#include "socketManager.hpp"


void sensorManager::add_sensor(sensor* sensor)
{
    sensors.push_back(sensor);
}

void sensorManager::poll_all()
{
    for (auto* sensor : sensors)
    {
        float value = sensor->get_value();
        std::string payload = std::string(sensor->get_id()) + ":" + std::to_string(value);
        socketManager::getInstance().send(payload.c_str(), payload.length());
    }
}
