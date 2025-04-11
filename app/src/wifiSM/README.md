# 📶 WiFi State Machine

This module implements a **State Design Pattern** to manage the WiFi lifecycle (Idle → Connecting → Connected → Disconnected/Error).

## 🧩 Design Pattern: State

Each WiFi state (Idle, Connecting, etc.) is a subclass of an abstract `wifiState` class and overrides `handle()` and `enter()`.

## 📚 Structure

- `wifiState.hpp`: Abstract base class
- `wifiStateImp.hpp/cpp`: Concrete state implementations
- `wifiContext.hpp`: Holds the current state and handles transitions

## 📌 Implementation Notes

- ctx.setState(newState) triggers transitions
- Each state encapsulates its logic and decides the next