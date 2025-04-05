#include "wifiManager.hpp"
#include "myLogger.h"
#include "wifi.h"

static K_SEM_DEFINE(ipv4_address_obtained, 0, 1);

extern wifiManager wifi;

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
        MYLOG("Connect Handler: Wifi Connected");
        wifi.connecting->setConnectedCalled(true);
    }
}

void wifiManager::handle_wifi_disconnect_result(struct net_mgmt_event_callback *cb)
{
    const struct wifi_status *status = (const struct wifi_status *)cb->info;

    if (DISCONNECTED != wifi.state)
    {
        if (status->status)
        {
            MYLOG("[Disconnect Handler] ❌ Wifi Disconnected without Disconnect being called Before");
            MYLOG("[Disconnect Handler] Reason: Disconnection request (%d)", status->disconn_reason);
            wifi.context->setState(static_cast<wifiState *>(wifi.error));
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
    MYLOG("IPv4 Handler: IPv4 Address Obtained");;

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
            MYLOG("✅ IPv4 address added");
            handle_ipv4_result(iface);
        }
        else if (mgmt_event & NET_EVENT_IPV4_CMD_ADDR_DEL)
        {
            MYLOG("❌ IPv4 address removed");
        }
        else if (mgmt_event & NET_EVENT_IPV4_CMD_MADDR_ADD)
        {
            MYLOG("✅ IPv4 Multicast address added");
        }
        else if (mgmt_event & NET_EVENT_IPV4_CMD_MADDR_DEL)
        {
            MYLOG("❌ IPv4 Multicast address removed");
        }
    }

    MYLOG("Raw event: 0x%08X", mgmt_event);
}

void wifiManager::wifi_mgmt_event_handler(struct net_mgmt_event_callback *cb,
                                    uint32_t mgmt_event,
                                    struct net_if *iface)
{
    MYLOG("Wifi Event Handler Called : 0x%08X", mgmt_event);

    if (mgmt_event && _NET_WIFI_EVENT)
    {
        MYLOG("[Legend] NET_EVENT_WIFI_CONNECT_RESULT : 0x%08lX", NET_EVENT_WIFI_CONNECT_RESULT);
        MYLOG("[Legend] NET_EVENT_WIFI_SCAN_DONE : 0x%08lX", NET_EVENT_WIFI_SCAN_DONE);
        MYLOG("[Legend] NET_EVENT_WIFI_SCAN_RESULT : 0x%08lX", NET_EVENT_WIFI_SCAN_RESULT);
        MYLOG("[Legend] NET_EVENT_WIFI_DISCONNECT_RESULT : 0x%08lX", NET_EVENT_WIFI_DISCONNECT_RESULT);

        if (mgmt_event & NET_EVENT_WIFI_CMD_CONNECT_RESULT)
        {
            wifi.handle_wifi_connect_result(cb);
        }
        if (mgmt_event & NET_EVENT_WIFI_CMD_CONNECT_RESULT)
        {
            wifi.handle_wifi_disconnect_result(cb);
        }
        if (mgmt_event & NET_EVENT_WIFI_CMD_SCAN_RESULT)
        {
            wifi.handle_wifi_scan_result(cb);
        }
        // switch (mgmt_event)
        // {
        //     case NET_EVENT_WIFI_CONNECT_RESULT:
        //         wifi.handle_wifi_connect_result(cb);
        //         break;
        //     case NET_EVENT_WIFI_SCAN_DONE:
        //     case NET_EVENT_WIFI_SCAN_RESULT:
        //         wifi.handle_wifi_scan_result(cb);
        //         break;
        //     case NET_EVENT_WIFI_DISCONNECT_RESULT:
        //         wifi.handle_wifi_disconnect_result(cb);
        //         break;
        //     default:
        //         break;
        // }
    }

}

