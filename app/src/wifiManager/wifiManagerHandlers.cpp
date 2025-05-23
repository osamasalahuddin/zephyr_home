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
#include "wifiManager.hpp"
#include "myLogger.hpp"
#include "wifi.h"

static K_SEM_DEFINE(ipv4_address_obtained, 0, 1);

/* Wrappers for Semaphore to be used by Application */
void wifiManager::ipv4_sem_take(void)
{
    k_sem_take(&ipv4_address_obtained, K_FOREVER);
}

void wifiManager::ipv4_sem_give(void)
{
    k_sem_give(&ipv4_address_obtained);
}


void wifiManager::handle_wifi_scan_result(struct net_mgmt_event_callback *cb)
{
    const struct wifi_status *status = (const struct wifi_status *)cb->info;

    if (status->status)
    {
        MYLOG("Scan request failed (%d)", status->status);
    }
    else
    {
        MYLOG("Scan Handler: Scan Completed");
    }
    isScanComplete = true;
    // setScanComplete(true);
}

void wifiManager::handle_wifi_connect_result(struct net_mgmt_event_callback *cb)
{
    const struct wifi_status *status = (const struct wifi_status *)cb->info;

    if (status->status)
    {
        MYLOG("Connection request failed (%d)", status->status);
    }
    else
    {
        MYLOG("[Connect Handler]: Wifi Connected");
    }
}

void wifiManager::handle_wifi_disconnect_result(struct net_mgmt_event_callback *cb)
{
    const struct wifi_status *status = (const struct wifi_status *)cb->info;

    if (DISCONNECTED != getInstance().state)
    {
        if (status->status)
        {
            MYLOG("[Disconnect Handler] ❌ Wifi Disconnected without Disconnect being called Before");
            MYLOG("[Disconnect Handler] Reason: Disconnection request (%d)", status->disconn_reason);
            getInstance().context->setState(static_cast<wifiState *>(getInstance().error));
        }
    }
    else
    {
        MYLOG("[Disconnect Handler] ❌ Wifi Disconnected");
    }
}

void wifiManager::handle_ipv4_result(struct net_if *iface)
{
    int i = 0;

    if (NET_EVENT_IPV4_ADDR_DEL)
    MYLOG("IPv4 Handler: IPv4 Address Obtained");

    for (i = 0; i < NET_IF_MAX_IPV4_ADDR; i++)
    {

        char buf[NET_IPV4_ADDR_LEN];

        if (iface->config.ip.ipv4->unicast[i].ipv4.addr_type != NET_ADDR_DHCP)
        {
            continue;
        }

        MYLOG("IPv4 address: %s",
                net_addr_ntop(AF_INET,
                                &iface->config.ip.ipv4->unicast[i].ipv4.address.in_addr,
                                buf, sizeof(buf)));
        MYLOG("Subnet: %s",
                net_addr_ntop(AF_INET,
                                &iface->config.ip.ipv4->unicast[i].netmask,
                                buf, sizeof(buf)));
        MYLOG("Router: %s",
                net_addr_ntop(AF_INET,
                                &iface->config.ip.ipv4->gw,
                                buf, sizeof(buf)));

        connecting->setIsConnected(true);
    }

    k_sem_give(&ipv4_address_obtained);
}

void wifiManager::ipv4_mgmt_event_handler(struct net_mgmt_event_callback *cb,
                                    uint32_t mgmt_event,
                                    struct net_if *iface)
{
    MYLOG("[IPv4] Event Handler Called : 0x%08X", mgmt_event);
    MYLOG("[Legend] NET_EVENT_IPV4_ADDR_ADD : 0x%08lX", NET_EVENT_IPV4_ADDR_ADD);
    MYLOG("[Legend] NET_EVENT_IPV4_ADDR_DEL : 0x%08lX", NET_EVENT_IPV4_ADDR_DEL);

    if (mgmt_event && _NET_EVENT_IPV4_BASE)
    {
        wifiManager wifi = wifiManager::getInstance();

        if ((mgmt_event & (NET_EVENT_IPV4_CMD_ADDR_ADD |
                          NET_EVENT_IPV4_CMD_ADDR_DEL |
                          NET_EVENT_IPV4_CMD_MADDR_ADD ))
                        == (NET_EVENT_IPV4_CMD_ADDR_ADD |
                            NET_EVENT_IPV4_CMD_ADDR_DEL |
                            NET_EVENT_IPV4_CMD_MADDR_ADD ))
        {
            MYLOG("[IPv4] Event Handler: Still connecting to the network");
        }
        else if (mgmt_event & NET_EVENT_IPV4_CMD_ADDR_ADD)
        {
            MYLOG("[IPv4] ✅ IPv4 address added");
            wifi.handle_ipv4_result(iface);
        }
        else if (mgmt_event & NET_EVENT_IPV4_CMD_ADDR_DEL)
        {
            MYLOG("[IPv4] ❌ IPv4 address removed");
        }
        else if (mgmt_event & NET_EVENT_IPV4_CMD_MADDR_ADD)
        {
            MYLOG("[IPv4] ✅ IPv4 Multicast address added");
        }
        else if (mgmt_event & NET_EVENT_IPV4_CMD_MADDR_DEL)
        {
            MYLOG("[IPv4] ❌ IPv4 Multicast address removed");
        }
    }
}

void wifiManager::wifi_mgmt_event_handler(struct net_mgmt_event_callback *cb,
                                    uint32_t mgmt_event,
                                    struct net_if *iface)
{
    MYLOG("[Wifi] Event Handler Called : 0x%08X", mgmt_event);

    if (mgmt_event && _NET_WIFI_EVENT)
    {
        MYLOG("[Legend] NET_EVENT_WIFI_CONNECT_RESULT : 0x%08lX", NET_EVENT_WIFI_CONNECT_RESULT);
        MYLOG("[Legend] NET_EVENT_WIFI_SCAN_DONE : 0x%08lX", NET_EVENT_WIFI_SCAN_DONE);
        MYLOG("[Legend] NET_EVENT_WIFI_SCAN_RESULT : 0x%08lX", NET_EVENT_WIFI_SCAN_RESULT);
        MYLOG("[Legend] NET_EVENT_WIFI_DISCONNECT_RESULT : 0x%08lX", NET_EVENT_WIFI_DISCONNECT_RESULT);

        wifiManager instance = wifiManager::getInstance();

        if (mgmt_event & NET_EVENT_WIFI_CMD_CONNECT_RESULT)
        {
            instance.handle_wifi_connect_result(cb);
        }
        if (mgmt_event & NET_EVENT_WIFI_CMD_CONNECT_RESULT)
        {
            if (instance.getWifiState() == wifiStateEnum::CONNECTED)
            {
                /* Only Call if it is in connected state already */
                instance.handle_wifi_disconnect_result(cb);
            }
        }
        // if (mgmt_event & NET_EVENT_WIFI_CMD_SCAN_RESULT)
        // {
        //     handle_wifi_scan_result(cb);
        // }
        // if (mgmt_event & (NET_EVENT_WIFI_CMD_SCAN_DONE
        //                 | NET_EVENT_WIFI_CMD_SCAN_RESULT))
        // {
        //     handle_wifi_scan_result(cb);
        // }
    }

}

