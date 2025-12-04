/**
 * @file    command_parser.c
 *
 * @brief   Text command interpretation for greenhouse configuration.
 *
 * This module processes incoming ASCII commands received over UART/Bluetooth,
 * extracting operational requests such as mode changes and setpoint updates.
 * It uses a case-insensitive tokenization approach to support flexible input
 * formatting while generating structured parser_result_t outputs.
 *
 * The command parser does not modify system state directly; instead, it provides
 * validated results for adjust_manager to apply securely.
 *
 * @par
 * Rodriguez Padilla, Daniel Jiram  
 * IE703331  
 * Martin del Campo, Mauricio  
 * IE734429
 */

 #include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "command_parser.h"

/* Helper: case-insensitive string compare */
static bool str_iequals(const char *a, const char *b)
{
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return false;
        a++; b++;
    }
    return (*a == '\0' && *b == '\0');
}

/* Parses commands like:
 *  TEMP=25.5,HUM=60,LUX=500
 *  MODE=READ
 *  MODE=ADJUST
 */
parser_result_t command_parser_parse(const char *cmd,
                                     const env_setpoints_t *current_sp)
{
    parser_result_t result = {
        .action = PARSER_ACTION_NONE,
        .new_setpoints = *current_sp /* default copy */
    };

    if (!cmd || strlen(cmd) < 3)
        return result;

    /* Make a working copy of the command */
    char buffer[64];
    strncpy(buffer, cmd, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    /* Remove spaces */
    for (int i = 0; buffer[i]; i++)
        if (buffer[i] == ' ') buffer[i] = '\0';

    /* ---- Parse mode commands ---- */
    if (str_iequals(buffer, "MODE=READ")) {
        result.action = PARSER_ACTION_MODE_READ;
        return result;
    }
    if (str_iequals(buffer, "MODE=ADJUST")) {
        result.action = PARSER_ACTION_MODE_ADJUST;
        return result;
    }

    /* ---- Parse setpoint commands ---- */
    /* Expected format: TEMP=xx,HUM=xx,LUX=xx */
    char *token = strtok(buffer, ",");
    bool updated_any = false;

    while (token != NULL) {
        char *eq = strchr(token, '=');
        if (eq) {
            *eq = '\0';
            const char *key = token;
            const char *val = eq + 1;
            float fval = atof(val);

            /* Match the variable and update */
            if (str_iequals(key, "TEMP")) {
                result.new_setpoints.target_temperature = fval;
                updated_any = true;
            }
            else if (str_iequals(key, "HUM")) {
                result.new_setpoints.target_humidity = fval;
                updated_any = true;
            }
            else if (str_iequals(key, "LUX")) {
                result.new_setpoints.target_light = fval;
                updated_any = true;
            }
        }

        token = strtok(NULL, ",");
    }

    if (updated_any) {
        result.action = PARSER_ACTION_SET_SETPOINTS;
    }

    return result;
}
