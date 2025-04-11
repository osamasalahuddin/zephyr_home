# 🌐 Network Manager

The central orchestrator of all network components.

## 🎯 Responsibilities

- Initializes WiFi, Ping, SNTP
- Manages network interface
- Registers socket protocols via `socketManager`

## 🔗 Dependencies

- `wifiManager`
- `networkTimeManager`
- `pingManager`
- `socketManager`