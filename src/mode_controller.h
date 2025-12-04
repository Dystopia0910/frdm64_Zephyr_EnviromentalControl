/**
 * @file    mode_controller.h
 *
 * @brief   Mode management API for switching between READ_ONLY and ADJUSTING modes.
 *
 * This module exposes functions to initialize the user mode interface (button + LED),
 * explicitly change system mode, and run a background thread that toggles modes upon
 * button presses. Mode changes propagate into the env_controller and may alter system
 * behavior such as actuator control and UI updates.
 *
 * @par
 * Rodriguez Padilla, Daniel Jiram  
 * IE703331  
 * Martin del Campo, Mauricio  
 * IE734429
 */

 #ifndef MODE_CONTROLLER_H
#define MODE_CONTROLLER_H

#include <zephyr/kernel.h>
#include "env_controller.h"

/* Initializes button GPIO and mode control logic */
int mode_controller_init(void);

/* Explicitly sets the system mode */
void mode_controller_set_mode(env_mode_t new_mode);

/* Background thread that listens for button presses */
void mode_controller_thread(void *p1, void *p2, void *p3);

#endif /* MODE_CONTROLLER_H */
