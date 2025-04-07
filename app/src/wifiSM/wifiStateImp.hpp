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
#include "wifiContext.hpp"

#include <zephyr/net/net_if.h>

class wifiStateIdle; // Forward declaration

// State: Disconnected
class wifiStateDisconnected : public wifiState
{
private:
    bool isConnectCalled = false;
    wifiStateIdle* idle;

public:
    wifiStateDisconnected(wifiStateIdle* next);
    void enter(wifiContext& ctx, net_if* iface) override;
    void handle(wifiContext& ctx, wifi_iface_status status) override;
    int name(void) const override;
    void setConnectCalled(bool value);
    bool getConnectCalled(void);
};

// State: Connected
class wifiStateConnected : public wifiState
{
private:
    bool isDisconnectCalled = false;
    wifiStateDisconnected* disconnected;

public:
    wifiStateConnected(wifiStateDisconnected* next);
    void enter(wifiContext& ctx, net_if* iface) override;
    void handle(wifiContext& ctx, wifi_iface_status status) override;
    int name(void) const override;
    void setDisconnectCalled(bool value);
    bool getDisconnectCalled(void);
};

// State: Connecting
class wifiStateConnecting : public wifiState
{
private:
    bool isConnectedCalled = false;
    bool isAssociated = false;
    bool isConnected = false;
    struct net_if* iface;
    wifiStateConnected* connected;
    wifiStateConnected* error;

public:
    wifiStateConnecting(wifiStateConnected* next);
    void enter(wifiContext& ctx, net_if* iface) override;
    void handle(wifiContext& ctx, wifi_iface_status status) override;
    int name(void) const override;
    void setIsConnected(bool value);
    void setConnectedCalled(bool value);
    bool getConnectedCalled(void);
};

// State: Idle
class wifiStateIdle : public wifiState
{
private:
    bool isConnectingCalled = false;
    struct net_if* iface;
    wifiStateConnecting* connecting;

public:
    wifiStateIdle(wifiStateConnecting* next);
    void enter(wifiContext& ctx, net_if* iface) override;
    void handle(wifiContext& ctx, wifi_iface_status status) override;
    int name(void) const override;
    void setConnectingCalled(bool value);
    bool getConnectingCalled(void);
};

// State: Error
class wifiStateError : public wifiState
{
private:
    wifiStateIdle* idle;
    wifiStateDisconnected* disconnected;

public:
    wifiStateError(wifiStateIdle* idleState,
                   wifiStateDisconnected* disconnectedState);
    void enter(wifiContext& ctx, net_if* iface) override;
    void handle(wifiContext& ctx, wifi_iface_status status) override;
    int name(void) const override;
};
