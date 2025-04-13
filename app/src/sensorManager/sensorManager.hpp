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

#include <vector>
#include <string>

#include "sensor.hpp"
#include "sockets.hpp"
#include "iManager.hpp"

class sensorManager : public iManager
{
  public:
    /**
     * @brief Get the singleton instance of the sensorManager class.
     * @return Reference to the singleton instance.
     */
    static sensorManager& getInstance();

    /* Delete copy constructor and assignment operator */
    sensorManager(const sensorManager&)            = delete;
    sensorManager& operator=(const sensorManager&) = delete;

    /**
     * @brief Initialization Function of the sensorManager class.
     * @return true if initialization was successful, false otherwise.
     */
    bool init() override;

    /**
     * @brief Periodic Tick function of the sensorManager class.
     * @note This function should be called periodically to poll all sensors.
     */
    void tick() override;

    /**
     * @brief Name of the manager Class.
     * @return Returns the string literal of the current Manager Class.
     */
    const char* name() const override;

    /**
     * @brief Add a sensor to the manager.
     * @param sensor Pointer to the sensor to add.
     * @param socket Pointer to the socket for the sensor.
     * @return true if sensor was added successfully, false otherwise.
     */
    bool add_sensor(sensor* sensor, sockets* socket);

    /**
     * @brief Cleanup the sensorManager class.
     * @note This function should be called to clean up resources used by the sensorManager.
     */
    void cleanup();

  private:
    /* Private Members */
    static struct k_mutex instance_mutex;
    static sensorManager* instance_ptr;
    struct k_mutex        sensor_mutex;
    bool                  is_initialized = false;

    /**
     * @brief Private constructor for singleton pattern.
     */
    sensorManager();

    /**
     * @brief Private destructor for singleton pattern.
     */
    ~sensorManager();

    /**
     * @brief Structure to hold sensor and socket information.
     */
    struct _sensor
    {
        sensor*  _sensor;
        sockets* _socket;
    };

    /**
     * @brief Vector to store all sensors.
     */
    std::vector<_sensor> sensors;
};
