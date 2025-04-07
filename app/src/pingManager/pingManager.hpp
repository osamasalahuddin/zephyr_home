// app/src/net/pingManager.hpp
#pragma once
#include "iManager.hpp"

#include <zephyr/net/socket.h>
#include <zephyr/net/icmp.h>

#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>

#include <vector>
#include <string>
#include <functional>
class pingManager : public iManager
{
public:
    /**
     * @brief Constructor for the pingManager class.
     */
    pingManager();

    /**
     * @brief Get the singleton instance of the pingManager class.
     * @return Reference to the singleton instance.
     */
    static pingManager& getInstance();


    void init() override;
    void tick() override;
    const char* name() const override;

    void send_ping(const char* ip, struct net_if *iface = nullptr, std::function<void(bool)> callback = nullptr);
    void cleanup();

    struct PingRequest
    {
        int64_t     start_time;
        std::string ip;
        std::function<void(bool)> callback;
    };

private:
    /* Define a timeout threshold (e.g., 5000ms or 5 seconds) */
    const uint16_t PING_TIMEOUT_MS = 5000;
    std::vector<PingRequest> pending_requests;
    struct net_icmp_ctx icmp_ctx;

    static int handle_reply(struct net_icmp_ctx* ctx, struct net_pkt* pkt,
                            struct net_icmp_ip_hdr* ip_hdr,
                            struct net_icmp_hdr* icmp_hdr,
                            void* user_data);
    // int64_t get_ping_start_time() const;
};