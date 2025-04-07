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

#include "wifiStateImp.hpp"
#include "wifiContext.hpp"
#include "myLogger.h"
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_mgmt.h>

wifiStateIdle::wifiStateIdle(wifiStateConnecting* next) : connecting(next)
{

}

void wifiStateIdle::enter(wifiContext& ctx, net_if* _iface)
{
    MYLOG("🟡 Entered Idle state");
    isConnectingCalled = false;
    iface = _iface;
}

void wifiStateIdle::handle(wifiContext& ctx, wifi_iface_status status)
{
    if (isConnectingCalled)
    {
        MYLOG("🟡 Handling Idle state... transitioning to Connecting");

        if (status.state == WIFI_STATE_INACTIVE || status.state == WIFI_STATE_DISCONNECTED)
        {
            MYLOG("WiFi is Inactive or Disconnected");
            MYLOG("➡️ Transitioning to Connecting...");
            ctx.setState(connecting);
        }
    }
}

int wifiStateIdle::name() const
{
    return static_cast<int>(IDLE);
}

void wifiStateIdle::setConnectingCalled(bool value)
{
    isConnectingCalled = value;
}

bool wifiStateIdle::getConnectingCalled()
{
    return isConnectingCalled;
}