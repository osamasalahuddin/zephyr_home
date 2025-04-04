// app/src/net/pingManager.cpp
#include "pingManager.hpp"
#include "myLogger.h"
#include <cstring>

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
    net_icmp_init_ctx(&icmp_ctx, NET_ICMPV4_ECHO_REPLY, 0, handle_reply);
    MYLOG("Ping manager initialized");
}

void pingManager::send_ping(const char* ip)
{
    struct sockaddr_in dst = {};
    struct net_icmp_ping_params params;

    dst.sin_family = AF_INET;
    dst.sin_port = 0;
    inet_pton(AF_INET, ip, &dst.sin_addr);

    struct net_if* iface = net_if_get_default();
    int ret = net_icmp_send_echo_request(&icmp_ctx, iface, (struct sockaddr*)&dst, &params, nullptr);
    if (ret == 0)
    {
        MYLOG("✅ Ping sent to %s", ip);
        ping_start_time = k_uptime_get();
    } else
    {
        MYLOG("❌ Failed to send ping to %s: %d", ip, ret);
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
    int64_t end_time = k_uptime_get() - instance_ptr->get_ping_start_time();
    MYLOG("Received ping reply from %s %lldms", addr_str, end_time);
    // MYLOG("Received ping reply from %lldms", end_time);

    return 0;
}

int64_t pingManager::get_ping_start_time() const
{
    return ping_start_time;
}
