#include "wifiStateImp.hpp"
#include "myLogger.h"

wifiStateError::wifiStateError(wifiStateIdle* idleState,
                               wifiStateDisconnected* disconnectedState)
                            : idle(idleState), disconnected(disconnectedState)
{
    MYLOG("🛑 Initializing Error State");
    this->idle = idleState;
    MYLOG("🛑 Error state initialized");
}

void wifiStateError::enter(wifiContext& ctx, net_if* iface)
{
    MYLOG("🛑 Entered Error state");
}

void wifiStateError::handle(wifiContext& ctx, wifi_iface_status status)
{
    MYLOG("⚠️ Handling error... Going to Disconnected state");
    ctx.setState(static_cast<wifiState*>(this->disconnected));
    // Optionally retry or escalate
}

int wifiStateError::name() const
{
    return static_cast<int>(ERROR);
}

