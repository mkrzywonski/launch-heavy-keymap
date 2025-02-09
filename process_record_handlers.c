#include QMK_KEYBOARD_H
#include "process_record_handlers.h"
#include "timer.h"
#include "static_macros.h"
#include "custom_keycodes.h"

extern const rgb_mode_map rgb_modes[];
bool left_shift_pressed;
bool right_shift_pressed;

// Tap Shift to toggle Caps Word
// CTRL + Shift = Caps Lock
bool handle_shift_keys(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_LSFT:  // Left Shift key
        case KC_RSFT:  // Right Shift key
            if (keycode == KC_LSFT) {
                left_shift_pressed = record->event.pressed;
            } else {
                right_shift_pressed = record->event.pressed;
            }
            if (record->event.pressed) {        // Key pressed
                if (left_shift_pressed && right_shift_pressed) {
                    // Tap Both Shift Keys to toggle Caps Word
                    caps_word_toggle();
                }            
            }
            return true;
    }
    return false;
}

bool handle_rgb_controls(uint16_t keycode, keyrecord_t *record, rgb_config_t *layer_rgb) {
    if (record->event.pressed) {
        // FN + 1 to 10, -, =  to change RGB modes
        for (int i = 0; i < sizeof(rgb_modes)/sizeof(rgb_mode_map); i++) {
            if (keycode == rgb_modes[i].keycode) {
                layer_rgb[0].mode = rgb_modes[i].mode;
                rgb_matrix_mode(rgb_modes[i].mode);
                return true;
            }
        }

        // Handle FN Keys that adjust RBG settings
        switch (keycode) {
            case RGB_NEXT:  // FN + Print Screen to increse hue
                rgb_matrix_increase_hue();
                layer_rgb[0].hsv.h = rgb_matrix_get_hue();
                return true;
            case RGB_PREV:  // FN + Num Lock to decrease hue
                rgb_matrix_decrease_hue();
                layer_rgb[0].hsv.h = rgb_matrix_get_hue();
                return true;
            // Brightness controls mirror audio controls on same buttons
            case RGB_TOG:   // FN + Mute to toggle RGB
                layer_rgb[0].enable = !layer_rgb[0].enable;
                rgb_matrix_toggle();
                return true;
            case RGB_VAD:   // FN + Volume Down to decrease brightness
                rgb_matrix_decrease_val();
                layer_rgb[0].hsv.v = rgb_matrix_get_val();
                rgb_matrix_sethsv(layer_rgb[0].hsv.h, layer_rgb[0].hsv.s, layer_rgb[0].hsv.v);
                return true;
            case RGB_VAI:   // FN + Volume Up to increase brightness
                rgb_matrix_increase_val();
                layer_rgb[0].hsv.v = rgb_matrix_get_val();
                rgb_matrix_sethsv(layer_rgb[0].hsv.h, layer_rgb[0].hsv.s, layer_rgb[0].hsv.v);
                return true;
            // Saturation controls mirror audio controls on row above
            case RGB_SAT:   // FN + / to toggle saturation 0<->255
                if (layer_rgb[0].hsv.s == 255) {
                    layer_rgb[0].hsv.s = 0;
                } else {
                    layer_rgb[0].hsv.s = 255;
                }
                rgb_matrix_sethsv(layer_rgb[0].hsv.h, layer_rgb[0].hsv.s, layer_rgb[0].hsv.v);
                return true;
            case RGB_SATD:  // FN + * to decrease saturation
                rgb_matrix_decrease_sat();
                layer_rgb[0].hsv.s = rgb_matrix_get_sat();
                rgb_matrix_sethsv(layer_rgb[0].hsv.h, layer_rgb[0].hsv.s, layer_rgb[0].hsv.v);
                return true;
            case RGB_SATI:  // FN + - to increase saturation
                rgb_matrix_increase_sat();
                layer_rgb[0].hsv.s = rgb_matrix_get_sat();
                rgb_matrix_sethsv(layer_rgb[0].hsv.h, layer_rgb[0].hsv.s, layer_rgb[0].hsv.v);
                return true;
            // Animation speed controls
            case RGB_SPDI:  // FN + Numpad + to increase animation speed
                if (layer_rgb[0].speed + 10 > layer_rgb[0].speed) {
                    layer_rgb[0].speed += 10;
                }
                return true;
            case RGB_SPDD:  // FN + Numpad Enter to decrease animation speed
                if (layer_rgb[0].speed - 10 < layer_rgb[0].speed) {
                    layer_rgb[0].speed -= 10;
                }
                return true;
        }
    }
    return false;
}

// Handle Static Macros
bool handle_macros(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case X_NAME:
                SEND_STRING(X_NAME_TEXT);
                return true;
            case X_FIRST:
                SEND_STRING(X_FIRST_TEXT);
                return true;
            case X_LAST:
                SEND_STRING(X_LAST_TEXT);
                return true;
            case X_ADDR:
                SEND_STRING(X_ADDR_TEXT);
                return true;
            case X_EMAIL:
                SEND_STRING(X_EMAIL_TEXT);
                return true;
            case X_PHONE:
                SEND_STRING(X_PHONE_TEXT);
                return true;
        }
    }
    return false;
}

