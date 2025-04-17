/**
 * ath20.h
 *
 * This file is used to define the AHT20 sensor constants and functions.
 *
 * Author: Nils Lahaye (2024)
 *
 */

#ifndef AHT20_H_
#define AHT20_H_

#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/crc.h>
/*#include "../utils.h"*/

#define AHT20_CMD_RESET 0xBA              /* Reset command */
#define AHT20_CMD_TRIGGER_MEASURE 0xAC    /* Trigger measure command */
#define AHT20_TRIGGER_MEASURE_BYTE_0 0x33 /* Trigger measure command byte 0 */
#define AHT20_TRIGGER_MEASURE_BYTE_1 0x00 /* Trigger measure command byte 1 */
#define AHT20_CMD_GET_STATUS 0x71         /* Get status command */
#define AHT20_CMD_INITIALIZE 0xBE         /* Initialize command */

int aht20_init(void);

int aht20_read(float* temperature, float* humidity);

#endif /* AHT20_H */