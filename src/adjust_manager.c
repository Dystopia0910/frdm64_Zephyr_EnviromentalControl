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
#define LIGHT_NODE        DT_ALIAS(led0)

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

    /* Light control */
    gpio_pin_set_dt(&light_gpio, (m.light < sp.target_light));

    /* Temperature control */
    gpio_pin_set_dt(&fan_gpio, (m.temperature > sp.target_temperature));

    /* Humidity control */
    gpio_pin_set_dt(&irrigation_gpio, (m.humidity < sp.target_humidity));
}

/* Apply new setpoints */
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

/* Validation of setpoints */
bool adjust_manager_validate(const env_setpoints_t *sp)
{
    if (!sp) return false;

    return !(sp->target_temperature < 0.0f || sp->target_temperature > 50.0f ||
             sp->target_humidity    < 0.0f || sp->target_humidity    > 100.0f ||
             sp->target_light       < 0.0f || sp->target_light       > 2000.0f);
}

/* Process parsed command from UART */
void adjust_manager_process_action(env_mode_t requested_mode,
                                   const env_setpoints_t *parsed_sp,
                                   bool change_setpoints,
                                   bool change_mode)
{
    /* Handle mode change */
    if (change_mode) {
        mode_controller_set_mode(requested_mode);
    }

    /* Handle setpoint update */
    if (change_setpoints && parsed_sp) {
        if (adjust_manager_validate(parsed_sp)) {
            adjust_manager_apply_new_setpoints(parsed_sp);
        } else {
            printk("Invalid setpoints received.\n");
        }
    }
}
