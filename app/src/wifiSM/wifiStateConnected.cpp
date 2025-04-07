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

#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_event.h>

wifiStateConnected::wifiStateConnected(wifiStateDisconnected* next): disconnected(next)
{

}

void wifiStateConnected::enter(wifiContext& ctx, net_if* iface)
{
    MYLOG("📶 Connected! Holding...");
    MYLOG("📡 Maintaining connection");
}

void wifiStateConnected::handle(wifiContext& ctx, wifi_iface_status status)
{
    // MYLOG("📡 Maintaining connection");
    bool lostConnection = false; // Replace with actual logic
    if (lostConnection)
    {
        MYLOG("❌ Connection lost. Switching to Disconnected state...");
        ctx.setState(static_cast<wifiState*>(disconnected));
    }
    if (isDisconnectCalled)
    {
        MYLOG("❌ Disconnecting. Switching to Disconnected state...");

        ctx.setState(static_cast<wifiState*>(disconnected));

        struct net_if *iface = net_if_get_default();

        int ret = net_mgmt(NET_REQUEST_WIFI_DISCONNECT, iface, NULL, 0);
        if (ret)
        {
            MYLOG("WiFi Disconnection Request Failed\n");
        }
        isDisconnectCalled = false;
    }
}

int wifiStateConnected::name() const
{
    return static_cast<int>(CONNECTED);
}

void wifiStateConnected::setDisconnectCalled(bool value)
{
    isDisconnectCalled = value;
}

bool wifiStateConnected::getDisconnectCalled()
{
    return isDisconnectCalled;
}