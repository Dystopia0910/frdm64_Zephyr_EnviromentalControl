/**
 * @file    env_controller.h
 *
 * @brief   Global environment controller data structure and API.
 *
 * This module defines the central shared state of the greenhouse control system,
 * including operating mode, real-time sensor measurements, and user-defined
 * setpoints. All modules interact with this controller to read or modify system
 * state safely through a mutex-protected structure.
 *
 * The env_controller acts as the synchronized "core" that ties together
 * sensor_manager, display_manager, adjust_manager, mode_controller, and
 * uart_bt modules.
 *
 * @par
 * Rodriguez Padilla, Daniel Jiram  
 * IE703331  
 * Martin del Campo, Mauricio  
 * IE734429
 */

#ifndef ENV_CONTROLLER_H
#define ENV_CONTROLLER_H

#include <zephyr/kernel.h>

/* System operating modes */
typedef enum {
    ENV_MODE_READ_ONLY = 0,
    ENV_MODE_ADJUSTING = 1
} env_mode_t;

/* Latest measurements provided by sensor_manager */
typedef struct {
    float temperature;   /* Celsius */
    float humidity;      /* Percentage */
    float light;         /* Lux */
} env_measurements_t;

/* User-defined target values (adjustable by Bluetooth or UI) */
typedef struct {
    float target_temperature;   /* Celsius */
    float target_humidity;      /* Percentage */
    float target_light;         /* Lux */
} env_setpoints_t;

/* Central environment controller structure */
typedef struct {
    env_mode_t mode;
    env_measurements_t measurements;
    env_setpoints_t setpoints;
    struct k_mutex lock; /* Protects all shared data */
} env_controller_t;

/* Global instance */
extern env_controller_t env;

/* Initializes the controller (mutex + default values) */
void env_controller_init(void);

#endif /* ENV_CONTROLLER_H */
