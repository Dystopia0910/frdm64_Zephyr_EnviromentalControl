/**
 * @file    mode_controller.c
 *
 * @brief   Implementation of greenhouse operating mode control.
 *
 * This module configures the hardware button and on-board LED and manages
 * transitions between READ_ONLY and ADJUSTING modes. It updates the global
 * environment controller (env) and ensures thread-safe operations through
 * mutex protection.
 *
 * A dedicated thread waits for button interrupts and performs mode toggling,
 * providing a simple local interface to override system behavior without
 * relying on Bluetooth input.
 *
 * @par
 * Rodriguez Padilla, Daniel Jiram  
 * IE703331  
 * Martin del Campo, Mauricio  
 * IE734429
 */

 #include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "mode_controller.h"
#include "display_manager.h"
#include "env_controller.h"

/* Button and LED aliases must exist in the board overlay */
#define BUTTON_NODE DT_ALIAS(sw0)
#define LED_NODE    DT_ALIAS(led0)

/* Read button and LED pin specifications */
static struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);
static struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

static struct gpio_callback button_cb;
K_SEM_DEFINE(button_sem, 0, 1);

/* Interrupt callback for button press */
static void button_pressed(const struct device *dev,
                           struct gpio_callback *cb,
                           uint32_t pins)
{
    k_sem_give(&button_sem);
}

/* Initialize LED + button + interrupts */
int mode_controller_init(void)
{
    if (!device_is_ready(led.port)) {
        printk("LED device not ready\n");
        return -1;
    }

    if (!device_is_ready(button.port)) {
        printk("Button device not ready\n");
        return -1;
    }

    gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&button, GPIO_INPUT);
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(&button_cb, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb);

    printk("Mode controller initialized.\n");
    return 0;
}

/* Update global mode safely and notify display */
void mode_controller_set_mode(env_mode_t new_mode)
{
    k_mutex_lock(&env.lock, K_FOREVER);
    env.mode = new_mode;
    k_mutex_unlock(&env.lock);

    /* Update LED based on mode */
    gpio_pin_set_dt(&led, (new_mode == ENV_MODE_ADJUSTING));

    /* Print debug info */
    if (new_mode == ENV_MODE_READ_ONLY) {
        printk("System mode set to READ_ONLY\n");
    } else {
        printk("System mode set to ADJUSTING\n");
    }
}

/* Thread: waits for button press and toggles mode */
void mode_controller_thread(void *p1, void *p2, void *p3)
{
    while (1) {
        k_sem_take(&button_sem, K_FOREVER);

        /* Toggle mode */
        k_mutex_lock(&env.lock, K_FOREVER);
        env_mode_t current = env.mode;
        env_mode_t new_mode = 
            (current == ENV_MODE_READ_ONLY) ?
            ENV_MODE_ADJUSTING : ENV_MODE_READ_ONLY;
        k_mutex_unlock(&env.lock);

        mode_controller_set_mode(new_mode);
    }
}
