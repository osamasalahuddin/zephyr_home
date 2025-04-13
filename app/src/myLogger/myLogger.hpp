/*
 * This file is part of the Zephyr Home project.
 *
 * Copyright (C) 2025 Osama Salah-ud-Din
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

#include <zephyr/kernel.h>
#include <string.h>
#include <iostream>

#include "networkTimeManager.hpp"

/*
    Logging macro with formatted uptime
    and trimmed file path (removes top 2 folders)
*/
#define LOG_MSG_LENGTH      512
// #define NET_LOG_MSG_LENGTH 1024

// static char __netlog_msg[NET_LOG_MSG_LENGTH];
extern char __mylog_msg[LOG_MSG_LENGTH];

class myLogger
{
public:
    static myLogger& getInstance();
    void init();
    void send(const char* log_msg, size_t len);
    bool isSocket;
};

#define MYLOG(fmt, ...) do {                                            \
    /* Get full path from __FILE__ */                                   \
    const char* __full_path = __FILE__;                                 \
    const char* __short_file = __full_path;                             \
                                                                        \
    /* Strip top 2 folders from path */                                 \
    int __slashes = 0;                                                  \
    for (const char* p = __full_path; *p; ++p) {                        \
        if (*p == '/') {                                                \
            __slashes++;                                                \
            if (__slashes == 2) __short_file = p + 1;                   \
        }                                                               \
    }                                                                   \
    int64_t __uptime_ms;                                                \
    /* Get uptime in milliseconds */                                    \
    if(networkTimeManager::getInstance().is_synced()) {                 \
        __uptime_ms = networkTimeManager::getInstance().get_network_time();\
    }                                                                   \
    else{                                                               \
        __uptime_ms = k_uptime_get();                                   \
    }                                                                   \
                                                                        \
                                                                        \
    /* Convert to hh:mm:ss.mmm format */                                \
    int64_t __hours = __uptime_ms / (1000 * 60 * 60);                   \
    int64_t __minutes = (__uptime_ms / (1000 * 60)) % 60;               \
    int64_t __seconds = (__uptime_ms / 1000) % 60;                      \
    int64_t __milliseconds = __uptime_ms % 1000;                        \
                                                                        \
    /* Format and print the log message */                              \
    int __len = snprintf(__mylog_msg, sizeof(__mylog_msg),              \
        "[%02lld:%02lld:%02lld.%03lld] %s:%d - " fmt ,                  \
        __hours, __minutes, __seconds, __milliseconds,                  \
        __short_file, __LINE__, ##__VA_ARGS__);                         \
    printk("%s\n", __mylog_msg);                                        \
    myLogger::getInstance().send(__mylog_msg, __len);                   \
        \
} while (0);
