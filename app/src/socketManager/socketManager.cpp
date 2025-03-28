// app/src/socketStrategy/socketManager.cpp
#include "socketManager.hpp"
#include "socketStrategy.hpp"
#include <memory>

static socketManager* instance_ptr = nullptr;

socketManager& socketManager::instance()
{
    if (!instance_ptr) {
        static socketManager inst;
        instance_ptr = &inst;
    }
    return *instance_ptr;
}

bool socketManager::init(Protocol protocol, const std::string& host, uint16_t port)
{
    switch (protocol) {
        case Protocol::TCP:
            strategy = std::make_unique<tcpSocketStrategy>();
            break;
        case Protocol::UDP:
            strategy = std::make_unique<udpSocketStrategy>();
            break;
        case Protocol::TLS:
            strategy = std::make_unique<tlsSocketStrategy>();
            break;
        default:
            return false;
    }
    return strategy->connect(host, port);
}

ssize_t socketManager::send(const void* data, size_t len)
{
    if (strategy) {
        return strategy->send(data, len);
    }
    return -1;
}

ssize_t socketManager::receive(void* buffer, size_t maxLen)
{
    if (strategy) {
        return strategy->receive(buffer, maxLen);
    }
    return -1;
}

void socketManager::shutdown()
{
    if (strategy) {
        strategy->disconnect();
        strategy.reset();
    }
}
