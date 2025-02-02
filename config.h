#pragma once
#include "../../config.h"

#define TAPPING_TERM 200    // Time in milliseconds (200ms is a common default)

#undef RGB_MATRIX_DEFAULT_MODE
#undef RGB_MATRIX_DEFAULT_HUE
#undef RGB_MATRIX_DEFAULT_SAT
#undef RGB_MATRIX_DEFAULT_VAL

#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_COLOR
#define RGB_MATRIX_DEFAULT_HUE 170
#define RGB_MATRIX_DEFAULT_SAT 128
#define RGB_MATRIX_DEFAULT_VAL 64

#define CAPS_WORD_INVERT_ON_SHIFT