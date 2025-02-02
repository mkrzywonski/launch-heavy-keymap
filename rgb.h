#pragma once
#include QMK_KEYBOARD_H

void set_keycode_color(uint16_t keycode, uint32_t rgb);
uint16_t char_to_keycode(char c);
void flash(const char* text, bool append);
uint32_t rgb(uint8_t r, uint8_t g, uint8_t b);

extern rgb_config_t my_layer_rgb[DYNAMIC_KEYMAP_LAYER_COUNT];
