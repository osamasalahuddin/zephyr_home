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
#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/net/sntp.h>
#include <zephyr/net/net_config.h>
#include <vector>
#include <string>

/**
 * @class networkTimeManager
 * @brief Singleton class to manage network time synchronization and distribution.
 *
 * This class handles:
 * - SNTP time synchronization with network time servers
 * - Distribution of synchronized time to observers
 * - Thread-safe singleton pattern implementation
 * - Error handling and retry mechanisms
 */
class networkTimeManager : public iManager
{
  public:
    /**
     * @brief Get the singleton instance of the networkTimeManager class.
     * @return Reference to the singleton instance.
     */
    static networkTimeManager& getInstance();

    /**
     * @brief Initialize the network time manager.
     * @return true if initialization was successful, false otherwise.
     */
    bool init() override;

    /**
     * @brief Perform periodic time synchronization and updates.
     */
    void tick() override;

    /**
     * @brief Get the name of the manager.
     * @return Name of the manager.
     */
    const char* name() const override;

    /**
     * @brief Get the current time in timespec format.
     * @return Current time as timespec.
     */
    struct timespec getCurrentTime() const;

    /**
     * @brief Get the current time in milliseconds since epoch.
     * @return Current time in milliseconds.
     */
    int64_t getCurrentTimeMs() const;

    /**
     * @brief Get the current time in seconds since epoch.
     * @return Current time in seconds.
     */
    time_t getCurrentTimeSec() const;

    /**
     * @brief Get the current time in nanoseconds.
     * @return Current time in nanoseconds.
     */
    int64_t getCurrentTimeNs() const;

    /**
     * @brief Get the current time in microseconds.
     * @return Current time in microseconds.
     */
    int64_t getCurrentTimeUs() const;

    /**
     * @brief Check if time synchronization is active.
     * @return true if time sync is active, false otherwise.
     */
    bool isTimeSyncActive() const;

    /**
     * @brief Get the number of time synchronization attempts.
     * @return Number of sync attempts.
     */
    uint32_t getSyncAttempts() const;

    /**
     * @brief Get the last time synchronization error.
     * @return Last sync error code.
     */
    int getLastSyncError() const;

    /**
     * @brief Synchronize time with SNTP server.
     * @param server SNTP server address.
     * @param timeout_ms Timeout in milliseconds.
     * @return true if sync was successful, false otherwise.
     */
    bool sync(const char* server, int timeout_ms);

    bool is_synced() const;

    int64_t get_network_time() const;

    /**
     * @brief Convert Unix timestamp to human-readable date format.
     * @param unix_time Unix timestamp to convert.
     */
    void convert_unix_time_to_date(uint64_t unix_time);

  private:
    /* Internal Variables */
    static struct k_mutex      instance_mutex;       /**< Mutex for singleton instance protection */
    static networkTimeManager* instance;             /**< Singleton instance */
    struct k_mutex             state_mutex;          /**< Mutex for state protection */
    struct k_mutex             time_mutex;           /**< Mutex for time data protection */
    atomic_t                   sync_attempts;        /**< Counter for sync attempts */
    atomic_t                   last_sync_error;      /**< Last sync error code */
    atomic_t                   is_syncing;           /**< Flag indicating sync in progress */
    atomic_t                   is_sync_active;       /**< Flag indicating sync is active */
    atomic_t                   synced;               /**< Flag indicating if time is synced */
    atomic_t                   synced_time;          /**< Last synced time in milliseconds */
    atomic_t                   last_uptime;          /**< Last uptime when time was synced */
    bool                       m_initialized{false}; /**< Initialization state flag */

    /**
     * @brief Time between sync attempts in milliseconds.
     * @note This is set to 1 hour (3600000ms)
     */
    const uint32_t SYNC_INTERVAL;

    /**
     * @brief Maximum number of sync retries.
     * @note This is set to 3 attempts
     */
    static constexpr int MAX_RETRIES = 3;

    /**
     * @brief Delay between retries in milliseconds.
     * @note This is set to 1 second (1000ms)
     */
    static constexpr int RETRY_DELAY = 1000;

    /**
     * @brief Timeout for sync operations in milliseconds.
     * @note This is set to 5 seconds (5000ms)
     */
    const uint32_t SYNC_TIMEOUT = 5000;

    /**
     * @brief SNTP server address.
     * @note This is set to "pool.ntp.org"
     */
    const std::string SNTP_SERVER;

    /**
     * @brief SNTP port.
     * @note This is set to the standard SNTP port (123)
     */
    static constexpr int SNTP_PORT = 123;

    /**
     * @brief Current system time in timespec format.
     */
    struct timespec current_time;

    /**
     * @brief Private constructor for the singleton pattern.
     */
    networkTimeManager();

    /**
     * @brief Disable copy constructor.
     */
    networkTimeManager(const networkTimeManager&) = delete;

    /**
     * @brief Disable assignment operator.
     */
    networkTimeManager& operator=(const networkTimeManager&) = delete;

    /**
     * @brief Cleanup the network time manager.
     */
    void cleanup();

    /**
     * @brief Reset the state of the network time manager.
     */
    void reset_state();

    /**
     * @brief Perform time synchronization with SNTP server.
     * @param server SNTP server address.
     * @param timeout_ms Timeout in milliseconds.
     * @return true if sync was successful, false otherwise.
     */
    bool perform_sync(const char* server, int timeout_ms);

    /**
     * @brief Notify all observers of time updates.
     * @param new_time The new time to notify observers about.
     */
    void notifyObservers(const struct timespec& new_time);

    /**
     * @brief Update internal time state.
     * @param new_time The new time to set.
     */
    void updateTime(const struct timespec& new_time);

    /**
     * @brief Validate SNTP server configuration.
     * @return true if configuration is valid, false otherwise.
     */
    bool validateServerConfig() const;

    /**
     * @brief Handle time synchronization errors.
     * @param operation Operation that failed.
     * @param error_code Error code.
     */
    void handle_error(const char* operation, int error_code);

    /**
     * @brief Check if it's time to perform a sync.
     * @return true if sync should be performed, false otherwise.
     */
    bool shouldSync() const;

    /**
     * @brief Reset sync state and counters.
     */
    void resetSyncState();

    /**
     * @brief Validate SNTP server address.
     * @param server Server address to validate.
     * @return true if valid, false otherwise.
     */
    bool validate_server(const char* server);
};
