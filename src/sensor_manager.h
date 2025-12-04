/**
 * @file sensor_manager.h
 * @brief Manager for all environmental sensors
 */

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <zephyr/drivers/sensor.h>

/* Sensor data structure */
typedef struct {
    float temperature;     /* in Celsius */
    float light_level;     /* in lux */
    float humidity;       /* in percentage */
    bool temperature_valid;
    bool light_valid;
    bool humidity_valid;
    uint32_t timestamp;   /* Last reading timestamp */
} sensor_data_t;

/* Sensor initialization */
int sensor_manager_init(void);

/* Read all sensors */
int sensor_manager_read_all(sensor_data_t *data);

/* Read individual sensors */
int sensor_manager_read_temperature(float *temp);
int sensor_manager_read_light(float *light);
int sensor_manager_read_humidity(float *humidity);

/* Sensor status */
bool sensor_manager_is_ready(void);
bool sensor_manager_has_error(void);

/* Get last error string */
const char* sensor_manager_get_error(void);

#endif /* SENSOR_MANAGER_H */