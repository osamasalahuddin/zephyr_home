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

#include "pingManager.hpp"
#include "myLogger.hpp"

#include <zephyr/net/net_core.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/icmp.h>
#include <zephyr/kernel.h>

/* Initialize static members */
struct k_mutex pingManager::instance_mutex;
pingManager*   pingManager::instance_ptr = nullptr;

pingManager::pingManager()
{
    k_mutex_init(&request_mutex);
    is_initialized = false;
}

pingManager::~pingManager()
{
    cleanup();
}

pingManager& pingManager::getInstance()
{
    if (instance_ptr == nullptr)
    {
        k_mutex_lock(&instance_mutex, K_FOREVER);
        if (instance_ptr == nullptr)
        {
            instance_ptr = new pingManager();
        }
        k_mutex_unlock(&instance_mutex);
    }
    return *instance_ptr;
}

bool pingManager::init()
{
    if (is_initialized)
    {
        return true;
    }

    k_mutex_lock(&request_mutex, K_FOREVER);

    /* Initialize ICMP context */
    int ret = net_icmp_init_ctx(&icmp_ctx, NET_ICMPV4_ECHO_REPLY, 0, handle_reply);
    if (ret < 0)
    {
        MYLOG("❌ PingManager initialization failed: %d", ret);
        k_mutex_unlock(&request_mutex);
        return false;
    }

    is_initialized = true;
    MYLOG("✅ PingManager initialized");
    k_mutex_unlock(&request_mutex);
    return true;
}

void pingManager::tick()
{
    if (!is_initialized)
    {
        return;
    }

    k_mutex_lock(&request_mutex, K_FOREVER);

    int64_t current_time = k_uptime_get();
    auto    it           = pending_requests.begin();

    while (it != pending_requests.end())
    {
        if (current_time - it->start_time > PING_TIMEOUT_MS)
        {
            MYLOG("❌ Ping to %s timed out", it->ip.c_str());
            if (it->callback)
            {
                it->callback(false);
            }
            it = pending_requests.erase(it);
        }
        else
        {
            ++it;
        }
    }

    k_mutex_unlock(&request_mutex);
}

const char* pingManager::name() const
{
    return "pingManager";
}

bool pingManager::send_ping(const char* ip, struct net_if* iface, std::function<void(bool)> callback)
{
    if (!is_initialized)
    {
        MYLOG("❌ Ping manager not initialized");
        return false;
    }

    if (!validate_ip(ip))
    {
        MYLOG("❌ Invalid IP address: %s", ip);
        return false;
    }

    if (!validate_interface(iface))
    {
        MYLOG("❌ Invalid network interface");
        return false;
    }

    struct sockaddr addr;
    int             ret = net_ipaddr_parse(ip, strlen(ip), &addr);
    if (ret < 0)
    {
        MYLOG("❌ Failed to parse IP address: %s", ip);
        return false;
    }

    k_mutex_lock(&request_mutex, K_FOREVER);

    PingRequest request;
    request.start_time = k_uptime_get();
    request.ip         = ip;
    request.callback   = callback;

    pending_requests.push_back(request);

    ret = net_icmp_send_echo_request(&icmp_ctx, iface, &addr, nullptr, nullptr);
    if (ret < 0)
    {
        MYLOG("❌ Failed to send ping request: %d", ret);
        pending_requests.pop_back();
        k_mutex_unlock(&request_mutex);
        return false;
    }

    /*  MYLOG("🏓 Ping sent to %s", ip); */

    k_mutex_unlock(&request_mutex);
    return true;
}

void pingManager::cleanup()
{
    if (!is_initialized)
    {
        return;
    }

    k_mutex_lock(&request_mutex, K_FOREVER);

    /* Notify all pending requests of failure */
    for (auto& request : pending_requests)
    {
        if (request.callback)
        {
            request.callback(false);
        }
    }

    pending_requests.clear();
    net_icmp_cleanup_ctx(&icmp_ctx);
    is_initialized = false;

    MYLOG("Ping manager cleaned up");
    k_mutex_unlock(&request_mutex);
}

int pingManager::handle_reply(struct net_icmp_ctx* ctx, struct net_pkt* pkt, struct net_icmp_ip_hdr* ip_hdr,
                              struct net_icmp_hdr* icmp_hdr, void* user_data)
{
    char addr_str[NET_IPV4_ADDR_LEN];
    net_addr_ntop(AF_INET, &ip_hdr->ipv4->src, addr_str, sizeof(addr_str));

    pingManager& manager = getInstance();

    k_mutex_lock(&manager.request_mutex, K_FOREVER);

    auto it = std::find_if(manager.pending_requests.begin(), manager.pending_requests.end(),
                           [&](const PingRequest& req) { return req.ip == addr_str; });

    if (it != manager.pending_requests.end())
    {
        int64_t end_time = k_uptime_get() - it->start_time;
        MYLOG("✅ Received ping reply from %s %lldms", addr_str, end_time);

        if (it->callback)
        {
            it->callback(true);
        }
        manager.pending_requests.erase(it);
    }
    else
    {
        MYLOG("Received unexpected ping reply from %s", addr_str);
    }

    k_mutex_unlock(&manager.request_mutex);
    return 0;
}

bool pingManager::validate_interface(struct net_if* iface)
{
    if (iface == nullptr)
    {
        iface = net_if_get_default();
    }

    if (iface == nullptr)
    {
        MYLOG("❌ No network interface available");
        return false;
    }

    if (!net_if_is_up(iface))
    {
        MYLOG("❌ Network interface is down");
        return false;
    }

    return true;
}

bool pingManager::validate_ip(const char* ip)
{
    if (ip == nullptr || strlen(ip) == 0)
    {
        return false;
    }

    struct sockaddr addr;
    return net_ipaddr_parse(ip, strlen(ip), &addr);
}
