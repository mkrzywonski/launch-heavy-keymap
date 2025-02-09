#pragma once
#include QMK_KEYBOARD_H
#include "custom_keycodes.h"

bool handle_shift_keys(uint16_t keycode, keyrecord_t *record);
bool handle_rgb_controls(uint16_t keycode, keyrecord_t *record, rgb_config_t *layer_rgb);
bool handle_macros(uint16_t keycode, keyrecord_t *record);
extern bool left_shift_pressed;
extern bool right_shift_pressed;

typedef struct {
    uint16_t keycode;
    uint8_t mode;
} rgb_mode_map;

static const rgb_mode_map rgb_modes[] = {
    {RGB_01, RGB_MATRIX_SOLID_COLOR},
    {RGB_02, RGB_MATRIX_CYCLE_ALL},
    {RGB_03, RGB_MATRIX_CYCLE_LEFT_RIGHT},
    {RGB_04, RGB_MATRIX_CYCLE_UP_DOWN},
    {RGB_05, RGB_MATRIX_CYCLE_OUT_IN},
    {RGB_06, RGB_MATRIX_CYCLE_OUT_IN_DUAL},
    {RGB_07, RGB_MATRIX_RAINBOW_MOVING_CHEVRON},
    {RGB_08, RGB_MATRIX_CYCLE_PINWHEEL},
    {RGB_09, RGB_MATRIX_CYCLE_SPIRAL},
    {RGB_10, RGB_MATRIX_RAINDROPS},
    {RGB_11, RGB_MATRIX_SPLASH},
    {RGB_12, RGB_MATRIX_MULTISPLASH}
};
