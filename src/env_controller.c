/**
 * @file    env_controller.c
 *
 * @brief   Implementation of global environment controller.
 *
 * This file defines the global instance of the environment controller (`env`),
 * initializes its mutex, and sets default operating parameters such as initial
 * mode and default setpoints. All system modules rely on this shared structure
 * to synchronize access to greenhouse state information.
 *
 * @par
 * Rodriguez Padilla, Daniel Jiram  
 * IE703331  
 * Martin del Campo, Mauricio  
 * IE734429
 */

#include "env_controller.h"

/* Global environment controller instance */
env_controller_t env = {
    .mode = ENV_MODE_READ_ONLY,
    .measurements = {0.0f, 0.0f, 0.0f},
    .setpoints = {
        .target_temperature = 25.0f,
        .target_humidity = 60.0f,
        .target_light = 500.0f
    }
};

void env_controller_init(void)
{
    /* Initialize the mutex that protects global state */
    k_mutex_init(&env.lock);

    /* Optionally overwrite default values here if needed */
}
