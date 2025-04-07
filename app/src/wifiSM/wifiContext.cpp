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
#include "wifiContext.hpp"

wifiContext::wifiContext(wifiState* initial, net_if* _iface) : state(initial)
{
    iface = _iface;
    state->enter(*this, iface);
}

void wifiContext::setState(wifiState* newState)
{
    state = newState;
    MYLOG("🔁 Transitioned to state: %s", getStateName());
    state->enter(*this, iface);
}

void wifiContext::update(wifi_iface_status status)
{
    if (state)
    {
        state->handle(*this, status);
    }
}

const char * wifiContext::getStateName()
{
    switch(state->name())
    {
        case IDLE:
            return "IDLE";
        case CONNECTING:
            return "CONNECTING";
        case CONNECTED:
            return "CONNECTED";
        case DISCONNECTED:
            return "DISCONNECTED";
        case ERROR:
            return "ERROR";
        default:
            return "ERROR";
    }
}

wifiStateEnum wifiContext::getState()
{
    return static_cast<wifiStateEnum>(state->name());
}