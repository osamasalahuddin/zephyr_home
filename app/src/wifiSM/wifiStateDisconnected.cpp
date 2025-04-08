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

wifiStateDisconnected::wifiStateDisconnected(wifiStateIdle* next): idle(next)
{

}

void wifiStateDisconnected::enter(wifiContext& ctx, net_if* iface)
{
    MYLOG("❌ Disconnected. Awaiting reconnection...");
}

void wifiStateDisconnected::handle(wifiContext& ctx, wifi_iface_status status)
{
    // MYLOG("🔄 Trying to recover or restart connection");
    bool recoveryComplete = false; // Replace with real logic
    if (isConnectCalled)
    {
        ctx.setState(static_cast<wifiState*>(idle));
    }
    // if (isConnectCalled)
    // {
    //     MYLOG("🔗 Connecting. Switching to Connecting state...");
    //     ctx.setState(static_cast<wifiState*>(connecting));
    //     isConnectCalled = false;
    // }
}

int wifiStateDisconnected::name() const
{
    return static_cast<int>(DISCONNECTED);
}

void wifiStateDisconnected::setConnectCalled(bool value)
{
    isConnectCalled = value;
}

bool wifiStateDisconnected::getConnectCalled()
{
    return isConnectCalled;
}