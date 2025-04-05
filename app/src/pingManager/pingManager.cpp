// app/src/net/pingManager.cpp
#include "pingManager.hpp"
#include "myLogger.h"
#include <cstring>
#include <algorithm>

static pingManager* instance_ptr = nullptr;

pingManager& pingManager::instance()
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

void pingManager::send_ping(const char* ip, struct net_if *iface)
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

        // struct net_if* iface = net_if_get_default();
        ret = net_icmp_send_echo_request(&icmp_ctx, iface, (struct sockaddr*)&dst, &params, nullptr);
        if (ret == 0)
        {
            MYLOG("✅ Ping sent to %s", ip);
            pending_requests.push_back({k_uptime_get(), ip});
        } else
        {
            MYLOG("❌ Failed to send ping to %s: %d", ip, ret);
        }
    }

}

void pingManager::tick()
{
    int64_t current_time = k_uptime_get();

    // Iterate through pending requests and check for timeouts
    auto it = pending_requests.begin();
    while (it != pending_requests.end())
    {
        if (current_time - it->start_time > PING_TIMEOUT_MS)
        {
            MYLOG("❌ Ping to %s timed out", it->ip.c_str());
            it = pending_requests.erase(it); // Remove the timed-out request
        }
        else
        {
            ++it; // Move to the next request
        }
    }
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
        MYLOG("Received ping reply from %s %lldms", addr_str, end_time);
        /* Remove the completed request */
        instance_ptr->pending_requests.erase(it);
    }
    else
    {
        MYLOG("Received unexpected ping reply from %s", addr_str);
    }

    return 0;
}
