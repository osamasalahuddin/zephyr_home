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

#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_event.h>

#include "wifiStateImp.hpp"
#include "wifiContext.hpp"
#include "myLogger.hpp"


wifiStateConnecting::wifiStateConnecting(wifiStateConnected* next): connected(next)
{

}

void wifiStateConnecting::enter(wifiContext& ctx, net_if* _iface)
{
    MYLOG("🔗 Entered Connecting state");
    iface = _iface;
    isAssociated = false;
    isConnected = false;

    /* Take the SSID and Password from Environment Variables. */
    const std::string CONFIG_WIFI_SSID(WIFI_SSID);
    const std::string CONFIG_WIFI_PASSWORD(WIFI_PASSWORD);

    struct wifi_connect_req_params params =
    {
        .ssid = (const uint8_t*) CONFIG_WIFI_SSID.c_str(),
        .ssid_length = (uint8_t) CONFIG_WIFI_SSID.length(),
        .psk = (const uint8_t*) CONFIG_WIFI_PASSWORD.c_str(),
        .psk_length = (uint8_t) CONFIG_WIFI_PASSWORD.length(),
        .security = WIFI_SECURITY_TYPE_PSK,
    };

    MYLOG("Sending Connection Request");
    int ret = net_mgmt(NET_REQUEST_WIFI_CONNECT, iface, &params, sizeof(params));

    if (ret)
    {
        MYLOG("Failed to connect to WiFi network! [Error]:%d", ret);
        ctx.setState(static_cast<wifiState*>(error));
    }
    else
    {
        MYLOG("🔗 Connecting to Wi-Fi [SSID]: %s", CONFIG_WIFI_SSID.c_str());
    }
}

void wifiStateConnecting::handle(wifiContext& ctx, wifi_iface_status status)
{
    if (status.state >= WIFI_STATE_ASSOCIATED && !isAssociated)
    {
        MYLOG("WiFi is Associated");
        isAssociated = true;
    }

    if (status.state == WIFI_STATE_COMPLETED && isAssociated)
    {
        if (isConnected)
        {
            isConnectedCalled = false;
            MYLOG("✅ Connected. Switching to Connected state...");
            ctx.setState(static_cast<wifiState*>(connected));
        }
        else
        {

        }

        // /* Check for IP address assignment */
        // struct net_if_ipv4 *ipv4 = net_if_get_config(iface)->ip.ipv4;

        // if (ipv4 && ipv4->unicast[0].ipv4.is_used)
        // {
        //     struct in_addr addr = ipv4->unicast[0].ipv4.address.in_addr;
        //     char ip_str[NET_IPV4_ADDR_LEN];

        //     net_addr_ntop(AF_INET, &addr, ip_str, sizeof(ip_str));
        //     MYLOG("✅ IP address assigned: %s", ip_str);

        //     MYLOG("✅ Connected. Switching to Connected state...");
        //     isConnectedCalled = false;
        //     ctx.setState(static_cast<wifiState*>(connected));
        // }
        // else
        // {
        //     // MYLOG("IP address not assigned yet");
        // }
    }
}

int wifiStateConnecting::name() const
{
    return static_cast<int>(CONNECTING);
}

void wifiStateConnecting::setIsConnected(bool value)
{
    isConnected = value;
}

void wifiStateConnecting::setConnectedCalled(bool value)
{
    isConnectedCalled = value;
}

bool wifiStateConnecting::getConnectedCalled()
{
    return isConnectedCalled;
}