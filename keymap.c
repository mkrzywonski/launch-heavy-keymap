#include QMK_KEYBOARD_H
#include "system76_ec.h"
#include "process_record_handlers.h"
#include "timer.h"
#include "static_macros.h"
#include "custom_keycodes.h"
#include "idle_messages.h"
#include "calculator.h"
#include "repeat.h"
#include "rgb.h"
#include <stdio.h>

#define ______ KC_TRNS
#define MSG_DELAY 300 // milliseconds per character
#define IDLE_TIMEOUT_MS 900000 // 15 minutes in milliseconds

static bool lyr1_activated = false;
static bool insert_mode = true;
static int32_t msg_timer = 0;
static bool boot_msg = true;
static uint32_t idle_ms = 0;
static uint16_t last_timer = 0;
char message[256];
enum flash_states { OFF, RISE, ON, FALL, PAUSE };
static enum flash_states flash_state = OFF;

enum layers{ First, Second, Third, Fourth };

const key_override_t backspace_override = ko_make_basic(MOD_MASK_SHIFT, KC_BSPC, KC_DEL);
const key_override_t delete_override = ko_make_basic(MOD_MASK_SHIFT, KC_DEL, KC_INS);

const key_override_t **key_overrides = (const key_override_t *[]){
    &backspace_override,
    &delete_override,
    NULL // Null terminate the array
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  /* Layer 0, default layer
__________________________________________________________________________________________________________________________________  ________  ___________________________________
|        |        |        |        |        |        |        |        |        |        |        |        |        |            ||        || PRINT  |  MUTE  | VOLUME | VOLUME |
| ESC/FN |   F1   |   F2   |   F3   |   F4   |   F5   |   F6   |   F7   |   F8   |   F9   |  F10   |  F11   |  F12   | BACKSPACE  ||  HOME  || SCREEN |        |  DOWN  |   UP   |
|________|________|________|________|________|________|________|________|________|________|________|________|________|____________||________||________|________|________|________|
|        |        |        |        |        |        |        |        |        |        |        |        |        |            ||        || NUM    | NUM    | NUM    | NUM    |
|  ~`    |   1    |   2    |   3    |   4    |   5    |   6    |   7    |   8    |   9    |   0    |  _ -   | =  +   |   DELETE   ||  PGUP  || LOCK   | SLASH  |  *     |  -     |
|________|________|________|________|________|________|________|________|________|________|________|________|________|____________||________||________|________|________|________|
|            |        |        |        |        |        |        |        |        |        |        |  [     |   ]    |        ||        || NUM    | NUM    | NUM    |        |
|     TAB    |   Q    |   W    |   E    |   R    |   T    |   Y    |   U    |   I    |   O    |   P    |  {     |   }    |  |   \ ||  PGDN  ||  7     |  8     |  9     |        |
|____________|________|________|________|________|________|________|________|________|________|________|________|________|________||________||________|________|________| NUM    |
  |            |        |        |        |        |        |        |        |        |        |   ;    |   '    |            |   |        || NUM    | NUM    | NUM    |  +     |
  |     ESC    |   A    |   S    |   D    |   F    |   G    |   H    |   J    |   K    |   L    |   :    |   "    |   ENTER    |   |  END   ||  4     |  5     |  6     |        |
  |____________|________|________|________|________|________|________|________|________|________|________|________|____________|___|________||________|________|________|________|
  |                |        |        |        |        |        |        |        |   ,    |    .   |   /    |            |        |         | NUM    | NUM    | NUM    |        |
  |     SHIFT      |   Z    |   X    |   C    |   V    |   B    |   N    |   M    |   <    |    >   |   ?    |   SHIFT    |   UP   |         |  1     |  2     |  3     |        |
  |________________|________|________|________|________|________|________|________|________|________|________|____________|________|________ |________|________|________| NUM    |
  |            |        |       |        |                 |                 |        |        |             |   |        |        |        ||       NUM       | NUM    | ENTER  |
  |    CTRL    | MACRO1 |  SUP  |  LALT  |    SPACE        |      SPACE      |  RALT  | MACRO2 |    CTRL     |   |  LEFT  |  DOWN  | RIGHT  ||        0        |  .     |        |
  |____________|________|_______|________|_________________|_________________|________|________|_____________|   |________|________|________||_________________|________|________|
*/

  [First] = LAYOUT(
    LT(1,KC_ESC), KC_F1, KC_F2, KC_F3,  KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_BSPC,      KC_HOME,  KC_PSCR, KC_MUTE, KC_VOLD, KC_VOLU,
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_DEL,       KC_PGUP,  KC_NUM,  KC_PSLS, KC_PAST, KC_PMNS,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,  KC_PGDN,  KC_P7,   KC_P8,   KC_P9,   KC_PPLS,
          KC_ESC,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,    KC_ENT,       KC_END,   KC_P4,   KC_P5,   KC_P6,
          KC_LSFT,     KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,    KC_RSFT,    KC_UP,           KC_P1,   KC_P2,   KC_P3,   KC_PENT,
          KC_LCTL, DM_PLY1, KC_LGUI, KC_LALT,    KC_SPC,           KC_SPC,     KC_RALT,  DM_PLY2,   KC_RCTL,          KC_LEFT, KC_DOWN, KC_RGHT,    KC_P0,      KC_PDOT
  ),

   /* Layer 1, function layer
__________________________________________________________________________________________________________________________________  ________  ___________________________________
|        |        |        |        |        |        |        |        |        |        |        |        |        |            || LAYER  ||  RGB   |  RGB   |  RGB   |  RGB   |
|        |  F13   |  F14   |  F15   |  F16   |  F17   |  F18   |  F19   |  F20   |  F21   |  F22   |  F23   |  F24   | BACKSPACE  ||   4    ||  HUE+  | TOGGLE |  DIM   | BRIGHT|
|________|________|________|________|________|________|________|________|________|________|________|________|________|____________||________||________|________|________|________|
|        | SOLID  | CYCLE  | LEFT-> | UP->   | OUT->  | OUT->  | RAINBOW| PIN    | SPIRAL | RAIN   | SPLASH | MULTI  |            || LAYER  ||  RGB   | RGB SAT| RGB SAT| RGB SAT|
|        | COLOR  | ALL    | RIGHT  | DOWN   | IN     | IN DUAL| CHAVRON| WHEEL  |        | DROPS  |        | SPLASH |   REBOOT   ||   3    ||  HUE-  | TOGGLE |    -   |    +   |
|________|________|________|________|________|________|________|________|________|________|________|________|________|____________||________||________|________|________|________|
|            |        |        |        |        |        |        |        |        |        |        |        |        |        || LAYER  ||        |        |        |        |
|            |        |        | X_EMAIL|        | X_PHONE|        |        |KC_IDLE| C_OCT  |  C_PI  |        |        |        ||   2    ||        |        |        | RGB    |
|____________|________|________|________|________|________|________|________|________|________|________|________|________|________||________||________|________|________| EFFECT |
  |            |        |        |        |        |        |        |        |        |        |        |        |            |   | LAYER  ||        |        |        | SPEED  |
  |            | X_ADDR |        | C_DEC  | X_FIRST|        | C_HEX  |        |        | X_LAST |        |        |            |   |   1    || C_HEXD | C_HEXE | C_HEXF |   +    |
  |____________|________|________|________|________|________|________|________|________|________|________|________|____________|___|________||________|________|________|________|
  |                |        |        |        |        |        |        |        |        |        |        |            |        |         |        |        |        |        |
  |    CAPS LOCK   |        |        | C_CLEAR|        | C_BIN  | X_NAME |        |        |        |        |  CAPS LOCK |        |         | C_HEXA | C_HEXB | C_HEXC | RGB    |
  |________________|________|________|________|________|________|________|________|________|________|________|____________|________|________ |________|________|________| EFFECT |
  |            |        |       |        |                 |                 |        | MACRO  |             |   |        |        |        ||                 |        | SPEED  |
  |            |        |QK_BOOT|        |                 |                 |        | RECORD |             |   |        |        |        ||                 |        |   -    |
  |____________|________|_______|________|_________________|_________________|________|________|_____________|   |________|________|________||_________________|________|________|
  * 'QK_BOOT' resets the controller and puts the board into firmware flashing mode. If this key is hit accidentally, just unplug the board
  *        and plug it back in.
  */

  [Second] = LAYOUT(
    _______, KC_F13,  KC_F14,  KC_F15,  KC_F16,  KC_F17,  KC_F18,  KC_F19,  KC_F20,  KC_F21,  KC_F22,  KC_F23,  KC_F24,  _______,      TO(3),    RGB_PREV, RGB_TOG, RGB_VAD, RGB_VAI,
    _______, RGB_01,  RGB_02,  RGB_03,  RGB_04,  RGB_05,  RGB_06,  RGB_07,  RGB_08,  RGB_09,  RGB_10,  RGB_11,  RGB_12,  _______,      TO(2),    RGB_NEXT, RGB_SAT, RGB_SATD, RGB_SATI,
        _______, _______, _______, X_EMAIL, KC_RPT,  X_PHONE,  _______, _______, KC_IDLE, C_OCT,   C_PI,    _______, _______, _______, LAYER1,    _______, _______, _______, RGB_SPDI,
          _______, X_ADDR,  _______, C_DEC,   X_FIRST, _______, C_HEX,   _______, _______,   X_LAST,  _______, _______,    _______,    TO(0),    C_HEXD,  C_HEXE,  C_HEXF,
          KC_CAPS,     _______, _______, C_CLEAR, _______, C_BIN,   X_NAME, _______, _______, _______, _______,    KC_CAPS,   DT_UP,              C_HEXA,  C_HEXB,  C_HEXC,  RGB_SPDD,
          _______, DM_REC1, _______, _______,     _______,          _______,    _______,   DM_REC2,  _______,        _______, DT_DOWN, DT_PRNT,         _______,    _______
  ),

  [Third] = LAYOUT(
    LT(1,KC_ESC), _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      _______,  _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      _______,  KC_SCRL, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,  _______,  _______, _______, _______, _______,
          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,    _______,      TO(0),    _______, _______, _______,
          _______,     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,    _______,  _______,           _______, _______, _______, _______,
          _______, _______, _______, _______,     _______,          _______,    _______,   _______,  _______,        _______, _______, _______,        _______,    _______
  ),

  // Calculator
  [Fourth] = LAYOUT(
    LT(1,KC_ESC), _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      _______,  _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      _______,  KC_SCRL, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,  _______,  _______, _______, _______, _______,
          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,    _______,      TO(0),    _______, _______, _______,
          _______,     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,    _______,  _______,           _______, _______, _______, _______,
          _______, _______, _______, _______,     _______,          _______,    _______,   _______,  _______,        _______, _______, _______,        _______,    _______
  ),
};

// Handle layer changes
layer_state_t layer_state_set_user(layer_state_t state) {
    // Special handling for layer 1 (FN Layer)
    // Needed because MO(1) resets the layer state to 0 after trying to select layer 1
    if (lyr1_activated) {
        lyr1_activated = false;
        state = (layer_state_t)1 << 1;
    }

    // Apply RGB Settings after changes to layer state
    uint8_t layer = get_highest_layer(state);

    if (my_layer_rgb[layer].enable) {
        rgb_matrix_enable();
        rgb_matrix_mode(my_layer_rgb[layer].mode);
        rgb_matrix_set_speed(my_layer_rgb[layer].speed);
        rgb_matrix_sethsv(my_layer_rgb[0].hsv.h, my_layer_rgb[0].hsv.s, my_layer_rgb[0].hsv.v);

        // If we're on a higher layer, overlay its colors for active keys
        if (layer > 0) {
            for (int row = 0; row < MATRIX_ROWS; row++) {
                for (int col = 0; col < MATRIX_COLS; col++) {
                    uint16_t keycode = keymap_key_to_keycode(layer, (keypos_t){.row = row, .col = col});
                    if (keycode != KC_TRNS && keycode != 0) {
                        uint8_t led_index = g_led_config.matrix_co[row][col];
                        if (led_index != NO_LED) {
                            rgb_matrix_set_color(led_index,
                                my_layer_rgb[layer].hsv.h,
                                my_layer_rgb[layer].hsv.s,
                                my_layer_rgb[layer].hsv.v);
                        }
                    }
                }
            }
        }
    } else {
        rgb_matrix_disable();
    }
    return state;
}

// Show active keys on FN layers in a different collor
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint16_t current_timer = timer_read();
    uint8_t layer = get_highest_layer(layer_state);


    // Check for timer rollover and update idle counter
    if (current_timer < last_timer) {
        idle_ms += (65535 - last_timer + current_timer);
        msg_timer -= 65536;
    } else {
        idle_ms += (current_timer - last_timer);
    }
    last_timer = current_timer;

    // Check for idle timeout and display a random message
    if (idle_ms >= IDLE_TIMEOUT_MS) {
        uint8_t msg_index = rand() % NUM_IDLE_MESSAGES;
        const char *msg = (const char *)idle_messages[msg_index];
        flash(msg, true);
        idle_ms = 0;
    }

    // Display Messages
    if (msg_timer == 0) {   // Just powered on
        msg_timer = current_timer;
    }
    if (boot_msg) { // Show boot message
        snprintf(message, sizeof(message), "HELLO %s", X_FIRST_TEXT);
        boot_msg = false;
    }
    if (message[0] == '\0') {   // No Message Mike
        flash_state = OFF;
    } else {                    // Display Message
        switch (flash_state) {
            case OFF:   // Not displaying anything yet
                msg_timer = current_timer;  // Reset timer
                flash_state = RISE;
                break;
            case RISE:
                if (timer_elapsed(msg_timer) < (MSG_DELAY / 2)) {
                    uint16_t r = (timer_elapsed(msg_timer) * 255) / (MSG_DELAY / 2);
                    uint16_t g = 0;
                    uint16_t b = 0;
                    set_keycode_color(char_to_keycode(message[0]), rgb(r, g, b));
                } else {
                    flash_state = ON;
                    msg_timer = current_timer;  // Reset timer
                }
                break;
            case ON:
                if (timer_elapsed(msg_timer) < MSG_DELAY) {
                    uint16_t r = 255;
                    uint16_t g = 0;
                    uint16_t b = 0;
                    set_keycode_color(char_to_keycode(message[0]), rgb(r, g, b));
                } else {
                    flash_state = FALL;
                    msg_timer = current_timer;  // Reset timer
                }
                break;
            case FALL:
                if (timer_elapsed(msg_timer) < (MSG_DELAY / 2)) {
                    uint16_t r = 255 - ((timer_elapsed(msg_timer) * 255) / (MSG_DELAY / 2));
                    uint16_t g = 0;
                    uint16_t b = 0;
                    set_keycode_color(char_to_keycode(message[0]), rgb(r, g, b));
                } else {
                    flash_state = PAUSE;
                    msg_timer = current_timer;  // Reset timer
                    if (message[0] != '\0' && strlen(message) > 0) {
                        memmove(message, message + 1, strlen(message));
                    }                    
                }
                break;
            case PAUSE:
                if (timer_elapsed(msg_timer) > (MSG_DELAY / 3)) {
                    flash_state = OFF;
                }
                break;
        }
    }

    // Add caps lock indicator
    if (host_keyboard_led_state().caps_lock) {
        set_keycode_color(KC_CAPS, rgb(RGB_RED));
        set_keycode_color(KC_LSFT, rgb(RGB_RED));
        set_keycode_color(KC_RSFT, rgb(RGB_RED));
    }

    // Add caps word indicator for shift keys
    if (is_caps_word_on()) {
        set_keycode_color(KC_LSFT, rgb(RGB_GOLD));
        set_keycode_color(KC_RSFT, rgb(RGB_GOLD));
    }

    calculator_rgb_display();
    repeat_mode_display();

    // Make delete key red when not n insert mode
    if (!insert_mode) {
        set_keycode_color(KC_DEL, rgb(RGB_RED));
    }

    if (layer > 0 && my_layer_rgb[layer].enable) {
        // Convert layer 0's HSV to RGB
        RGB base_rgb = hsv_to_rgb((HSV){
            .h = my_layer_rgb[0].hsv.h,
            .s = my_layer_rgb[0].hsv.s,
            .v = my_layer_rgb[0].hsv.v
        });

        // Set base layer colors
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            for (uint8_t col = 0; col < MATRIX_COLS; col++) {
                uint8_t index = g_led_config.matrix_co[row][col];
                if (index >= led_min && index < led_max && index != NO_LED) {
                    rgb_matrix_set_color(index, base_rgb.r, base_rgb.g, base_rgb.b);
                }
            }
        }

        // Convert current layer's HSV to RGB
        RGB layer_rgb = hsv_to_rgb((HSV){
            .h = my_layer_rgb[layer].hsv.h,
            .s = my_layer_rgb[layer].hsv.s,
            .v = my_layer_rgb[layer].hsv.v
        });

        // Overlay active keys with current layer colors
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            for (uint8_t col = 0; col < MATRIX_COLS; col++) {
                uint16_t keycode = keymap_key_to_keycode(layer, (keypos_t){.row = row, .col = col});
                if (keycode != KC_TRNS && keycode != 0) {
                    uint8_t index = g_led_config.matrix_co[row][col];
                    if (index >= led_min && index < led_max && index != NO_LED) {
                        rgb_matrix_set_color(index, layer_rgb.r, layer_rgb.g, layer_rgb.b);
                    }
                }
            }
        }
    }
    return false;
}

void keyboard_post_init_kb(void) {
    // Your one-time startup code goes here
    // For example:
    srand(timer_read());
}

// Handle Keyboard Input
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    srand(timer_read() + (keycode * timer_read32()));
    idle_ms = 0; // Reset idle timer on keypress

    // Enable Caps Word / Caps Lock
    if (handle_shift_keys(keycode, record)) {
        return true;
    }

    // Handle RGB controls
    if (handle_rgb_controls(keycode, record, my_layer_rgb)) {
        return false;
    }
    
    // Handle Macros
    if (handle_macros(keycode, record)) {
        return false;
    }
        
    // Handle FN + PG_DN to activate Layer 1
    if (record->event.pressed && keycode == LAYER1) {
        lyr1_activated = true;
        return false;
    }

    // Track state of INSERT/OVERWRITE mode for LED indicator
    if (record->event.pressed) {
        switch (keycode) {
            case KC_INS:
                insert_mode = !insert_mode;
                return true;
            case KC_DEL:
                if (get_mods() & MOD_MASK_SHIFT) {
                    insert_mode = !insert_mode;
                }
                return true;
        }
    }

    // Numpad Calculator
    if (!handle_calculator_input(keycode, record)) {
        return false;    
    }

    if (!handle_repeat(keycode, record)) {
        return false;    
    }

    if (keycode == KC_IDLE) {
        if(record->event.pressed) {
            // Flash idle-timer message
            idle_ms = IDLE_TIMEOUT_MS;
        }
        return false;
    }

    if (keycode == QK_BOOT) {
        if (record->event.pressed) {
            system76_ec_unlock();  // Unlock the bootloader
            bootloader_jump();     // Immediately enter bootloader mode
        }
        return false;
    }

    if (keycode == Q_RESET) {
        if (record->event.pressed) {
            bootloader_jump();     // Immediately enter bootloader mode
        }
        return false;
    }

    return true;
}
