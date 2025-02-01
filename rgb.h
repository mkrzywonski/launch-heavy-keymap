#pragma once

void set_keycode_color(uint16_t keycode, uint8_t r, uint8_t g, uint8_t b);
uint16_t char_to_keycode(char c);
void flash(const char* text, bool append);
