/**
 * @file    adjust_manager.c
 *
 * @brief   Implementation of setpoint adjustment and actuator control logic.
 *
 * This file validates and applies incoming setpoint changes, updates the global
 * environment controller, and evaluates system measurements to control actuators
 * such as ventilation, irrigation motors, and light indicators.
 *
 * The actuator control routine compares actual environmental readings against
 * user-defined thresholds and activates or deactivates outputs accordingly.
 * This enables automated greenhouse behavior driven by Bluetooth configuration.
 *
 * @par
 * Rodriguez Padilla, Daniel Jiram  
 * IE703331  
 * Martin del Campo, Mauricio  
 * IE734429
 */

 #include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>

#include "adjust_manager.h"
#include "mode_controller.h"
#include "env_controller.h"

/* Actuator GPIO definitions from device tree aliases */
#define FAN_NODE          DT_ALIAS(fan_actuator)
#define IRRIGATION_NODE   DT_ALIAS(irrigation_actuator)
#define LIGHT_NODE        DT_ALIAS(led0)   /* Onboard LED already exists */

static const struct gpio_dt_spec fan_gpio =
        GPIO_DT_SPEC_GET(FAN_NODE, gpios);

static const struct gpio_dt_spec irrigation_gpio =
        GPIO_DT_SPEC_GET(IRRIGATION_NODE, gpios);

static const struct gpio_dt_spec light_gpio =
        GPIO_DT_SPEC_GET(LIGHT_NODE, gpios);

/* Initializes actuator GPIO pins */
static void adjust_manager_init_actuators(void)
{
    gpio_pin_configure_dt(&fan_gpio, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&irrigation_gpio, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&light_gpio, GPIO_OUTPUT_INACTIVE);
}

/* Call this ONCE from env_controller_init or main */
void adjust_manager_init(void)
{
    adjust_manager_init_actuators();
}

/* Core actuator control logic */
void adjust_manager_update_actuators(void)
{
    env_measurements_t m;
    env_setpoints_t sp;

    /* Read shared state safely */
    k_mutex_lock(&env.lock, K_FOREVER);
    m = env.measurements;
    sp = env.setpoints;
    k_mutex_unlock(&env.lock);

    /* --- LIGHT CONTROL using onboard LED --- */
    if (m.light < sp.target_light) {
        gpio_pin_set_dt(&light_gpio, 1);   /* Light is too low */
    } else {
        gpio_pin_set_dt(&light_gpio, 0);
    }

    /* --- TEMPERATURE CONTROL (Fan Motor) --- */
    if (m.temperature > sp.target_temperature) {
        gpio_pin_set_dt(&fan_gpio, 1);     /* Activate fan */
    } else {
        gpio_pin_set_dt(&fan_gpio, 0);
    }

    /* --- HUMIDITY CONTROL (Irrigation Motor) --- */
    if (m.humidity < sp.target_humidity) {
        gpio_pin_set_dt(&irrigation_gpio, 1);  /* Activate pump */
    } else {
        gpio_pin_set_dt(&irrigation_gpio, 0);
    }
}

/* These remain unchanged from earlier (setpoints, validation, etc.) */
void adjust_manager_apply_new_setpoints(const env_setpoints_t *new_sp)
{
    if (!new_sp) return;

    k_mutex_lock(&env.lock, K_FOREVER);
    env.setpoints = *new_sp;
    k_mutex_unlock(&env.lock);

    printk("Setpoints updated: T=%.1f  H=%.1f  L=%.1f\n",
           new_sp->target_temperature,
           new_sp->target_humidity,
           new_sp->target_light);
}

bool adjust_manager_validate(const env_setpoints_t *sp)
{ … unchanged … }

void adjust_manager_process_action(...)
{ … unchanged (for now until UART integration) … }
