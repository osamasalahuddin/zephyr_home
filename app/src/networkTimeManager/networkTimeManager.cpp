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

#include "networkTimeManager.hpp"

#include "myLogger.hpp"

#include <zephyr/net/sntp.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/posix/time.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include <cstring>

/* Initialize static members */
struct k_mutex      networkTimeManager::instance_mutex;
networkTimeManager* networkTimeManager::instance = nullptr;

networkTimeManager::networkTimeManager() : SYNC_INTERVAL(3600000), SNTP_SERVER("5.9.19.62")
{
    k_mutex_init(&state_mutex);
    k_mutex_init(&time_mutex);
    atomic_set(&sync_attempts, 0);
    atomic_set(&last_sync_error, 0);
    atomic_set(&is_syncing, false);
    atomic_set(&is_sync_active, false);
    atomic_set(&synced, false);
    atomic_set(&synced_time, 0);
    atomic_set(&last_uptime, 0);
}

networkTimeManager& networkTimeManager::getInstance()
{
    k_mutex_lock(&instance_mutex, K_FOREVER);
    if (!instance)
    {
        static networkTimeManager inst;
        instance = &inst;
    }
    k_mutex_unlock(&instance_mutex);
    return *instance;
}

bool networkTimeManager::init()
{
    k_mutex_init(&state_mutex);
    k_mutex_init(&instance_mutex);
    atomic_set(&synced, false);
    atomic_set(&last_uptime, 0);
    atomic_set(&synced_time, 0);
    reset_state();
    MYLOG("Network Time Manager initialized");
    return true;
}

const char* networkTimeManager::name() const
{
    return "Network Time Manager";
}

void networkTimeManager::tick()
{
    sync(SNTP_SERVER.c_str(), 5000);
}

void networkTimeManager::cleanup()
{
    k_mutex_lock(&state_mutex, K_FOREVER);
    reset_state();
    k_mutex_unlock(&state_mutex);
}

void networkTimeManager::reset_state()
{
    atomic_set(&synced, false);
    atomic_set(&last_uptime, 0);
    atomic_set(&synced_time, 0);
}

bool networkTimeManager::validate_server(const char* server)
{
    if (!server || strlen(server) == 0)
    {
        MYLOG("Invalid server address");
        return false;
    }
    return true;
}

void networkTimeManager::handle_error(const char* operation, int error_code)
{
    MYLOG("Error in %s: %d", operation, error_code);
    cleanup();
}

bool networkTimeManager::perform_sync(const char* server, int timeout_ms)
{
    struct sntp_ctx ctx = {};
    int             ret;

    /* Initialize address structure */
    struct sockaddr_in addr = {};
    addr.sin_family         = AF_INET;
    addr.sin_port           = htons(SNTP_PORT);

    struct sntp_time sntpTime;

    /* Convert server address */
    ret = inet_pton(AF_INET, server, &addr.sin_addr);
    if (ret != 1)
    {
        handle_error("inet_pton", ret);
        return false;
    }

    /* Initialize SNTP context */
    ret = sntp_init(&ctx, (struct sockaddr*) &addr, sizeof(struct sockaddr_in));
    if (ret < 0)
    {
        handle_error("sntp_init", ret);
        return false;
    }

    /* Query SNTP server */
    ret = sntp_query(&ctx, timeout_ms, &sntpTime);
    if (ret < 0)
    {
        handle_error("sntp_query", ret);
        sntp_close(&ctx);
        return false;
    }

    /* Update state with new time */
    k_mutex_lock(&state_mutex, K_FOREVER);
    atomic_set(&synced, true);
    atomic_set(&last_uptime, k_uptime_get());

    /* Log successful sync */
    struct tm time_info;
    time_t    time = sntpTime.seconds;
    gmtime_r(&time, &time_info);
    MYLOG("Time synced: %04d-%02d-%02d %02d:%02d:%02d", time_info.tm_year + 1900, time_info.tm_mon + 1,
          time_info.tm_mday, time_info.tm_hour, time_info.tm_min, time_info.tm_sec);

    if ((time_info.tm_mon + 1 > 3) && (time_info.tm_mon + 1 < 11))
    {
        /* Daylight Savings Time */
        atomic_set(&synced_time,
                   (((time_info.tm_hour + 2) * SEC_PER_HOUR) + (time_info.tm_min * SEC_PER_MIN) + time_info.tm_sec) *
                       MSEC_PER_SEC);
    }
    else
    {
        atomic_set(&synced_time,
                   (((time_info.tm_hour + 1) * SEC_PER_HOUR) + (time_info.tm_min * SEC_PER_MIN) + time_info.tm_sec) *
                       MSEC_PER_SEC);
    }

    k_mutex_unlock(&state_mutex);

    sntp_close(&ctx);
    return true;
}

bool networkTimeManager::sync(const char* server, int timeout_ms)
{
    /* Validate input */
    if (!validate_server(server))
    {
        return false;
    }

    /* Set default timeout if invalid */
    if (timeout_ms <= 0)
    {
        timeout_ms = SYNC_TIMEOUT;
    }

    /* Attempt sync with retries */
    int retry_count = 0;
    while (retry_count < MAX_RETRIES)
    {
        if (perform_sync(server, timeout_ms))
        {
            return true;
        }
        retry_count++;

        if (retry_count < MAX_RETRIES)
        {
            k_sleep(K_MSEC(RETRY_DELAY));
        }
    }

    return false;
}

bool networkTimeManager::is_synced() const
{
    return atomic_get(&synced);
}

int64_t networkTimeManager::get_network_time() const
{
    if (!is_synced())
    {
        return 0;
    }

    return atomic_get(&synced_time) + (k_uptime_get() - atomic_get(&last_uptime));
}

void networkTimeManager::convert_unix_time_to_date(uint64_t unix_time)
{
    struct tm time_info;
    time_t    time = unix_time;
    gmtime_r(&time, &time_info);

    MYLOG("Time: %04d-%02d-%02d %02d:%02d:%02d", time_info.tm_year + 1900, time_info.tm_mon + 1, time_info.tm_mday,
          time_info.tm_hour, time_info.tm_min, time_info.tm_sec);
}
