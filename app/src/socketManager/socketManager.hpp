// app/src/socketStrategy/socketManager.hpp
#pragma once

#include "socketStrategy.hpp"
#include <memory>
#include <string>

class socketManager
{
public:
    enum class Protocol
    {
        TCP,
        UDP,
        TLS
    };

    static socketManager& instance();

    bool init(Protocol protocol, const std::string& host, uint16_t port);
    ssize_t send(const void* data, size_t len);
    ssize_t receive(void* buffer, size_t maxLen);
    void shutdown();

private:
    socketManager() = default;
    std::unique_ptr<socketStrategy> strategy;
};
