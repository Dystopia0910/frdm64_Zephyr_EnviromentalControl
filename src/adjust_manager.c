#include <zephyr/sys/printk.h>
#include "adjust_manager.h"
#include "mode_controller.h"
#include "env_controller.h"
#include "display_manager.h"

/* Validate incoming setpoints */
bool adjust_manager_validate(const env_setpoints_t *sp)
{
    if (!sp) return false;

    /* Temperature range */
    if (sp->target_temperature < 0.0f || sp->target_temperature > 50.0f)
        return false;

    /* Humidity range */
    if (sp->target_humidity < 0.0f || sp->target_humidity > 100.0f)
        return false;

    /* Light level */
    if (sp->target_light < 0.0f || sp->target_light > 2000.0f)
        return false;

    return true;
}

/* Safely apply new setpoints to env_controller */
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

/* Central logic that executes parsed actions */
void adjust_manager_process_action(env_mode_t requested_mode,
                                   const env_setpoints_t *parsed_sp,
                                   bool change_setpoints,
                                   bool change_mode)
{
    /* Handle mode change first */
    if (change_mode) {
        mode_controller_set_mode(requested_mode);
    }

    /* Apply new setpoints if valid */
    if (change_setpoints && parsed_sp) {
        if (adjust_manager_validate(parsed_sp)) {
            adjust_manager_apply_new_setpoints(parsed_sp);
        } else {
            printk("Invalid setpoints received, ignoring.\n");
        }
    }
}
