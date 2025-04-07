// app/src/net/networkTimeManager.hpp
#pragma once

#include <string>

class networkTimeManager
{
public:
    static networkTimeManager& getInstance();

    void init();
    bool sync(const char* server, int timeout_ms = 5000);
    bool is_synced() const;

private:
    networkTimeManager() = default;

    bool synced = false;

    const uint8_t SNTP_PORT = 123;
};
