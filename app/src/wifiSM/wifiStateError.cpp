#include "wifiStateImp.hpp"
#include "myLogger.h"

wifiStateError::wifiStateError(wifiStateIdle* idleState)
{

}

void wifiStateError::enter(wifiContext& ctx, net_if* iface)
{
    MYLOG("🛑 Entered Error state");
}

void wifiStateError::handle(wifiContext& ctx, wifi_iface_status status)
{
    MYLOG("⚠️ Handling error... Going to IDLE ");
    ctx.setState(static_cast<wifiState*>(idle));
    // Optionally retry or escalate
}

int wifiStateError::name() const
{
    return static_cast<int>(ERROR);
}

