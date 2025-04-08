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

/* Port assignments for Zephyr Home project */

namespace portConfig
{

    /* Sensor data upload port (TCP) */
    constexpr int PORT_TEMP_SENSOR   = 5000;

    /* Device control port (TCP) */
    constexpr int DEVICE_CONTROL     = 5001;

    /* Time sync proxy service (if applicable) */
    constexpr int TIME_SYNC_PROXY    = 5002;

    /* Reserved for future use */
    constexpr int DEBUG_CONSOLE      = 5050;

} // namespace portConfig
