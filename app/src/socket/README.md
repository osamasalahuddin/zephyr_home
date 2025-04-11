# 🧱 Sockets (Wrapper)

A lightweight wrapper for using socketManager per-module.

## 🧩 Responsibilities

- Stores protocol, host, port
- Calls `socketManager::instance().send(...)`
- Simple `open()`, `send()`, `close()` API

This decouples modules from knowing socket internals.