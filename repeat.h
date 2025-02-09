#pragma once
#include "custom_keycodes.h"
#include "rgb.h"

extern bool repeat_active;
extern uint16_t repeat_count;

bool handle_repeat(uint16_t keycode, keyrecord_t *record);
void repeat_mode_display(void);