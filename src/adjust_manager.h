/**
 * @file    adjust_manager.h
 *
 * @brief   High-level API for applying setpoint changes and system adjustments.
 *
 * This module receives validated parameter updates (typically from UART/Bluetooth
 * commands), ensures data integrity, and applies them to the environment
 * controller. It also triggers actuator control logic, enabling or disabling
 * motors or indicators based on real-time measurements compared to configured
 * thresholds.
 *
 * The adjust_manager acts as the "decision layer" between raw command parsing
 * and system state mutation.
 *
 * @par
 * Rodriguez Padilla, Daniel Jiram  
 * IE703331  
 * Martin del Campo, Mauricio  
 * IE734429
 */

 #ifndef ADJUST_MANAGER_H
#define ADJUST_MANAGER_H

#include "env_controller.h"

void adjust_manager_init(void);
void adjust_manager_update_actuators(void);


/* Applies validated setpoints to the global controller */
void adjust_manager_apply_new_setpoints(const env_setpoints_t *new_sp);

/* Validates setpoints before applying them */
bool adjust_manager_validate(const env_setpoints_t *sp);

/* Converts parser results into system changes (WITHOUT UART) */
void adjust_manager_process_action(env_mode_t requested_mode,
                                   const env_setpoints_t *parsed_sp,
                                   bool change_setpoints,
                                   bool change_mode);

#endif /* ADJUST_MANAGER_H */
