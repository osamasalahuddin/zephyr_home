/*
 * This file is part of the Zephyr Home project.
 *
 * Copyright (C) 2024 Osama Salah-ud-din
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
#include "wifiState.hpp"
#include "myLogger.hpp"

#include <zephyr/net/net_if.h>
#include <zephyr/net/wifi_mgmt.h>

typedef enum
{
    IDLE = 0,
    CONNECTING,
    CONNECTED,
    DISCONNECTED,
    ERROR,
} wifiStateEnum;

class wifiContext
{
private:
    wifiState* state;
    net_if* iface;

public:
    wifiContext(wifiState* initial, net_if* iface);
    void setState(wifiState* newState);
    wifiStateEnum  getState();
    const char *getStateName();
    void update(wifi_iface_status status);
};
