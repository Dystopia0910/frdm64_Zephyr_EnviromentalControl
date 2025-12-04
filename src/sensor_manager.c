/**
 * @file sensor_manager.c
 * @brief Manager for all environmental sensors
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <stdio.h>   /* Para snprintf */
#include <string.h>  /* Para memset */
#include <math.h>  /* Para NAN y funciones matemáticas */
#include "sensor_manager.h"

LOG_MODULE_REGISTER(sensor_manager, LOG_LEVEL_DBG);

/* Device tree labels - MUST MATCH your .dts files */
#define LM35_NODE        DT_ALIAS(lm35)
#define BH1750_NODE      DT_ALIAS(bh1750)
#define DHT11_NODE       DT_ALIAS(dht11)

/* Device pointers */
static const struct device *const lm35_dev = DEVICE_DT_GET_OR_NULL(LM35_NODE);
static const struct device *const bh1750_dev = DEVICE_DT_GET_OR_NULL(BH1750_NODE);
static const struct device *const dht11_dev = DEVICE_DT_GET_OR_NULL(DHT11_NODE);

/* Error tracking */
static char error_msg[64] = {0};
static bool sensors_ready = false;

/**
 * @brief Initialize all sensors
 * @return 0 on success, negative error code on failure
 */
int sensor_manager_init(void)
{
    int ret = 0;
    
    LOG_INF("Initializing sensor manager...");
    
    /* Check if devices are ready */
    if (lm35_dev && !device_is_ready(lm35_dev)) {
        LOG_ERR("LM35 device not ready");
        snprintf(error_msg, sizeof(error_msg), "LM35 not ready");
        ret = -ENODEV;
    }
    
    if (bh1750_dev && !device_is_ready(bh1750_dev)) {
        LOG_ERR("BH1750 device not ready");
        snprintf(error_msg, sizeof(error_msg), "BH1750 not ready");
        ret = -ENODEV;
    }
    
    if (dht11_dev && !device_is_ready(dht11_dev)) {
        LOG_ERR("DHT11 device not ready");
        snprintf(error_msg, sizeof(error_msg), "DHT11 not ready");
        ret = -ENODEV;
    }
    
    /* Check if at least one sensor is available */
    if (!lm35_dev && !dht11_dev) {
        LOG_ERR("No temperature sensor available");
        snprintf(error_msg, sizeof(error_msg), "No temp sensor");
        ret = -ENODEV;
    }
    
    if (!bh1750_dev) {
        LOG_ERR("No light sensor available");
        snprintf(error_msg, sizeof(error_msg), "No light sensor");
        ret = -ENODEV;
    }
    
    if (ret == 0) {
        sensors_ready = true;
        LOG_INF("Sensor manager initialized successfully");
        LOG_INF("Available sensors: %s%s%s",
                lm35_dev ? "LM35 " : "",
                bh1750_dev ? "BH1750 " : "",
                dht11_dev ? "DHT11" : "");
    }
    
    return ret;
}

/**
 * @brief Read temperature from available sensor
 * @param temp Pointer to store temperature value
 * @return 0 on success, negative error code on failure
 */
static int read_temperature(float *temp)
{
    struct sensor_value val;
    int ret;
    
    /* Prefer DHT11 if available (has humidity too) */
    if (dht11_dev && device_is_ready(dht11_dev)) {
        ret = sensor_sample_fetch(dht11_dev);
        if (ret < 0) {
            LOG_ERR("Failed to fetch DHT11 sample: %d", ret);
            return ret;
        }
        
        ret = sensor_channel_get(dht11_dev, SENSOR_CHAN_AMBIENT_TEMP, &val);
        if (ret < 0) {
            LOG_ERR("Failed to get DHT11 temperature: %d", ret);
            return ret;
        }
        
        *temp = sensor_value_to_float(&val);
        return 0;
    }
    
    /* Fall back to LM35 */
    if (lm35_dev && device_is_ready(lm35_dev)) {
        ret = sensor_sample_fetch(lm35_dev);
        if (ret < 0) {
            LOG_ERR("Failed to fetch LM35 sample: %d", ret);
            return ret;
        }
        
        ret = sensor_channel_get(lm35_dev, SENSOR_CHAN_AMBIENT_TEMP, &val);
        if (ret < 0) {
            LOG_ERR("Failed to get LM35 temperature: %d", ret);
            return ret;
        }
        
        *temp = sensor_value_to_float(&val);
        return 0;
    }
    
    return -ENODEV;
}

/**
 * @brief Read light level from BH1750
 * @param light Pointer to store light value in lux
 * @return 0 on success, negative error code on failure
 */
static int read_light(float *light)
{
    struct sensor_value val;
    int ret;
    
    if (!bh1750_dev || !device_is_ready(bh1750_dev)) {
        return -ENODEV;
    }
    
    ret = sensor_sample_fetch(bh1750_dev);
    if (ret < 0) {
        LOG_ERR("Failed to fetch BH1750 sample: %d", ret);
        return ret;
    }
    
    ret = sensor_channel_get(bh1750_dev, SENSOR_CHAN_LIGHT, &val);
    if (ret < 0) {
        LOG_ERR("Failed to get BH1750 light: %d", ret);
        return ret;
    }
    
    *light = sensor_value_to_float(&val);
    return 0;
}

/**
 * @brief Read humidity from DHT11
 * @param humidity Pointer to store humidity value
 * @return 0 on success, negative error code on failure
 */
static int read_humidity(float *humidity)
{
    struct sensor_value val;
    int ret;
    
    if (!dht11_dev || !device_is_ready(dht11_dev)) {
        return -ENODEV;
    }
    
    ret = sensor_sample_fetch(dht11_dev);
    if (ret < 0) {
        LOG_ERR("Failed to fetch DHT11 sample: %d", ret);
        return ret;
    }
    
    ret = sensor_channel_get(dht11_dev, SENSOR_CHAN_HUMIDITY, &val);
    if (ret < 0) {
        LOG_ERR("Failed to get DHT11 humidity: %d", ret);
        return ret;
    }
    
    *humidity = sensor_value_to_float(&val);
    return 0;
}

/**
 * @brief Read all sensors at once
 * @param data Pointer to sensor_data_t structure
 * @return 0 on success, negative error code if any sensor fails
 */
int sensor_manager_read_all(sensor_data_t *data)
{
    int ret = 0;
    int temp_ret, light_ret, humid_ret;
    
    if (!data || !sensors_ready) {
        return -EINVAL;
    }
    
    /* Clear data structure */
    memset(data, 0, sizeof(sensor_data_t));
    
    /* Read temperature */
    temp_ret = read_temperature(&data->temperature);
    if (temp_ret == 0) {
        data->temperature_valid = true;
    } else {
        data->temperature = NAN;
        data->temperature_valid = false;
        ret = temp_ret;
        LOG_WRN("Failed to read temperature: %d", temp_ret);
    }
    
    /* Read light */
    light_ret = read_light(&data->light_level);
    if (light_ret == 0) {
        data->light_valid = true;
    } else {
        data->light_level = NAN;
        data->light_valid = false;
        ret = light_ret;
        LOG_WRN("Failed to read light: %d", light_ret);
    }
    
    /* Read humidity */
    humid_ret = read_humidity(&data->humidity);
    if (humid_ret == 0) {
        data->humidity_valid = true;
    } else {
        data->humidity = NAN;
        data->humidity_valid = false;
        ret = humid_ret;
        LOG_WRN("Failed to read humidity: %d", humid_ret);
    }
    
    data->timestamp = k_uptime_get_32();
    
    return ret;
}

/* Individual sensor reading functions */
int sensor_manager_read_temperature(float *temp)
{
    return read_temperature(temp);
}

int sensor_manager_read_light(float *light)
{
    return read_light(light);
}

int sensor_manager_read_humidity(float *humidity)
{
    return read_humidity(humidity);
}

/* Status functions */
bool sensor_manager_is_ready(void)
{
    return sensors_ready;
}

bool sensor_manager_has_error(void)
{
    return error_msg[0] != '\0';
}

const char* sensor_manager_get_error(void)
{
    return error_msg;
}