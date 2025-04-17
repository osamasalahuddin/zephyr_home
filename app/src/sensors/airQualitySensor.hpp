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
#include <zephyr/kernel.h>
#include <atomic>

class airQualitySensor : public sensor
{
  public:
    /**
     * @brief Constructor for the airQualitySensor class.
     */
    airQualitySensor();

    /**
     * @brief Get the sensor ID.
     * @return The sensor ID as a string.
     */
    const char* get_id() const override;

    /**
     * @brief Get the current air quality value.
     * @return The current air quality value.
     */
    float get_value() const override;

    /**
     * @brief Periodic tick function for the sensor.
     * @note This function should be called periodically to update the sensor value.
     */
    void tick() override;

  private:
    /**
     * @brief Read the raw value from the sensor.
     * @return The raw sensor value.
     */
    float read_value() override;

    /**
     * @brief Validate the sensor value.
     * @param value The value to validate.
     * @return true if the value is valid, false otherwise.
     */
    bool validate_value(float value) const;

    /* Private Members */
    struct k_mutex     sensor_mutex;
    std::atomic<float> last_value{0.0f};
    std::atomic<bool>  is_initialized{false};
    std::atomic<bool>  has_error{false};
};