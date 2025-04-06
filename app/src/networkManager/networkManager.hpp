#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include "wifiManager.hpp"
#include "pingManager.hpp"

/**
 * @class networkManager
 * @brief Singleton class to manage network-related functionality.
 */
class networkManager
{
public:
    /**
     * @brief Get the singleton instance of the networkManager class.
     * @return Reference to the singleton instance.
     */
    static networkManager& instance();

    /**
     * @brief Initialize the network.
     */
    void init();

    /**
     * @brief Tick the network.
     */
    void tick();

private:

    /** Internal Variables */
    bool isConnectRequested;
    bool isNewConnect;
    wifiStateEnum state;

    int64_t start;
    uint8_t ticks;

    /** String Values gotten from build system for LAN and WAN Servers */
    const std::string CONFIG_MY_LOCAL;
    const std::string CONFIG_MY_REMOTE;

    /** WiFi instance */
    wifiManager wifi;

    /** PingManager instance pointer */
    pingManager& ping;

    /**
     * @brief Private constructor for the singleton pattern.
     */
    networkManager();

    /**
     * @brief Disable copy constructor.
     */
    networkManager(const networkManager&) = delete;

    /**
     * @brief Disable assignment operator.
     */
    networkManager& operator=(const networkManager&) = delete;

};

#endif /* NETWORK_MANAGER_HPP */

