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
#include <cstring>

static networkTimeManager* instance_ptr = nullptr;

networkTimeManager& networkTimeManager::getInstance()
{
    if (!instance_ptr)
    {
        static networkTimeManager inst;
        instance_ptr = &inst;
    }
    return *instance_ptr;
}

void networkTimeManager::init()
{

    last_uptime = k_uptime_get();
    synced = false;

}

void networkTimeManager::convert_unix_time_to_date(uint64_t unix_time)
{
}

bool networkTimeManager::sync(const char* server, int timeout_ms)
{
    struct sntp_ctx ctx = {};
    int ret;
    bool return_value = false;


    std::string server_ = "5.9.19.62";
    server = server_.c_str();

    /* ipv4 */
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SNTP_PORT);

    ret = inet_pton(AF_INET, server_.c_str(), &addr.sin_addr);
    if (ret != 1)
    {
        MYLOG("Failed to convert address %d", ret);
    }

    ret = sntp_init(&ctx, (struct sockaddr *) &addr,sizeof(struct sockaddr_in));
    if (ret < 0)
    {
        MYLOG("Failed to init SNTP: %d", ret);
    }
    else
    {
        ret = sntp_query(&ctx, timeout_ms, &sntp_time);

        if (ret < 0)
        {
            MYLOG("SNTP request failed: %d", ret);
            sntp_close(&ctx);
            return false;
        }
        else
        {
            struct timespec ts;

            ts.tv_sec = sntp_time.seconds;
            ts.tv_nsec = ((uint64_t)sntp_time.fraction) >> 32;

            /* Get current uptime in milliseconds */
            last_uptime = k_uptime_get();
            synced = true;

            /* Convert NTP epoch to Unix epoch */
            uint64_t unix_time = sntp_time.seconds - 2208988800ULL;

            struct tm time_info;

            /* Convert Unix time to UTC (or local time if using localtime_r) */
            gmtime_r((time_t*)&unix_time, &time_info);

            if ((time_info.tm_mon + 1 > 3) && (time_info.tm_mon + 1 < 11))
            {
                /* Daylight Savings Time */
                synced_time = (((time_info.tm_hour + 2) * SEC_PER_HOUR) +
                                (time_info.tm_min * SEC_PER_MIN) +
                                time_info.tm_sec)
                                 * 1000U;
            }
            else
            {
                synced_time = (((time_info.tm_hour + 1) * SEC_PER_HOUR) +
                                (time_info.tm_min * SEC_PER_MIN) +
                                time_info.tm_sec)
                                 * 1000U;
            }

            MYLOG("Converted Time (UTC): %d-%02d-%02d %02d:%02d:%02d",
                                        (time_info.tm_year + 1970),
                                        (time_info.tm_mon + 1),
                                        (time_info.tm_mday - 1),
                                        time_info.tm_hour + 1,
                                        time_info.tm_min,
                                        time_info.tm_sec);

            sntp_close(&ctx);
            return_value = true;
        }
    }

    return return_value;
}

int64_t networkTimeManager::get_network_time()
{
    return (synced_time  + k_uptime_get() - last_uptime);
}

bool networkTimeManager::is_synced() const
{
    return synced;
}
