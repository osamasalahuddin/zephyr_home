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

#pragma once
#include "sensor.hpp"

class lightSensor : public sensor
{
public:

    /**
     * @brief Constructor for Light Sensor. Initializes the Zypher device.
     */
    lightSensor();

    /**
     * @brief Return the string name of the Sensor.
     * @return const char* ID of the sensor.
     */
    const char* get_id() const override;

    /**
     * @brief Get the Sensor value.
     * @return std::float current sensor value.
     */
    float get_value() const override;

    /**
     * @brief Tick function to be called periodically to update the sensor value.
     */
    void tick() override;

private:

    /**
     * @brief Get the updated value from the sensor.
     * @return std::float current sensor value.
     */
    float read_value() override;

    float lux;

    const struct device* dev;
};
