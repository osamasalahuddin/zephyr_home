// wifiManager.hpp
#pragma once
#include "iManager.hpp"

/* State Machine inlcudes */
#include "wifiContext.hpp"
#include "wifiStateImp.hpp"
#include "wifiManager.hpp"

/* Zephyr APIs */
#include <zephyr/net/net_if.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_event.h>

class wifiManager : public iManager
{
private:
    bool isConnecting = false;
    bool isError = false;
    bool isIpObtained = false;
    bool isScanComplete = false;

    wifiStateEnum state;

    wifiContext* context;
    wifiStateIdle* idle;
    wifiStateDisconnected* disconnected;
    wifiStateConnected* connected;
    wifiStateConnecting* connecting;
    wifiStateError* error;

    struct net_if *iface;

    void register_wifi_events();

    /* Handlers */
    void handle_wifi_scan_result(struct net_mgmt_event_callback *cb);
    void handle_wifi_connect_result(struct net_mgmt_event_callback *cb);
    void handle_wifi_disconnect_result(struct net_mgmt_event_callback *cb);
    void handle_ipv4_result(struct net_if *iface);

    /* Semaphore Wrappers */
    void ipv4_sem_take(void);
    void ipv4_sem_give(void);

    /* Event Handlers */
    static void ipv4_mgmt_event_handler(struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface);
    static void wifi_mgmt_event_handler(struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface);

    void setScanComplete(bool);
public:
    void init() override;
    void tick() override;
    const char* name() const override;

    /* Wifi Manager Functions */
    static wifiManager& getInstance();
    void reinit();
    void connect();
    void disconnect();
    void scan();
    wifiStateEnum wifi_status();
    wifiStateEnum getWifiState();
    wifi_iface_status get_wifi_status(struct net_if *iface);
    struct net_if * get_wifi_iface();
};
