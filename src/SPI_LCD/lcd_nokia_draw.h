#include <stdio.h>
#include <stdint.h>

typedef enum{
    pass_code = 0,
    zero_division_error,
    out_of_bounds_error
}error_code;

#define PIXEL_X_MAX_LIMIT 83
#define PIXEL_X_MIN_LIMIT 0
#define PIXEL_Y_MAX_LIMIT 47
#define PIXEL_Y_MIN_LIMIT 0

uint8_t drawline(float x0, float y0, float x1, float y1, uint8_t mindots);