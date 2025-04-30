#!/bin/bash

# ~/.espressif/tools/openocd-esp32/v0.12.0-esp32-20241016/openocd-esp32/bin/openocd \
#     -s ~/.espressif/tools/openocd-esp32/v0.12.0-esp32-20241016/openocd-esp32/share/openocd/scripts/ \
#     -f interface/ftdi/esp32_devkitj_v1.cfg -f target/esp32.cfg \
#     -c "set ESP_RTOS none" -c "set ESP_FLASH_SIZE 0" -c "init"
~/.espressif/tools/openocd-esp32/v0.12.0-esp32-20241016/openocd-esp32/bin/openocd \
    -s ~/.espressif/tools/openocd-esp32/v0.12.0-esp32-20241016/openocd-esp32/share/openocd/scripts/ \
    -f board/esp32-no-rtos.cfg -c "init" -c "reset halt"