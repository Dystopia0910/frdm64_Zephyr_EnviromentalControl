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
