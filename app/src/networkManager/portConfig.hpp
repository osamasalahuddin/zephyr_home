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

/* Temperature Sensor port (UDP) */
constexpr int PORT_TEMP_SENSOR = 50000;

/* Light Sensor port (UDP) */
constexpr int PORT_LIGHT_SENSOR = 50001;

/* Air Quality Sensor port (UDP) */
constexpr int PORT_AIR_QUALITY_SENSOR = 50002;

/* Reserved for future use */
constexpr int DEBUG_CONSOLE = 50050;

} // namespace portConfig
