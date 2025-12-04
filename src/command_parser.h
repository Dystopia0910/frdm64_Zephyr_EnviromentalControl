/**
 * @file    command_parser.h
 *
 * @brief   Command parsing interface for Bluetooth UART input.
 *
 * This module defines parser result structures and actions representing the
 * interpretation of incoming text commands. Commands such as:
 *     - TEMP=25.5,HUM=60,LUX=400
 *     - MODE=READ
 *     - MODE=ADJUST
 * are validated and translated into structured parser_result_t objects to be
 * executed by the adjust_manager.
 *
 * @par
 * Rodriguez Padilla, Daniel Jiram  
 * IE703331  
 * Martin del Campo, Mauricio  
 * IE734429
 */

 #ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "env_controller.h"

/* Actions the parser can request */
typedef enum {
    PARSER_ACTION_NONE = 0,
    PARSER_ACTION_SET_SETPOINTS,
    PARSER_ACTION_MODE_READ,
    PARSER_ACTION_MODE_ADJUST
} parser_action_t;

/* Resulting structure after parsing a command */
typedef struct {
    parser_action_t action;
    env_setpoints_t new_setpoints; /* Only valid when action == SET_SETPOINTS */
} parser_result_t;

/* Parses a Bluetooth command string */
parser_result_t command_parser_parse(const char *cmd,
                                     const env_setpoints_t *current_sp);

#endif /* COMMAND_PARSER_H */
