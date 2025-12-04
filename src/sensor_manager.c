/**
 * @file    sensor_manager.c
 *
 * @brief   Implementation of environmental sensor sampling routines.
 *
 * This module initializes and interfaces with configured sensors defined in
 * the device tree. It performs synchronous sample acquisition, handles error
 * reporting, and produces structured sensor_data_t outputs containing both
 * raw values and validity indicators.
 *
 * These readings ultimately feed the env_controller structure and drive
 * display, actuator, and Bluetooth reporting behavior.
 *
 * @par
 * Rodriguez Padilla, Daniel Jiram  
 * IE703331  
 * Martin del Campo, Mauricio  
 * IE734429
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sensor_manager.h"

LOG_MODULE_REGISTER(sensor_manager, LOG_LEVEL_INF);

/* -------------------- Device Tree Bindings -------------------- */

#define LM35_ADC_NODE      DT_NODELABEL(adc0)
#define LM35_CHANNEL_ID    6   /* ADC0_SE6a = PTD2 */

#define LM35_NODE          DT_ALIAS(lm35)
#define BH1750_NODE        DT_ALIAS(bh1750)
#define DHT11_NODE         DT_ALIAS(dht11)

static const struct device *adc_dev  = DEVICE_DT_GET(LM35_ADC_NODE);
static const struct device *bh1750_dev = DEVICE_DT_GET_OR_NULL(BH1750_NODE);
static const struct device *dht11_dev  = DEVICE_DT_GET_OR_NULL(DHT11_NODE);

/* Error tracking */
static char error_msg[64] = {0};
static bool sensors_ready = false;

/* ADC setup */
static struct adc_channel_cfg lm35_ch_cfg = {
    .gain = ADC_GAIN_1,
    .reference = ADC_REF_INTERNAL,
    .acquisition_time = ADC_ACQ_TIME_DEFAULT,
    .channel_id = LM35_CHANNEL_ID,
    .differential = 0,
};

/* Buffer for ADC conversion */
static int16_t lm35_buf;

/* ADC sequence */
static struct adc_sequence lm35_seq = {
    .channels = BIT(LM35_CHANNEL_ID),
    .buffer = &lm35_buf,
    .buffer_size = sizeof(lm35_buf),
    .resolution = 12,
};

/* ------------------ Initialization ------------------ */

int sensor_manager_init(void)
{
    sensors_ready = false;

    if (!device_is_ready(adc_dev)) {
        LOG_ERR("ADC0 not ready!");
        snprintf(error_msg, sizeof(error_msg), "ADC not ready");
        return -ENODEV;
    }

    if (adc_channel_setup(adc_dev, &lm35_ch_cfg) != 0) {
        LOG_ERR("Failed ADC setup for LM35");
        return -EINVAL;
    }

    LOG_INF("ADC configured for LM35");

    if (bh1750_dev && !device_is_ready(bh1750_dev)) {
        LOG_ERR("BH1750 not ready");
        return -ENODEV;
    }

    if (dht11_dev && !device_is_ready(dht11_dev)) {
        LOG_ERR("DHT11 not ready");
        return -ENODEV;
    }

    sensors_ready = true;
    LOG_INF("Sensor manager initialized successfully");
    return 0;
}

/* ------------------ LM35 via ADC ------------------ */

static int read_lm35(float *temperature)
{
    int ret = adc_read(adc_dev, &lm35_seq);
    if (ret < 0) {
        LOG_ERR("ADC read failed: %d", ret);
        return ret;
    }

    /* Convert ADC reading (0–4095) to mV */
    float mv = (lm35_buf * 3300.0f) / 4095.0f;

    /* LM35 outputs 10mV per °C */
    *temperature = mv * 10.0f;

    LOG_INF("LM35 raw=%d, mv=%.2f, temp=%.2f C",
            lm35_buf, (double)mv, (double)(*temperature));

    return 0;
}

/* ------------------ BH1750 Light Sensor ------------------ */

static int read_bh1750(float *lux)
{
    struct sensor_value val;
    int ret;

    ret = sensor_sample_fetch(bh1750_dev);
    if (ret < 0) {
        LOG_ERR("BH1750 sample error: %d", ret);
        return ret;
    }

    ret = sensor_channel_get(bh1750_dev, SENSOR_CHAN_LIGHT, &val);
    if (ret < 0) {
        LOG_ERR("BH1750 channel error: %d", ret);
        return ret;
    }

    *lux = sensor_value_to_float(&val);
    return 0;
}

/* ------------------ DHT11 (Humidity + Temp) ------------------ */

static int read_dht11(float *temperature, float *humidity)
{
    struct sensor_value t, h;
    int ret;

    ret = sensor_sample_fetch(dht11_dev);
    if (ret < 0) {
        LOG_ERR("DHT11 fetch error: %d", ret);
        return ret;
    }

    if (sensor_channel_get(dht11_dev, SENSOR_CHAN_AMBIENT_TEMP, &t) < 0)
        return -EIO;

    if (sensor_channel_get(dht11_dev, SENSOR_CHAN_HUMIDITY, &h) < 0)
        return -EIO;

    *temperature = sensor_value_to_float(&t);
    *humidity    = sensor_value_to_float(&h);
    return 0;
}

/* ------------------ Main Read Function ------------------ */

int sensor_manager_read_all(sensor_data_t *data)
{
    if (!sensors_ready)
        return -ENODEV;

    memset(data, 0, sizeof(*data));

    float t, h, l;

    /* 1) Read temperature (prefer DHT11 if valid) */
    if (dht11_dev && read_dht11(&t, &h) == 0) {
        data->temperature = t;
        data->humidity    = h;
        data->temperature_valid = true;
        data->humidity_valid = true;
    } else {
        /* fallback LM35 */
        if (read_lm35(&t) == 0) {
            data->temperature = t;
            data->temperature_valid = true;
        } else {
            data->temperature = NAN;
        }
        data->humidity = NAN; /* no humidity */
    }

    /* 2) Light */
    if (bh1750_dev && read_bh1750(&l) == 0) {
        data->light_level = l;
        data->light_valid = true;
    } else {
        data->light_level = NAN;
    }

    data->timestamp = k_uptime_get_32();
    return 0;
}
