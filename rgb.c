#include "rgb.h"
#include QMK_KEYBOARD_H

extern char message[256];

// Flash messages to the keyboard's RGB matrix
void flash(const char* text, bool append) {
    if (append) {
        if (message[0] != '\0') {
            strncat(message, " ", sizeof(message) - strlen(message) - 1);
        }
        strncat(message, text, sizeof(message) - strlen(message) - 1);
    } else {
        strncpy(message, text, sizeof(message) - 1);
        message[sizeof(message) - 1] = '\0';
    }
}



void set_keycode_color(uint16_t keycode, uint8_t r, uint8_t g, uint8_t b) {
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            if (keymap_key_to_keycode(0, (keypos_t){.row = row, .col = col}) == keycode) {
                uint8_t led_index = g_led_config.matrix_co[row][col];
                if (led_index != NO_LED) {
                    rgb_matrix_set_color(led_index, r, g, b);
                }
            }
        }
    }
}

uint16_t char_to_keycode(char c) {
    if (c >= 'a' && c <= 'z') {
        return KC_A + (c - 'a');
    }
    if (c >= 'A' && c <= 'Z') {
        return KC_A + (c - 'A');
    }
    if (c == '0') {
        return KC_0;
    }
    if (c >= '1' && c <= '9') {
        return KC_1 + (c - '1');
    }

    // Add more special characters as needed
    switch (c) {
        case ' ': return KC_SPACE;
        case '.': return KC_DOT;
        case ',': return KC_COMMA;
        case '+': return KC_PPLS;
        case '-': return KC_PMNS;
        case '*': return KC_PAST;
        case '/': return KC_PSLS;
        case '=': return KC_PEQL;
        case '[': return KC_LBRC;
        case ']': return KC_RBRC;
        case '\\': return KC_BSLS;
        case ';': return KC_SCLN;
        case '\'': return KC_QUOT;
        case '`': return KC_GRV;
        default: return KC_NO;
    }
}
