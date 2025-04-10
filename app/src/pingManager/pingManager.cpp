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
#include <cstring>
#include <algorithm>

static pingManager* instance_ptr = nullptr;


pingManager::pingManager()
{
    /* Initialize the ICMP context */
    memset(&icmp_ctx, 0, sizeof(icmp_ctx));
}

pingManager& pingManager::getInstance()
{
    if (!instance_ptr)
    {
        static pingManager inst;
        instance_ptr = &inst;
    }
    return *instance_ptr;
}

void pingManager::init()
{
    if (0 != net_icmp_init_ctx(&icmp_ctx, NET_ICMPV4_ECHO_REPLY, 0, handle_reply))
    {
        MYLOG("❌ PingManager initialization failed");
    }
    else
    {
        MYLOG(" ✅ PingManager initialized");
    }
}

void pingManager::send_ping(const char* ip,
                            struct net_if *iface,
                            std::function<void(bool)> callback)
{
    struct sockaddr_in dst = {};
    struct net_icmp_ping_params params;

    if (iface == nullptr)
    {
        MYLOG("❌ Invalid Interface for sending Ping %s", ip);
    }
    else
    {
        int ret = inet_pton(AF_INET, ip, &dst.sin_addr);

        if (ret <= 0)
        {
            MYLOG("❌ Invalid IP address: %s", ip);
            return;
        }

        dst.sin_family = AF_INET;
        dst.sin_port = 0;

        ret = net_icmp_send_echo_request(&icmp_ctx, iface, (struct sockaddr*)&dst, &params, nullptr);
        if (ret == 0)
        {
            // MYLOG("🏓 Ping sent to %s", ip);
            pending_requests.push_back({k_uptime_get(), ip, callback});
        } else
        {
            MYLOG("❌ Failed to send ping to %s: %d", ip, ret);
        }
    }

}

void pingManager::tick()
{
    int64_t current_time = k_uptime_get();

    /* Iterate through pending requests and check for timeouts */
    auto it = pending_requests.begin();
    while (it != pending_requests.end())
    {
        if (current_time - it->start_time > PING_TIMEOUT_MS)
        {
            MYLOG("❌ Ping to %s timed out", it->ip.c_str());

            /* Call the callback if provided */
            if (it->callback)
            {
                it->callback(false);
            }

            /* Remove the timed-out request */
            it = pending_requests.erase(it);
        }
        else
        {
            /* Move to the next request */
            ++it;
        }
    }
}


const char* pingManager::name() const
{
    return "pingManager";
}

void pingManager::cleanup()
{
    net_icmp_cleanup_ctx(&icmp_ctx);
    MYLOG("Ping manager cleaned up");
}

int pingManager::handle_reply(struct net_icmp_ctx* ctx, struct net_pkt* pkt,
                              struct net_icmp_ip_hdr* ip_hdr,
                              struct net_icmp_hdr* icmp_hdr,
                              void* user_data)
{
    char addr_str[NET_IPV4_ADDR_LEN];
    net_addr_ntop(AF_INET, &ip_hdr->ipv4->src, addr_str, sizeof(addr_str));

    /* Find the matching request */
    auto it = std::find_if(instance_ptr->pending_requests.begin(), instance_ptr->pending_requests.end(),
                           [&](const PingRequest& req)
                            {
                                 return req.ip == addr_str;
                            }
                            );

    if (it != instance_ptr->pending_requests.end())
    {
        int64_t end_time = k_uptime_get() - it->start_time;
        // MYLOG("✅ Received ping reply from %s %lldms", addr_str, end_time);

        /* Call the callback if provided */
        if (it->callback)
        {
            it->callback(true);
        }

        /* Remove the completed request */
        instance_ptr->pending_requests.erase(it);
    }
    else
    {
        MYLOG("Received unexpected ping reply from %s", addr_str);
    }

    return 0;
}
