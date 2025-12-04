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
