#-------------------------------------------------------------------------------
# Zephyr Example Application
#
# Copyright (c) 2021 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.3)

#-------------------------------------------
# Overlay File
set(DTC_OVERLAY_FILE "boards/esp32.overlay")
set(CONFIG_APPLICATION_DEFINED_SYSCALL  TRUE)

# Define additional overlay configuration files
# set(OVERLAY_CONFIG "overlay-debug.conf; overlay-enterprise-variable-bufs.conf; overlay-enterprise.conf;")

# Pass configuration files to the build system
# set_property(GLOBAL PROPERTY OVERLAY_CONFIG "${OVERLAY_CONFIG}")

#-------------------------------------------
# Include Zephyr SDK
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

#-------------------------------------------
# Project
project(app)

#-------------------------------------------
# Source Files
# Main
target_sources(app PRIVATE src/main.cpp)

# My Logger
target_sources(app PRIVATE src/myLogger/myLogger.cpp)

# Manager Factory
target_sources(app PRIVATE src/managerFactory/managerFactory.cpp)

# Network Manager
target_sources(app PRIVATE src/networkManager/networkManager.cpp)

# Network Time Manager
target_sources(app PRIVATE src/networkTimeManager/networkTimeManager.cpp)

# Ping Manager
target_sources(app PRIVATE src/pingManager/pingManager.cpp)

# Sensor Manager
target_sources(app PRIVATE src/sensorManager/sensorManager.cpp)

# Sensors
target_sources(app PRIVATE src/sensors/temperatureSensor.cpp)
target_sources(app PRIVATE src/sensors/lightSensor.cpp)

# Socket
target_sources(app PRIVATE src/sockets/sockets.cpp)

# Socket Manager
target_sources(app PRIVATE src/socketManager/socketManager.cpp)

# Socket Strategy
target_sources(app PRIVATE src/socketStrategy/socketStrategy.cpp)

# Wifi Manager
target_sources(app PRIVATE src/wifiManager/wifiManager.cpp)
target_sources(app PRIVATE src/wifiManager/wifiManagerHandlers.cpp)

# Wifi State Machine
target_sources(app PRIVATE src/wifiSM/wifiContext.cpp)
target_sources(app PRIVATE src/wifiSM/wifiStateConnected.cpp)
target_sources(app PRIVATE src/wifiSM/wifiStateConnecting.cpp)
target_sources(app PRIVATE src/wifiSM/wifiStateDisconnected.cpp)
target_sources(app PRIVATE src/wifiSM/wifiStateError.cpp)
target_sources(app PRIVATE src/wifiSM/wifiStateIdle.cpp)

#-------------------------------------------
# Include Directories
# zephyr_syscall_include_directories(./../include/app/drivers/)
# zephyr_include_directories(./../include)
# zephyr_include_directories(./../include/app/drivers/)

# My Logger
target_include_directories(app PRIVATE src/myLogger)

# Network Manager
target_include_directories(app PRIVATE src/networkManager)

# Network Time Manager
target_include_directories(app PRIVATE src/networkTimeManager)

# Ping Manager
target_include_directories(app PRIVATE src/pingManager)

# Sensor Manager
target_include_directories(app PRIVATE src/sensorManager)

# Sensors
target_include_directories(app PRIVATE src/sensors)

# Sockets
target_include_directories(app PRIVATE src/sockets)

# Socket Manager
target_include_directories(app PRIVATE src/socketManager)
target_include_directories(app PRIVATE src/socketStrategy)

# Wifi Manager
target_include_directories(app PRIVATE src/managerFactory)
target_include_directories(app PRIVATE src/wifiManager)
target_include_directories(app PRIVATE src/wifiSM)

target_include_directories(app PRIVATE ./../include/)
target_include_directories(app PRIVATE ./../include/app/)
target_include_directories(app PRIVATE ./../include/app/drivers)
# target_include_directories(app PRIVATE ./../include/utils/myLogger)

#-------------------------------------------
# System Calls Header Files
# zephyr_syscall_header_ifdef (./../include/app/drivers/my_syscall.h)
# zephyr_syscall_header (./../include/app/drivers/blink.h)

#-------------------------------------------
# Libraries
target_link_libraries(app PRIVATE zephyr_interface)

#-------------------------------------------
# Compiler Options
target_compile_definitions(app PRIVATE
    WIFI_SSID="$ENV{WIFI_SSID}"
    WIFI_PASSWORD="$ENV{WIFI_PASSWORD}"
    MY_REMOTE="$ENV{MY_REMOTE}"
    MY_LOCAL="$ENV{MY_LOCAL}"
)

#-------------------------------------------
# GraphViz
add_custom_target(graphviz
    COMMAND ${CMAKE_COMMAND} "--graphviz=dependencyGraph.dot" .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)