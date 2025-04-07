#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include "iManager.hpp"
#include "wifiManager.hpp"
#include "pingManager.hpp"

/**
 * @class networkManager
 * @brief Singleton class to manage network-related functionality.
 */
class networkManager  : public iManager
{
public:
    /**
     * @brief Get the singleton instance of the networkManager class.
     * @return Reference to the singleton instance.
     */
    static networkManager& getInstance();

    /**
     * @brief Initialize the network.
     */
    void init();

    /**
     * @brief Tick the network.
     */
    void tick() override;

    /**
     * @brief Get the name of the manager.
     * @return Name of the manager.
     */
    const char* name() const override;

    /**
     * @brief Get if the network is connected to LAN.
     * @return True or False if connected to LAN.
     */
    bool isConnectedLAN();

    /**
     * @brief Get if the network is connected to WAN.
     * @return True or False if connected to WAN.
     */
    bool isConnectedWAN();

    /**
     * @brief Function to check if the network is up or not.
     * @return True if network is up, false otherwise.
     */
    bool isNetworkUp();
private:

    /** Internal Variables */

    /**
     * @brief Flag to check if the network is requested to connect.
     * @note This is set to true when the network manager is requested to connect
     */
    bool isConnectRequested;

    /**
     * @brief Flag to check if the network is in a new connection state.
     * @note This is set to true when the network manager is requested to connect
     */
    bool isNewConnect;

    /**
     * @brief Flag to check if the network is connected to LAN or not.
     * @note This is set to true when the pingManager receives a successful response
     */
    bool isLanConnected;

    /**
     * @brief Flag to check if the network is connected to WAN or not.
     * @note This is set to true when the pingManager receives a successful response
     */
    bool isWanConnected;

    /**
     * @brief State of the WiFi connection.
     * @note This is set to the current state of the WiFi connection
     *       using the wifiManager class.
     */
    wifiStateEnum wifiState;

    int64_t start;
    uint8_t ticks;

    /** String Values gotten from build system for LAN and WAN Servers */
    const std::string CONFIG_MY_LOCAL;
    const std::string CONFIG_MY_REMOTE;

    /** WiFi instance pointer */
    wifiManager& wifi;

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

    /**
     * @brief Callback that will be called by pingManager for Local Server Request.
     */
    static void setIsConnectedLAN(bool value);

    /**
     * @brief Callback that will be called by pingManager for Remote Server Request.
     */
    static void setIsConnectedWAN(bool value);
};

#endif /* NETWORK_MANAGER_HPP */

