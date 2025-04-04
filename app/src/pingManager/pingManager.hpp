// app/src/net/pingManager.hpp
#pragma once

#include <zephyr/net/socket.h>
#include <zephyr/net/icmp.h>

#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>

class pingManager
{
public:
    static pingManager& instance();

    void init();
    void send_ping(const char* ip);
    void cleanup();

private:
    pingManager() = default;
    static int handle_reply(struct net_icmp_ctx* ctx, struct net_pkt* pkt,
                            struct net_icmp_ip_hdr* ip_hdr,
                            struct net_icmp_hdr* icmp_hdr,
                            void* user_data);

    int64_t ping_start_time;
    struct net_icmp_ctx icmp_ctx;
    int64_t get_ping_start_time() const;
};