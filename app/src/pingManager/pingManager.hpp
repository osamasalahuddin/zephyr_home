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

#pragma once
#include "iManager.hpp"

#include <zephyr/net/socket.h>
#include <zephyr/net/icmp.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/kernel.h>

#include <vector>
#include <string>
#include <functional>
#include <atomic>

class pingManager : public iManager
{
  public:
    /***** Local Type Definition *******/
    struct PingRequest
    {
        int64_t                   start_time;
        std::string               ip;
        std::function<void(bool)> callback;
    };

    /***** Constructors *******/
    /**
     * @brief Constructor for the pingManager class.
     */
    pingManager();

    /**
     * @brief Destructor for the pingManager class.
     */
    ~pingManager();

    /**
     * @brief Get the singleton instance of the pingManager class.
     * @return Reference to the singleton instance.
     */
    static pingManager& getInstance();

    /* Delete copy constructor and assignment operator */
    pingManager(const pingManager&)            = delete;
    pingManager& operator=(const pingManager&) = delete;

    /***** Overriden Virtual Functions from base class *******/

    /**
     * @brief Initialization Function of the pingManager class.
     * @return true if initialization was successful, false otherwise.
     */
    bool init() override;

    /**
     * @brief Periodic Tick function of the pingManager class.
     * @note This function should be called periodically to handle ping requests.
     */
    void tick() override;

    /**
     * @brief Name of the manager Class.
     * @return Returns the string lateral of the current Manager Class.
     */
    const char* name() const override;

    /***** Class Functions *******/

    /**
     * @brief Send ping on the specified IP address.
     * @param const char* ip - string ip address.
     * @param struct net_if *iface - network interface to use.
     * @param std::function<void(bool)> callback - callback function to handle the result.
     * @return true if ping request was sent successfully, false otherwise.
     */
    bool send_ping(const char* ip, struct net_if* iface = nullptr, std::function<void(bool)> callback = nullptr);

    /**
     * @brief Cleanup the pingManager class.
     * @note This function should be called to clean up resources used by the pingManager.
     */
    void cleanup();

  private:
    /**** Private Members ******/
    static struct k_mutex instance_mutex;
    static pingManager*   instance_ptr;
    struct k_mutex        request_mutex;
    std::atomic<bool>     is_initialized{false};

    /**
     * Define a timeout threshold (e.g., 5000ms or 5 seconds) for ping requests.
     */
    const uint16_t PING_TIMEOUT_MS = 5000;

    /**
     * Vector to store pending ping requests.
     */
    std::vector<PingRequest> pending_requests;

    /**
     * ICMP context for ping operations.
     */
    struct net_icmp_ctx icmp_ctx;

    /**** Private Functions ******/
    /**
     * @brief Process the ping reply.
     * @param ctx ICMP context used in this request.
     * @param pkt Received ICMP response network packet.
     * @param ip_hdr IP header of the packet.
     * @param icmp_hdr ICMP header of the packet.
     * @return 0 on success, negative error code otherwise.
     */
    static int handle_reply(struct net_icmp_ctx* ctx, struct net_pkt* pkt, struct net_icmp_ip_hdr* ip_hdr,
                            struct net_icmp_hdr* icmp_hdr, void* user_data);

    /**
     * @brief Validate the network interface.
     * @param iface Network interface to validate.
     * @return true if interface is valid, false otherwise.
     */
    bool validate_interface(struct net_if* iface);

    /**
     * @brief Validate the IP address.
     * @param ip IP address to validate.
     * @return true if IP is valid, false otherwise.
     */
    bool validate_ip(const char* ip);
};