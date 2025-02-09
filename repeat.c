#include QMK_KEYBOARD_H
#include "repeat.h"
#include "rgb.h"
#include "custom_keycodes.h"

bool repeat_active = false;
uint16_t repeat_count = 0;
uint16_t repeat_mode_last_keycode = KC_NO;
uint16_t rpt_last_key_timer;


bool handle_repeat(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        // FN + R toggles repeat mode
        if (keycode == KC_RPT) {
            repeat_active = !repeat_active; // Toggle repeat mode
            repeat_count = 0;
            return false;   // Keycode handled, don't process further
        }
        
        if (repeat_active) {
            // Cancel Repeat mode
            if (keycode == KC_ESC) {
                repeat_active = false;
                return false;   // Keycode handled, don't process further
            }

            // Accept digits for repeat_count
            if (keycode >= KC_P1 && keycode <= KC_P0) {
                repeat_count *= 10;
                if (keycode != KC_P0) {
                    repeat_count += keycode - KC_P1 + 1;
                }
                repeat_mode_last_keycode = keycode;
                rpt_last_key_timer = timer_read();
                return false;   // Keycode handled, don't process further
            } else {
                repeat_mode_last_keycode = KC_NO;
            }

            // Keycode is not ESC or a digit, so repeat the keycode
            repeat_active = false; // Disable repeat mode
            if (repeat_count > 0) {
                for (uint8_t i = 0; i < repeat_count; i++) {
                    tap_code(keycode);
                }
                return false;   // Keycode handled, don't process further
            }
        }
    }
    return true; // Keycode not handled, continue processing
}


void repeat_mode_display(void) {
    // Handle RGB display in repeat mode 

    if (repeat_active) {
        // Highlight numpad digits when in repeat mode
        for (uint8_t i = KC_P1; i <= KC_P0; i++) {
            set_keycode_color(i, rgb(RGB_GREEN));
        }

        // Provide visual feedback for the last digit pressed
        if (repeat_mode_last_keycode != KC_NO && timer_elapsed(rpt_last_key_timer) < 500) {
            set_keycode_color(repeat_mode_last_keycode, rgb(RGB_RED));
        }
    }
}
