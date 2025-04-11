# 🌡️ Sensor Manager

Handles polling and sending sensor data.

## 🧩 Design Pattern: Template/Strategy

Sensors inherit from a base `sensor` interface and implement `read()` + `getName()`.

## 📚 Flow

```mermaid
classDiagram
    class sensorManager
    class sensor
    class lightSensor
    class temperatureSensor

    sensor <|-- lightSensor
    sensor <|-- temperatureSensor
    sensorManager --> sensor

```

## 🔄 Workflow

- sensorManager owns a list of sensors
- Calls readSensors()
- Uses sockets to send their data