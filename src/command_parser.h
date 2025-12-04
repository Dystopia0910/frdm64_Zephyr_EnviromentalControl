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
