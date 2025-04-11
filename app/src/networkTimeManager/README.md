# 🕒 Network Time Manager

Syncs time from SNTP server using Zephyr’s built-in API.

## 🧩 Dependencies

- `networkManager` for interface
- SNTP Zephyr API

## 🔄 Flow

- Resolves DNS
- Sends SNTP query
- Applies system time