#ifndef ADJUST_MANAGER_H
#define ADJUST_MANAGER_H

#include "env_controller.h"

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
