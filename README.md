# Zephyr Example Application

<a href="https://wakatime.com/badge/github/osamasalahuddin/zephyr_home">
    <img src="https://wakatime.com/badge/github/osamasalahuddin/zephyr_home.svg" alt="wakatime">
</a>
<a href="https://github.com/zephyrproject-rtos/example-application/actions/workflows/build.yml?query=branch%3Amain">
  <img src="https://github.com/zephyrproject-rtos/example-application/actions/workflows/build.yml/badge.svg?event=push">
</a>
<a href="https://github.com/zephyrproject-rtos/example-application/actions/workflows/docs.yml?query=branch%3Amain">
  <img src="https://github.com/zephyrproject-rtos/example-application/actions/workflows/docs.yml/badge.svg?event=push">
</a>
<a href="https://zephyrproject-rtos.github.io/example-application">
  <img alt="Documentation" src="https://img.shields.io/badge/documentation-3D578C?logo=sphinx&logoColor=white">
</a>
<a href="https://zephyrproject-rtos.github.io/example-application/doxygen">
  <img alt="API Documentation" src="https://img.shields.io/badge/API-documentation-3D578C?logo=c&logoColor=white">
</a>

This repository contains a Zephyr example application. The main purpose of this
repository is to serve as a reference on how to structure Zephyr-based
applications. Some of the features demonstrated in this example are:

- Basic [Zephyr application][app_dev] skeleton
- [Zephyr workspace applications][workspace_app]
- [Zephyr modules][modules]
- [West T2 topology][west_t2]
- [Custom boards][board_porting]
- Custom [devicetree bindings][bindings]
- Out-of-tree [drivers][drivers]
- Out-of-tree libraries
- Example CI configuration (using GitHub Actions)
- Custom [west extension][west_ext]
- Custom [Zephyr runner][runner_ext]
- Doxygen and Sphinx documentation boilerplate

This repository is versioned together with the [Zephyr main tree][zephyr]. This
means that every time that Zephyr is tagged, this repository is tagged as well
with the same version number, and the [manifest](west.yml) entry for `zephyr`
will point to the corresponding Zephyr tag. For example, the `example-application`
v2.6.0 will point to Zephyr v2.6.0. Note that the `main` branch always
points to the development branch of Zephyr, also `main`.

[app_dev]: https://docs.zephyrproject.org/latest/develop/application/index.html
[workspace_app]: https://docs.zephyrproject.org/latest/develop/application/index.html#zephyr-workspace-app
[modules]: https://docs.zephyrproject.org/latest/develop/modules.html
[west_t2]: https://docs.zephyrproject.org/latest/develop/west/workspaces.html#west-t2
[board_porting]: https://docs.zephyrproject.org/latest/guides/porting/board_porting.html
[bindings]: https://docs.zephyrproject.org/latest/guides/dts/bindings.html
[drivers]: https://docs.zephyrproject.org/latest/reference/drivers/index.html
[zephyr]: https://github.com/zephyrproject-rtos/zephyr
[west_ext]: https://docs.zephyrproject.org/latest/develop/west/extensions.html
[runner_ext]: https://docs.zephyrproject.org/latest/develop/modules.html#external-runners

## Getting Started

Before getting started, make sure you have a proper Zephyr development
environment. Follow the official
[Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/getting_started/index.html).

### Initialization

The first step is to initialize the workspace folder (``my-workspace``) where
the ``example-application`` and all Zephyr modules will be cloned. Run the following
command:

```shell
# initialize my-workspace for the example-application (main branch)
west init -m https://github.com/zephyrproject-rtos/example-application --mr main my-workspace
# update Zephyr modules
cd my-workspace
west update
```

### Building and running

To build the application, run the following command:

```shell
cd example-application
west build -b $BOARD app
```

where `$BOARD` is the target board.

You can use the `custom_plank` board found in this
repository. Note that Zephyr sample boards may be used if an
appropriate overlay is provided (see `app/boards`).

A sample debug configuration is also provided. To apply it, run the following
command:

```shell
west build -b $BOARD app -- -DEXTRA_CONF_FILE=debug.conf
```

Once you have built the application, run the following command to flash it:

```shell
west flash
```

### Testing

To execute Twister integration tests, run the following command:

```shell
west twister -T tests --integration
```

### Documentation

A minimal documentation setup is provided for Doxygen and Sphinx. To build the
documentation first change to the ``doc`` folder:

```shell
cd doc
```

Before continuing, check if you have Doxygen installed. It is recommended to
use the same Doxygen version used in [CI](.github/workflows/docs.yml). To
install Sphinx, make sure you have a Python installation in place and run:

```shell
pip install -r requirements.txt
```

API documentation (Doxygen) can be built using the following command:

```shell
doxygen
```

The output will be stored in the ``_build_doxygen`` folder. Similarly, the
Sphinx documentation (HTML) can be built using the following command:

```shell
make html
```

The output will be stored in the ``_build_sphinx`` folder. You may check for
other output formats other than HTML by running ``make help``.


## 📘 Mermaid Class Diagram


### ✅ Color Legend

| Color        | Category       |
|--------------|----------------|
| 🟦 Blue       | WiFi subsystem |
| 🟨 Yellow     | Socket system  |
| 🟩 Green      | Sensor system  |
| 🩷 Pink       | Time sync      |
| ⚪ Gray       | Core entry & infra |


## 📘 UML Class Diagram
```mermaid
classDiagram
    class main {
        +main()
    }

    class wifiManager {
        +connect()
        +disconnect()
    }

    class wifiStateMachine {
        +setState()
        +handle()
    }

    class wifiState
    class wifiStateIdle
    class wifiStateConnecting
    class wifiStateConnected
    class wifiStateError

    wifiManager --> wifiStateMachine
    wifiStateMachine --> wifiState
    wifiState <|-- wifiStateIdle
    wifiState <|-- wifiStateConnecting
    wifiState <|-- wifiStateConnected
    wifiState <|-- wifiStateError

    class pingManager {
        +send_ping()
    }

    class networkManager {
        +init()
        +monitor()
        +getInterface()
    }

    networkManager --> pingManager
    networkManager --> wifiManager

    class socketManager {
        +open()
        +send()
        +close()
    }

    class socketStrategy {
        +connect()
        +send()
        +close()
    }

    class udpSocketStrategy
    class tcpSocketStrategy
    class tlsSocketStrategy

    socketStrategy <|-- udpSocketStrategy
    socketStrategy <|-- tcpSocketStrategy
    socketStrategy <|-- tlsSocketStrategy
    socketManager --> socketStrategy

    class sockets {
        -socketManager* pSocketManager
        -std::string host
        -uint16_t port
        -protocol proto
        +open()
        +send()
        +close()
    }
    sockets --> socketManager

    class sensorManager {
        +init()
        +readSensors()
        +sendData()
    }

    class sensor {
        +read()
        +getName()
    }

    class lightSensor {
        +read()
        +getLux()
    }

    class temperatureSensor {
        +read()
        +getCelsius()
    }

    sensorManager --> sensor
    sensor <|-- lightSensor
    sensor <|-- temperatureSensor

    class networkTimeManager {
        +syncTime()
    }

    main --> socketManager
    main --> sensorManager
    main --> networkManager
    main --> networkTimeManager
    main --> sockets

    %% === COLOR SCHEMES ===
    classDef WiFi fill:#D0E8FF,stroke:#003366,stroke-width:2;
    classDef Socket fill:#FFF4D6,stroke:#A67C00,stroke-width:2;
    classDef Sensor fill:#E1F8DC,stroke:#228B22,stroke-width:2;
    classDef Time fill:#F5D0E8,stroke:#C71585,stroke-width:2;
    classDef Core fill:#E6E6E6,stroke:#000000,stroke-width:1;

    %% === Apply styles ===
    class wifiManager,wifiStateMachine,wifiState,wifiStateIdle,wifiStateConnecting,wifiStateConnected,wifiStateError WiFi
    class socketManager,socketStrategy,udpSocketStrategy,tcpSocketStrategy,tlsSocketStrategy,sockets Socket
    class sensorManager,sensor,lightSensor,temperatureSensor Sensor
    class networkTimeManager Time
    class main,networkManager,pingManager Core
