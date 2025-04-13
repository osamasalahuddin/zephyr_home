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

#include <string>
#include <vector>
#include <algorithm>

#include <zephyr/net/sntp.h>
// #include "../systemTime/timeObserver.hpp"

class networkTimeManager
{
public:
    static networkTimeManager& getInstance();

    void init();
    bool sync(const char* server, int timeout_ms = 5000);
    bool is_synced() const;
    int64_t get_network_time();
    // void addObserver(timeObserver* observer) {
        //     observers.push_back(observer);
        // }

        // void removeObserver(timeObserver* observer) {
            //     // observers.erase(observer);
            //     /* observers.erase(
                //         std::remove(observers.begin(), observers.end(), observer),
                //         observers.end()
                //     );*/
                // }

                // void notifyObservers(const struct timespec& newTime) {
                    //     for (auto observer : observers) {
    //         observer->onTimeUpdate(newTime);
    //     }
    // }

    private:
    networkTimeManager() = default;
    void convert_unix_time_to_date(uint64_t unix_time);

    bool synced = false;

    const uint8_t SNTP_PORT = 123;

    /**
     * @brief Value of last system uptime before network sync
     */
    int64_t last_uptime;

    struct sntp_time sntp_time;

    int64_t synced_time;

    // std::vector<timeObserver*> observers;
    static networkTimeManager* instance;
};
