/**
 * ath20.c
 *
 * This file is used to define the AHT20 sensor constants and functions.
 *
 * Author: Nils Lahaye (2024)
 *
 */

#include "aht20.h"

#define DT_DRV_COMPAT aosong_aht20

LOG_MODULE_REGISTER(AHT20, CONFIG_AHT20_LOG_LEVEL); /* Register the module for log */

// static const struct i2c_dt_spec aht20_spec = I2C_DT_SPEC_GET(DT_NODELABEL(temperature_sensor)); /* AHT20 i2c spec */
static const struct i2c_dt_spec aht20_spec;

static bool isInitialized = false; /* Is the sensor initialized? */

static uint8_t  cmdBuff[4];      /* Command buffer */
static uint8_t  dataBuff[7];     /* Data buffer */
static uint32_t humidity_raw;    /* Humidity raw value */
static uint32_t temperature_raw; /* Temperature raw value */

/**
 * @brief Initalise the AHT20 sensor on i2c bus 1
 *
 * @return 0 on success, error code otherwise
 */
int aht20_init(void)
{
    if (isInitialized)
    {
        LOG_WRN("device already initialized");
        return 0;
    }

    LOG_INF("init");

    RET_IF_ERR(!device_is_ready(aht20_spec.bus), "I2C device not ready");

    cmdBuff[0] = AHT20_CMD_RESET;
    RET_IF_ERR(i2c_write_dt(&aht20_spec, cmdBuff, 1), "reset failed");

    k_sleep(K_MSEC(10)); /* Wait for the sensor to reset */

    cmdBuff[0] = AHT20_CMD_INITIALIZE;
    LOG_IF_ERR(i2c_write_dt(&aht20_spec, cmdBuff, 1), "initialization failed");

    cmdBuff[0] = AHT20_CMD_GET_STATUS;
    LOG_IF_ERR(i2c_write_dt(&aht20_spec, cmdBuff, 1), "get status failed");

    if (!dataBuff[3])
    { /* Check if the sensor is calibrated */
        LOG_INF("Not calibrated, calibrating...");
        cmdBuff[0] = AHT20_CMD_INITIALIZE;
        LOG_IF_ERR(i2c_write_dt(&aht20_spec, cmdBuff, 1), "calibration initialization failed");
        k_sleep(K_MSEC(10));
    }

    LOG_INF("Init done");

    isInitialized = true;

    return 0;
}

/**
 * @brief Read the temperature and humidity from the AHT20 sensor
 *
 * @param temperature pointer to the variable where the temperature will be stored
 * @param humidity pointer to the variable where the humidity will be stored
 *
 * @return 0 on success, error code otherwise
 */
int aht20_read(float* temperature, float* humidity)
{
    LOG_INF("Reading sensor");

    if (!isInitialized)
    {
        LOG_ERR("Not initialized");
        return 1;
    }

    cmdBuff[0] = AHT20_CMD_TRIGGER_MEASURE;
    cmdBuff[1] = AHT20_TRIGGER_MEASURE_BYTE_0;
    cmdBuff[2] = AHT20_TRIGGER_MEASURE_BYTE_1;
    if (i2c_write_dt(&aht20_spec, cmdBuff, 3) != 0)
    {
        LOG_IF_ERR(i2c_write_dt(&aht20_spec, cmdBuff, 3), "trigger measure failed");
    }

    k_sleep(K_MSEC(80));
    LOG_IF_ERR(i2c_read_dt(&aht20_spec, dataBuff, 7), "read failed");

    humidity_raw = dataBuff[1];
    humidity_raw <<= 8;
    humidity_raw |= dataBuff[2];
    humidity_raw <<= 4;
    humidity_raw |= dataBuff[3] >> 4;
    *humidity = ((float) humidity_raw * 100) / 0x100000;

    temperature_raw = dataBuff[3] & 0x0F;
    temperature_raw <<= 8;
    temperature_raw |= dataBuff[4];
    temperature_raw <<= 8;
    temperature_raw |= dataBuff[5];
    *temperature = ((float) temperature_raw * 200 / 0x100000) - 50;

    uint8_t crc = crc8(dataBuff, 6, 0x31, 0xff, false);
    if (crc != dataBuff[6])
    {
        LOG_WRN("CRC check failed (%02x != %02x)", crc, dataBuff[6]);
        return 4;
    }

    LOG_DBG("Raw data: %02x %02x %02x %02x %02x %02x %02x", dataBuff[0], dataBuff[1], dataBuff[2], dataBuff[3],
            dataBuff[4], dataBuff[5], dataBuff[6]);
    LOG_DBG("Temperature raw: %d \t converted : %d.%dC", temperature_raw, (int) *temperature,
            (int) (*temperature * 10) % 10);
    LOG_DBG("Humidity raw: %d \t converted : %d.%d%%", humidity_raw, (int) *humidity, (int) (*humidity * 10) % 10);

    LOG_INF("Read done");

    return 0;
}