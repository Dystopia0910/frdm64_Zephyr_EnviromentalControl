/**
 * @file display_manager.h
 * @brief Display manager interface for Nokia LCD
 */

#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <stdbool.h>

/* System mode enumeration */
typedef enum {
    MODE_READ_ONLY,     /* Read-only mode, no adjustments allowed */
    MODE_ADJUSTING      /* Adjustment mode, setpoints can be changed */
} system_mode_t;

/* Display data structure */
typedef struct {
    float temperature;   /* Temperature in Celsius */
    float light_level;   /* Light level in lux */
    float humidity;      /* Humidity percentage */
    system_mode_t mode;  /* Current system mode */
} display_data_t;

/* Display initialization and control */
int display_init(void);
void display_clear(void);
bool display_is_initialized(void);

/* Logo display functions */
void display_show_logo(void);
void display_show_iteso_logo(void);

/* Main display update functions */
void display_update(const display_data_t *data);
void display_update_table_format(const display_data_t *data);

/* Special display modes */
void display_setpoints(float temp_setpoint, float light_setpoint, float humid_setpoint);
void display_draw_graph(const float *values, uint8_t count, float max_value, uint8_t row);

/* Status and message displays */
void display_message(uint8_t line, const char *message);
void display_error(const char *error_message);
void display_bluetooth_status(bool connected);

#endif /* DISPLAY_MANAGER_H */