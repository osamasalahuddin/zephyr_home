// app/src/socketStrategy/socketStrategy.hpp
#pragma once

#include <zephyr/net/socket.h>
#include <string>

class socketStrategy
{
public:
    virtual bool connect(const std::string& host, uint16_t port) = 0;
    virtual ssize_t send(const void* data, size_t len) = 0;
    virtual ssize_t receive(void* buffer, size_t maxLen) = 0;
    virtual void disconnect() = 0;
    virtual ~socketStrategy() = default;
};

class tcpSocketStrategy : public socketStrategy
{
private:
    int sock = -1;

public:
    bool connect(const std::string& host, uint16_t port) override;
    ssize_t send(const void* data, size_t len) override;
    ssize_t receive(void* buffer, size_t maxLen) override;
    void disconnect() override;
};

class udpSocketStrategy : public socketStrategy
{
private:
    int sock = -1;
    struct sockaddr_in dest = {};

public:
    bool connect(const std::string& host, uint16_t port) override;
    ssize_t send(const void* data, size_t len) override;
    ssize_t receive(void* buffer, size_t maxLen) override;
    void disconnect() override;
};

class tlsSocketStrategy : public socketStrategy
{
private:
    int sock = -1;

public:
    bool connect(const std::string& host, uint16_t port) override;
    ssize_t send(const void* data, size_t len) override;
    ssize_t receive(void* buffer, size_t maxLen) override;
    void disconnect() override;
};
