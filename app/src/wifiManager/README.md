# 📡 WiFi Manager

This module manages the overall WiFi connection process and delegates connection logic to the `wifiStateMachine`.

## 💡 Responsibilities

- Starts the connection via `connect()`
- Uses `wifiStateMachine.setState()` to drive transitions
- Registers callbacks for connection events

## 🔄 Flow Overview

1. `connect()` triggers transition to Connecting
2. Callback triggers success/failure
3. WiFiManager observes and reacts

## 📶 WiFi Manager – State Transition Diagram

```mermaid

stateDiagram-v2
    [*] --> Idle : system startup
    Idle --> Connecting : wifiManager.connect()

    Connecting --> Connected : success
    Connecting --> Error : failure

    Error --> Idle : retry / reset

    Connected --> Disconnected : signal lost / wifiManager.disconnect()

    Disconnected --> Connecting : auto reconnect
    Disconnected --> Idle : user abort

    %% Style Definitions
    classDef success fill:#E0FFE0,stroke:#228B22,color:#000
    classDef active  fill:#E0F0FF,stroke:#1E90FF,color:#000
    classDef warning fill:#FFF5CC,stroke:#DAA520,color:#000
    classDef error   fill:#FFD6D6,stroke:#B22222,color:#000

    %% Apply Styles
    class Idle,Connected success
    class Connecting active
    class Disconnected warning
    class Error error

```

