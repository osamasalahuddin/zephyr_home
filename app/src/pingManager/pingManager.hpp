// app/src/net/pingManager.hpp
#pragma once

#include <zephyr/net/socket.h>
#include <zephyr/net/icmp.h>

#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>

#include <vector>
#include <string>
class pingManager
{
public:
    static pingManager& instance();

    void init();
    void send_ping(const char* ip, struct net_if *iface = nullptr);
    void tick();
    void cleanup();

    struct PingRequest
    {
        int64_t     start_time;
        std::string ip;
    };

private:
    /* Define a timeout threshold (e.g., 5000ms or 5 seconds) */
    const uint16_t PING_TIMEOUT_MS = 5000;
    std::vector<PingRequest> pending_requests;
    struct net_icmp_ctx icmp_ctx;



    pingManager() = default;
    static int handle_reply(struct net_icmp_ctx* ctx, struct net_pkt* pkt,
                            struct net_icmp_ip_hdr* ip_hdr,
                            struct net_icmp_hdr* icmp_hdr,
                            void* user_data);
    // int64_t get_ping_start_time() const;
};