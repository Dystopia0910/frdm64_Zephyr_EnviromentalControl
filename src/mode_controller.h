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
