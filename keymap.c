#include QMK_KEYBOARD_H
#include "timer.h"
#include "x_strings.h" // rename example_x_strings.h to x_strings.h
#define ______ KC_TRNS

static uint16_t shift_timer;
static bool shift_pressed = false;
static bool lyr1_activated = false;
static bool insert_mode = true;

enum layers{ First, Second, Third, Fourth };
enum custom_keycodes {
    RGB_01 = SAFE_RANGE, RGB_02, RGB_03, RGB_04, RGB_05, RGB_06, RGB_07, RGB_08, RGB_09, RGB_10, RGB_11, RGB_12, 
    RGB_NEXT, RGB_PREV, RGB_SATI, RGB_SATD, RGB_SAT, RGB_SPDI, RGB_SPDD, LAYER1,
    X_NAME, X_FIRST, X_LAST, X_ADDR, X_EMAIL, X_PHONE
};

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
|  ESC   |   F1   |   F2   |   F3   |   F4   |   F5   |   F6   |   F7   |   F8   |   F9   |  F10   |  F11   |  F12   |  DELETE    ||  HOME  || SCREEN |        |  DOWN  |   UP   |
|________|________|________|________|________|________|________|________|________|________|________|________|________|____________||________||________|________|________|________|
|        |        |        |        |        |        |        |        |        |        |        |        |        |            ||        || NUM    | NUM    | NUM    | NUM    |
|  ~`    |   1    |   2    |   3    |   4    |   5    |   6    |   7    |   8    |   9    |   0    |  _ -   | =  +   | BACKSPACE  ||  PGUP  || LOCK   | SLASH  |  *     |  -     |
|________|________|________|________|________|________|________|________|________|________|________|________|________|____________||________||________|________|________|________|
|            |        |        |        |        |        |        |        |        |        |        |  [     |   ]    |        ||        || NUM    | NUM    | NUM    |        |
|     TAB    |   Q    |   W    |   E    |   R    |   T    |   Y    |   U    |   I    |   O    |   P    |  {     |   }    |  |   \ ||  PGDN  ||  7     |  8     |  9     |        |
|____________|________|________|________|________|________|________|________|________|________|________|________|________|________||________||________|________|________| NUM    |
  |            |        |        |        |        |        |        |        |        |        |   ;    |   '    |            |   |        || NUM    | NUM    | NUM    |  +     |
  |    CAPS    |   A    |   S    |   D    |   F    |   G    |   H    |   J    |   K    |   L    |   :    |   "    |   ENTER    |   |  END   ||  4     |  5     |  6     |        |
  |____________|________|________|________|________|________|________|________|________|________|________|________|____________|___|________||________|________|________|________|
  |                |        |        |        |        |        |        |        |   ,    |    .   |   /    |            |        |         | NUM    | NUM    | NUM    |        |
  |     SHIFT      |   Z    |   X    |   C    |   V    |   B    |   N    |   M    |   <    |    >   |   ?    |   SHIFT    |   UP   |         |  1     |  2     |  3     |        |
  |________________|________|________|________|________|________|________|________|________|________|________|____________|________|________ |________|________|________| NUM    |
  |            |        |       |        |                 |                 |        |        |             |   |        |        |        ||       NUM       | NUM    | ENTER  |
  |    CTRL    |   FN   |  SUP  |  LALT  |    SPACE        |      SPACE      |  RALT  |  MACRO |    CTRL     |   |  LEFT  |  DOWN  | RIGHT  ||        0        |  .     |        |
  |____________|________|_______|________|_________________|_________________|________|________|_____________|   |________|________|________||_________________|________|________|
*/

  [First] = LAYOUT(
    KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,       KC_HOME,  KC_PSCR, KC_MUTE, KC_VOLD, KC_VOLU,
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,      KC_PGUP,  KC_NUM,  KC_PSLS, KC_PAST, KC_PMNS,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,  KC_PGDN,  KC_P7,   KC_P8,   KC_P9,   KC_PPLS,
          KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,    KC_ENT,       KC_END,   KC_P4,   KC_P5,   KC_P6,
          KC_LSFT,     KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,    KC_RSFT,    KC_UP,           KC_P1,   KC_P2,   KC_P3,   KC_PENT,
          KC_LCTL, MO(1), KC_LGUI,   KC_LALT,     KC_SPC,           KC_SPC,     KC_RALT,   DM_PLY1,   KC_RCTL,          KC_LEFT, KC_DOWN, KC_RGHT,        KC_P0,      KC_PDOT
  ),

   /* Layer 1, function layer
__________________________________________________________________________________________________________________________________  ________  ___________________________________
|        |        |        |        |        |        |        |        |        |        |        |        |        |            || LAYER  ||  RGB   |  RGB   |  RGB   |  RGB   |
| QK_BOOT|  F13   |  F14   |  F15   |  F16   |  F17   |  F18   |  F19   |  F20   |  F21   |  F22   |  F23   |  F24   |    INS     ||   4    ||  HUE+  | TOGGLE |  DIM   | BRIGHT|
|________|________|________|________|________|________|________|________|________|________|________|________|________|____________||________||________|________|________|________|
|        | SOLID  | CYCLE  | LEFT-> | UP->   | OUT->  | OUT->  | RAINBOW| PIN    | SPIRAL | RAIN   | SPLASH | MULTI  |            || LAYER  ||  RGB   | RGB SAT| RGB SAT| RGB SAT|
|        | COLOR  | ALL    | RIGHT  | DOWN   | IN     | IN DUAL| CHAVRON| WHEEL  |        | DROPS  |        | SPLASH |    DEL     ||   3    ||  HUE-  | TOGGLE |    -   |    +   |
|________|________|________|________|________|________|________|________|________|________|________|________|________|____________||________||________|________|________|________|
|            |        |        |        |        |        |        |        |        |        |        |        |        |        || LAYER  ||        |  PREV  |        |        |
|            |        |        | X_EMAIL|        |        |        |        |        |        | X_PHONE|        |        |        ||   2    ||        |  TRACK |        | RGB    |
|____________|________|________|________|________|________|________|________|________|________|________|________|________|________||________||________|________|________| EFFECT |
  |            |        |        |        |        |        |        |        |        |        |        |        |            |   | LAYER  ||        |  PLAY  |        | SPEED  |
  |            | X_ADDR |        |        | X_FIRST|        |        |        |        | X_LAST |        |        |            |   |   1    || REWIND |  PAUSE | FORWARD|   +    |
  |____________|________|________|________|________|________|________|________|________|________|________|________|____________|___|________||________|________|________|________|
  |                |        |        |        |        |        |        |        |        |        |        |            |        |         |        |  NEXT  |        |        |
  |    CAPS LOCK   |        |        |        |        |        | X_NAME |        |        |        |        |  CAPS LOCK |        |         |        |  TRACK |        | RGB    |
  |________________|________|________|________|________|________|________|________|________|________|________|____________|________|________ |________|________|________| EFFECT |
  |            |        |       |        |                 |                 |        | MACRO  |             |   |        |        |        ||                 |        | SPEED  |
  |            |        |       |        |                 |                 |        | RECORD |             |   |        |        |        ||                 |  STOP  |   -    |
  |____________|________|_______|________|_________________|_________________|________|________|_____________|   |________|________|________||_________________|________|________|
  * 'QK_BOOT' resets the controller and puts the board into firmware flashing mode. If this key is hit accidentally, just unplug the board
  *        and plug it back in.
  */

  [Second] = LAYOUT(
    QK_BOOT, KC_F13,  KC_F14,  KC_F15,  KC_F16,  KC_F17,  KC_F18,  KC_F19,  KC_F20,  KC_F21,  KC_F22,  KC_F23,  KC_F24,  KC_INS,       TO(3),    RGB_PREV, RGB_TOG, RGB_VAD, RGB_VAI,
    _______, RGB_01,  RGB_02,  RGB_03,  RGB_04,  RGB_05,  RGB_06,  RGB_07,  RGB_08,  RGB_09,  RGB_10,  RGB_11,  RGB_12,  KC_DEL,       TO(2),    RGB_NEXT, RGB_SAT, RGB_SATD, RGB_SATI,
        _______, _______, _______, X_EMAIL, _______, _______, _______, _______, _______, _______,  X_PHONE, _______, _______, _______, LAYER1,   _______, KC_MPRV, _______, RGB_SPDI,
          _______, X_ADDR,  _______, _______, X_FIRST, _______, _______, _______, _______,   X_LAST,  _______, _______,    _______,    TO(0),    KC_MRWD, KC_MPLY, KC_MFFD,
          KC_CAPS,     _______, _______, _______, _______, _______, X_NAME, _______, _______, _______, _______,    KC_CAPS,  _______,           _______, KC_MNXT, _______, RGB_SPDD,
          _______, _______, _______, _______,     _______,          _______,    _______,   DM_REC1,  _______,        _______, _______, _______,         _______,    KC_MSTP
  ),

  [Third] = LAYOUT(
    KC_ESC,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      _______,  _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      _______,  KC_SCRL, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,  _______,  _______, _______, _______, _______,
          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,    _______,      _______,  _______, _______, _______,
          _______,     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,    _______,  _______,           _______, _______, _______, _______,
          _______, _______, _______, _______,     _______,          _______,    _______,   _______,  _______,        _______, _______, _______,        _______,    _______
  ),

  [Fourth] = LAYOUT(
    KC_ESC,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      _______,  _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,      _______,  _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,  _______,  _______, _______, _______, _______,
          _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,    _______,      _______,  _______, _______, _______,
          _______,     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,    _______,  _______,           _______, _______, _______, _______,
          _______, _______, _______, _______,     _______,          _______,    _______,   _______,  _______,        _______, _______, _______,        _______,    _______
  ),
};

// Define RGB backlight defaults for each layer
rgb_config_t my_layer_rgb[DYNAMIC_KEYMAP_LAYER_COUNT] = {
    // Layer 0 (Base Layer)
    {
        .enable = 1,
        .mode = RGB_MATRIX_DEFAULT_MODE, // See config.h to change defaults
        .hsv = {
            .h = RGB_MATRIX_DEFAULT_HUE,
            .s = RGB_MATRIX_DEFAULT_SAT,
            .v = RGB_MATRIX_DEFAULT_VAL,
        },
        .speed = RGB_MATRIX_DEFAULT_SPD,
        .flags = LED_FLAG_KEYLIGHT,
    },
    // Layer 1 (FN Layer) Red
    {
        .enable = 1,
        .mode = RGB_MATRIX_CUSTOM_raw_rgb,
        .hsv = {
            .h = HSV_RED,
            .s = 255,
            .v = RGB_MATRIX_MAXIMUM_BRIGHTNESS,
        },
        .speed = RGB_MATRIX_DEFAULT_SPD,
        .flags = LED_FLAG_KEYLIGHT,
    },
    // Layer 2 (Unused) Green
    {
        .enable = 1,
        .mode = RGB_MATRIX_CUSTOM_raw_rgb,
        .hsv = {
            .h = HSV_GREEN,
            .s = 255,
            .v = RGB_MATRIX_MAXIMUM_BRIGHTNESS,
        },
        .speed = RGB_MATRIX_DEFAULT_SPD,
        .flags = LED_FLAG_KEYLIGHT,
    },
    // Layer 3 (Unused) Blue
    {
        .enable = 1,
        .mode = RGB_MATRIX_CUSTOM_raw_rgb,
        .hsv = {
            .h = HSV_GOLD,
            .s = 255,
            .v = RGB_MATRIX_MAXIMUM_BRIGHTNESS,
        },
        .speed = RGB_MATRIX_DEFAULT_SPD,
        .flags = LED_FLAG_KEYLIGHT,
    },        
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
    uint8_t layer = get_highest_layer(layer_state);

    // Add caps lock indicator
    if (host_keyboard_led_state().caps_lock) {
        uint8_t caps_led_index = g_led_config.matrix_co[3][0]; // Row 3, Col 0 for caps lock
        if (caps_led_index >= led_min && caps_led_index < led_max && caps_led_index != NO_LED) {
            rgb_matrix_set_color(caps_led_index, RGB_RED); // Set caps lock key to red
        }
    }
    
    // Add caps word indicator for shift keys
    if (is_caps_word_on()) {
        uint8_t left_shift_led = g_led_config.matrix_co[4][0];  // Row 4, Col 0 for left shift
        uint8_t right_shift_led = g_led_config.matrix_co[4][11]; // Row 4, Col 11 for right shift
        
        if (left_shift_led >= led_min && left_shift_led < led_max && left_shift_led != NO_LED) {
            rgb_matrix_set_color(left_shift_led, RGB_RED);
        }
        if (right_shift_led >= led_min && right_shift_led < led_max && right_shift_led != NO_LED) {
            rgb_matrix_set_color(right_shift_led, RGB_RED);
        }
    }

    // Make numlock key red when disabled
    if (!host_keyboard_led_state().num_lock) {
        uint8_t numlock_led = g_led_config.matrix_co[7][12]; // Row 1, Col 15 for numlock
        if (numlock_led >= led_min && numlock_led < led_max && numlock_led != NO_LED) {
            rgb_matrix_set_color(numlock_led, RGB_RED);
        }
    }

    // Make delete key red when not n insert mode
    if (!insert_mode) {
        uint8_t del_led = g_led_config.matrix_co[0][13]; // Position K0D in matrix for DEL key
        if (del_led >= led_min && del_led < led_max && del_led != NO_LED) {
            rgb_matrix_set_color(del_led, RGB_RED);
        }
    }    

    if (layer > 0 && my_layer_rgb[layer].enable) {
        // Convert layer 0's HSV to RGB
        RGB base_rgb = hsv_to_rgb((HSV){
            .h = my_layer_rgb[0].hsv.h,
            .s = my_layer_rgb[0].hsv.s,
            .v = my_layer_rgb[0].hsv.v
        });
        
        // Set base layer colors first
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

// Handle Keyboard Input
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Enable CapsWord on Shift key tap
    switch (keycode) {
        case KC_LSFT:  // Left Shift key
        case KC_RSFT:  // Right Shift key
            if (record->event.pressed) {
                // Key is pressed, start the timer
                shift_pressed = true;
                shift_timer = timer_read();
            } else {
                // Key is released, check the duration
                shift_pressed = false;
                if (timer_elapsed(shift_timer) < TAPPING_TERM) {
                    // Tap detected, toggle Caps Word
                    caps_word_toggle();
                }
            }
            return true;
    }    

    // Handle general keypresses
    if (record->event.pressed) {
        switch (keycode) {
            // FN + Number row to switch RGB effects
            case RGB_01:
                my_layer_rgb[0].mode = RGB_MATRIX_SOLID_COLOR;
                rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR); // Apply the mode immediately
                return false;
            case RGB_02:
                my_layer_rgb[0].mode = RGB_MATRIX_CYCLE_ALL;
                rgb_matrix_mode(RGB_MATRIX_CYCLE_ALL); // Apply the mode immediately
                return false;                
            case RGB_03:
                my_layer_rgb[0].mode = RGB_MATRIX_CYCLE_LEFT_RIGHT;
                rgb_matrix_mode(RGB_MATRIX_CYCLE_LEFT_RIGHT); // Apply the mode immediately
                return false;
            case RGB_04:
                my_layer_rgb[0].mode = RGB_MATRIX_CYCLE_UP_DOWN;
                rgb_matrix_mode(RGB_MATRIX_CYCLE_UP_DOWN); // Apply the mode immediately
                return false;
            case RGB_05:
                my_layer_rgb[0].mode = RGB_MATRIX_CYCLE_OUT_IN;
                rgb_matrix_mode(RGB_MATRIX_CYCLE_OUT_IN); // Apply the mode immediately
                return false;
            case RGB_06:
                my_layer_rgb[0].mode = RGB_MATRIX_CYCLE_OUT_IN_DUAL;
                rgb_matrix_mode(RGB_MATRIX_CYCLE_OUT_IN_DUAL); // Apply the mode immediately
                return false;
            case RGB_07:
                my_layer_rgb[0].mode = RGB_MATRIX_RAINBOW_MOVING_CHEVRON;
                rgb_matrix_mode(RGB_MATRIX_RAINBOW_MOVING_CHEVRON); // Apply the mode immediately
                return false;
            case RGB_08:
                my_layer_rgb[0].mode = RGB_MATRIX_CYCLE_PINWHEEL;
                rgb_matrix_mode(RGB_MATRIX_CYCLE_PINWHEEL); // Apply the mode immediately
                return false;
            case RGB_09:
                my_layer_rgb[0].mode = RGB_MATRIX_CYCLE_SPIRAL;
                rgb_matrix_mode(RGB_MATRIX_CYCLE_SPIRAL); // Apply the mode immediately
                return false;
            case RGB_10:
                my_layer_rgb[0].mode = RGB_MATRIX_RAINDROPS;
                rgb_matrix_mode(RGB_MATRIX_RAINDROPS); // Apply the mode immediately
                return false;
            case RGB_11:
                my_layer_rgb[0].mode = RGB_MATRIX_SPLASH;
                rgb_matrix_mode(RGB_MATRIX_SPLASH); // Apply the mode immediately
                return false;
            case RGB_12:
                my_layer_rgb[0].mode = RGB_MATRIX_MULTISPLASH;
                rgb_matrix_mode(RGB_MATRIX_MULTISPLASH); // Apply the mode immediately
                return false;

            // Print Screen / NumLock to cycle through RGB Colors (Use FN+1 for Single Color Mode)
            case RGB_NEXT:
                rgb_matrix_increase_hue();
                my_layer_rgb[0].hsv.h = rgb_matrix_get_hue();
                return false;
            case RGB_PREV:
                rgb_matrix_decrease_hue();
                my_layer_rgb[0].hsv.h = rgb_matrix_get_hue();
                return false;
            
            // FN + Mute / Volume Down / Volume Up to adjust RGB Brightness
            case RGB_TOG:
                my_layer_rgb[0].enable = !my_layer_rgb[0].enable;
                rgb_matrix_toggle();
                return false;
            case RGB_VAD:
                rgb_matrix_decrease_val();
                my_layer_rgb[0].hsv.v = rgb_matrix_get_val();
                rgb_matrix_sethsv(my_layer_rgb[0].hsv.h, my_layer_rgb[0].hsv.s, my_layer_rgb[0].hsv.v);
                return false;
            case RGB_VAI:
                rgb_matrix_increase_val();
                my_layer_rgb[0].hsv.v = rgb_matrix_get_val();
                rgb_matrix_sethsv(my_layer_rgb[0].hsv.h, my_layer_rgb[0].hsv.s, my_layer_rgb[0].hsv.v);
                return false;

            // FN + /, *, - to adjust RGB Color Saturation
            case RGB_SAT:
                if (my_layer_rgb[0].hsv.s == 255) {
                    my_layer_rgb[0].hsv.s = 0;
                } else {
                    my_layer_rgb[0].hsv.s = 255;
                }
                rgb_matrix_sethsv(my_layer_rgb[0].hsv.h, my_layer_rgb[0].hsv.s, my_layer_rgb[0].hsv.v);
                return false;
            case RGB_SATI:
                rgb_matrix_increase_sat();
                my_layer_rgb[0].hsv.s = rgb_matrix_get_sat();
                rgb_matrix_sethsv(my_layer_rgb[0].hsv.h, my_layer_rgb[0].hsv.s, my_layer_rgb[0].hsv.v);
                return false;
            case RGB_SATD:
                rgb_matrix_decrease_sat();
                my_layer_rgb[0].hsv.s = rgb_matrix_get_sat();
                rgb_matrix_sethsv(my_layer_rgb[0].hsv.h, my_layer_rgb[0].hsv.s, my_layer_rgb[0].hsv.v);
                return false;

            // FN +/Enter to adjust RGB Animation Speed
            case RGB_SPDI:
                rgb_matrix_increase_speed();
                my_layer_rgb[0].speed = rgb_matrix_get_speed();
                return false;
            case RGB_SPDD:
                rgb_matrix_decrease_speed();
                my_layer_rgb[0].speed = rgb_matrix_get_speed();
                return false;

            // Handle FN + PG_DN to activate Layer 1
            case LAYER1:
                lyr1_activated = true;
                return false;

            // Handle Static Macros
            // X_NAME, X_FIRST, X_LAST, X_ADDR, X_EMAIL, X_PHONE
            case X_NAME:
                SEND_STRING(X_NAME_TEXT);
                return false;
            case X_FIRST:
                SEND_STRING(X_FIRST_TEXT);
                return false;
            case X_LAST:
                SEND_STRING(X_LAST_TEXT);
                return false;
            case X_ADDR:
                SEND_STRING(X_ADDR_TEXT);
                return false;
            case X_EMAIL:
                SEND_STRING(X_EMAIL_TEXT);
                return false;
            case X_PHONE:
                SEND_STRING(X_PHONE_TEXT);
                return false;
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
    return true;
}