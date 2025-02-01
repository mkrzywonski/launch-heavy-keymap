#include QMK_KEYBOARD_H
#include "calculator.h"
#include "rgb.h"
#include "custom_keycodes.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

double calculator_current_value = 0.0;
double calculator_operand = 0.0;
char calculator_operator = '+';
int calculator_base = 10;
char calculator_input_buffer[BUFFER_SIZE] = {0};
int calculator_buf_index = 0;
uint16_t calculator_last_keycode = KC_NO;
bool calculator_last_keycode_valid = true;
bool last_key_status_active = false;
uint16_t last_key_status_timer;
bool clear_calc_status_active = false;
uint16_t clear_calc_status_timer;
bool calculator_print_active = false;

bool handle_calculator_input(uint16_t keycode, keyrecord_t *record) {
    // If Num Lock is enabled, do nothing and return
    if (host_keyboard_led_state().num_lock) {
        return true;    // Not for us, continue processing keycode
    }

    // If keycode is not a numpad key, do nothing and return
    if (!is_calculator_keycode(keycode)) {
        return true;    // Not for us, continue processing keycode
    }

    // Handle calculator input
    if (record->event.pressed) {
        // Since the keyboard has no display, we'll use the keyboard's
        // RGB matrix to provide visual feedback for input and results.
        // We will store the last key pressed and color ir red or green
        // to provide status information.
        calculator_last_keycode = keycode;    // Store the last key pressed
        calculator_last_keycode_valid = true; // default to true
        last_key_status_active = true;
        last_key_status_timer = timer_read();

        // Clear
        if (keycode == C_CLEAR) {
            clear_calculator();
            return false;   // Keycode handled, don't process further
        }
        if (keycode == KC_P0 && get_mods() & (MOD_MASK_SHIFT | MOD_MASK_CTRL | MOD_MASK_ALT)) {
            clear_calculator();
            return false;   // Keycode handled, don't process further
        }


        // Print Mode
        if (keycode == KC_PSCR) {
            calculator_last_keycode = KC_NO;
            calculator_print_active = !calculator_print_active;
            return false;   // Keycode handled, don't process further
        }

        // Handle numeric inputs
        if (!handle_digits(keycode)) {
            return false;   // Keycode handled, don't process further
        }

        // Handle base conversion keys: decimal, binary, octal, hexadecimal
        if (keycode == C_DEC || keycode == C_BIN || keycode == C_OCT || keycode == C_HEX) {
            handle_base_conversion(keycode);
            return false;   // Keycode handled, don't process further
        }

        // Handle Operators: + - * /
        if (keycode == KC_PPLS || keycode == KC_PMNS || keycode == KC_PAST || keycode == KC_PSLS) {
            handle_operators(keycode);
            return false;   // Keycode handled, don't process further
        }

        // Enter key (=)
        if (keycode == KC_PENT) {
            if (get_mods() & (MOD_MASK_SHIFT | MOD_MASK_CTRL | MOD_MASK_ALT)) {
                print_result();
            } else {
                handle_enter(keycode);
            }
            return false;   // Keycode handled, don't process further
        }

    }

    return true; // Keycode not handled, continue processing
}

void print_result(void) {
    char result[BUFFER_SIZE] = {0};
    clear_mods();
    double_to_string(calculator_current_value, calculator_base, result, sizeof(result), 9);
    SEND_STRING(result);
}

bool is_calculator_keycode(uint16_t keycode) {
    return ((keycode >= KC_PSLS && keycode <= KC_PDOT) ||
        (keycode >= C_CLEAR && keycode <= C_PI) ||
        (keycode == KC_PSCR));
}

void clear_calculator(void) {
    if (calculator_input_buffer[0] != '\0' || calculator_operand != 0.0) {
        // Clear Entry
        calculator_buf_index = 0;
        calculator_input_buffer[0] = '\0';
        calculator_operand = 0.0;
    } else {
        // Clear All
        calculator_current_value = 0.0;
        calculator_operator = '+';
    }
    clear_calc_status_active = true;
    clear_calc_status_timer = timer_read();
}

const char* char_to_str(char c) {
    static char str[2];
    str[0] = c;
    str[1] = '\0';
    return str;
}

bool handle_digits(uint16_t keycode) {
    uint8_t mod_state = get_mods();
    // Accept floating point numbers
    if (keycode == KC_PDOT) {
        // Only add the decimal point if one doesn't already exist
        if (strchr(calculator_input_buffer, '.') == NULL) {
            calculator_input_buffer[calculator_buf_index++] = '.';
            calculator_input_buffer[calculator_buf_index] = '\0';
            if (calculator_print_active) SEND_STRING(".");

        } else {
            calculator_last_keycode_valid = false;
        }
        return false;   // Keycode handled, don't process further
    }

    // Accept negative numbers (- in first position)
    // A minus after the first position is an operator
    if (keycode == KC_PMNS && calculator_buf_index == 0 && calculator_operator != '=') {
        // Add the minus sign to the input buffer
        calculator_input_buffer[calculator_buf_index++] = '-';
        calculator_input_buffer[calculator_buf_index] = '\0';
        if (calculator_print_active) SEND_STRING("-");
        return false;   // Keycode handled, don't process further
    }

    // Accept digits from 0 - F
    if ((keycode >= KC_P1 && keycode <= KC_P0) ||
        (keycode >= C_HEXA && keycode <= C_HEXF)) {

        // Handle modifiers: shift, ctrl, alt
        if (get_mods() & (MOD_MASK_SHIFT | MOD_MASK_CTRL | MOD_MASK_ALT)) {
            if (calculator_base == 16) {
                if (keycode >= KC_P1 && keycode <= KC_P6) {
                    // Shift-1 = A, Shift-2 = B, etc.
                    keycode = keycode - KC_P1 + C_HEXA;
                }
            }
            clear_mods();
        }
        // Check if the keycode is valid for the current base
        if (key_numeric_value[keycode] < calculator_base) {
            // Add the digit to the input buffer
            char digit = key_char_value[keycode];
            calculator_input_buffer[calculator_buf_index++] = digit;
            calculator_input_buffer[calculator_buf_index] = '\0';
            calculator_operand = string_to_double(calculator_input_buffer, calculator_base);
            if (calculator_print_active) SEND_STRING(char_to_str(digit));
        } else {
            calculator_last_keycode_valid = false;
        }
        set_mods(mod_state);
        return false;   // Keycode handled, don't process further
    }

    // Pi
    if (keycode == C_PI) {
        double_to_string(PI, calculator_base, calculator_input_buffer, sizeof(calculator_input_buffer), 9);
        calculator_operand = PI;
        if (calculator_print_active) SEND_STRING(calculator_input_buffer);
        return false;   // Keycode handled, don't process further
    }
    return true; // Keycode not handled, continue processing
}

void handle_base_conversion(uint16_t keycode) {
    if (keycode == C_BIN || keycode == C_OCT || keycode == C_DEC || keycode == C_HEX) {
        calculator_base = base_key_value[keycode]; // set the new base
        // format the input buffer to the new base
        double_to_string(calculator_operand, calculator_base, calculator_input_buffer, sizeof(calculator_input_buffer), 9);
    }
}

void handle_operators(uint16_t keycode) {
    // We start out with a value of 0 and an operator of +
    // you type 2 * 3 [Enter]
    // when you type the 2, that value becomes the operand
    // when you type the * we perform the earlier + operation with the 2
    // and the value is now 2 instead of 0, and the operator is now *
    // when you type the 3, that value becomes the operand
    // when you type [Enter] we perform the * operation with a result of 6

    // So, when we receive a new operator, we perform the operation with the
    // current value and the operand, and set the new operator

    if (keycode == KC_PPLS ||
        keycode == KC_PMNS ||
        keycode == KC_PAST ||
        keycode == KC_PSLS) {

        // Perform the operation with the current value and the operand
        calculator_current_value = perform_operation(calculator_current_value, calculator_operator, calculator_operand);

        // Set the new operator
        if (get_mods() & (MOD_MASK_SHIFT | MOD_MASK_CTRL | MOD_MASK_ALT)) {
            switch (keycode) {
                case KC_PMNS:   // Shift-minus for roots
                    calculator_operator = 'r';
                    break;
                case KC_PAST:   // Shift-* for exponents
                    calculator_operator = '^';
                    break;
                case KC_PSLS:   // Shift-/ for modulus
                    calculator_operator = '%';
                    break;
            }
            clear_mods();
        } else {
            calculator_operator = operator_key_value[keycode];
        }

        // Print the operator
        if (calculator_print_active) {
            char formatted_str[BUFFER_SIZE];
            switch (calculator_operator) {
                case 'r':
                    snprintf(formatted_str, sizeof(formatted_str), " rt ");
                    break;
                case 'e':
                    snprintf(formatted_str, sizeof(formatted_str), " ^ ");
                    break;
                default: 
                    snprintf(formatted_str, sizeof(formatted_str), " %c ", calculator_operator);
            }
            SEND_STRING(formatted_str);
        }

        // Reset the operand
        calculator_buf_index = 0;
        calculator_input_buffer[0] = '\0';
        calculator_operand = 0.0;

        char result[BUFFER_SIZE] = {0};
        double_to_string(calculator_current_value, calculator_base, result, sizeof(result), 9);
        if (message[0] == '\0') {
            flash(result, true);
        }
    }
}

double string_to_double(const char *buffer, uint16_t base) {
    if (base != 2 && base != 8 && base != 10 && base != 16) {
        return 0.0;  // Unsupported base
    }

    double result = 0.0;
    double fraction_part = 0.0;
    double fraction_divisor = base;
    int sign = 1;
    int i = 0;

    // Handle negative sign
    if (buffer[i] == '-') {
        sign = -1;
        i++;  // Skip '-'
    }

    // Integer part processing
    while (buffer[i] != '\0' && buffer[i] != '.') {
        unsigned char c = buffer[i];
        int digit = digit_numeric_value[c];

        if (digit >= base) {
            return 0.0;  // Invalid digit for the base
        }

        result = result * base + digit;
        i++;
    }

    // Fractional part processing
    if (buffer[i] == '.') {
        i++;  // Skip '.'

        while (buffer[i] != '\0') {
            unsigned char c = buffer[i];
            int digit = digit_numeric_value[c];

            if (digit >= base) {
                return 0.0;  // Invalid digit for the base
            }

            fraction_part += digit / fraction_divisor;
            fraction_divisor *= base;
            i++;
        }
    }

    return sign * (result + fraction_part);
}

void double_to_string(double value, uint16_t base, char *buffer, size_t buffer_size, int precision) {
    if (base != 2 && base != 8 && base != 10 && base != 16) {
        snprintf(buffer, buffer_size, "Unsupported base");
        return;
    }

    int index = 0;

    // Handle sign
    if (value < 0) {
        buffer[index++] = '-';
        value = -value;
    }

    // Extract integer part
    uint64_t integer_part = (uint64_t)value;
    double fractional_part = value - integer_part;

    // Convert integer part to string
    char int_buffer[buffer_size];
    int int_index = 0;
    if (integer_part == 0) {
        int_buffer[int_index++] = '0';
    } else {
        while (integer_part > 0) {
            int digit = integer_part % base;
            int_buffer[int_index++] = (digit < 10) ? (digit + '0') : (digit - 10 + 'A');
            integer_part /= base;
        }
    }

    // Reverse integer part since it was built in reverse order
    for (int i = 0; i < int_index; i++) {
        buffer[index++] = int_buffer[int_index - 1 - i];
    }

    // Handle fractional part
    if (fractional_part > 0 && precision > 0) {
        buffer[index++] = '.';  // Add decimal point
        for (int i = 0; i < precision; i++) {
            fractional_part *= base;
            int digit = (int)fractional_part;
            buffer[index++] = (digit < 10) ? (digit + '0') : (digit - 10 + 'A');
            fractional_part -= digit;
            if (fractional_part == 0) {
                break;  // Stop if there's no remaining fraction
            }
        }
        
        // Remove trailing zeros
        while (index > 0 && buffer[index - 1] == '0') {
            index--;
        }
        
        // Remove decimal point if it's the last character
        if (index > 0 && buffer[index - 1] == '.') {
            index--;
        }        
    }

    buffer[index] = '\0';  // Null-terminate the string
}

void handle_enter(uint16_t keycode) {
    char result[BUFFER_SIZE] = {0};
    if (keycode == KC_PENT) {
        // Perform the operation with the current value and the operand
        calculator_current_value = perform_operation(calculator_current_value, calculator_operator, calculator_operand);

        // Set the new operator
        calculator_operator = '=';

        // Reset the input buffer
        calculator_buf_index = 0;
        calculator_input_buffer[0] = '\0';

        double_to_string(calculator_current_value, calculator_base, result, sizeof(result), 9);
        if (calculator_print_active) {
            SEND_STRING(" = ");
            SEND_STRING(result);
            tap_code(KC_ENTER);
        }        
        if (message[0] == '\0') {
            flash(result, true);
        }
    }
}

double perform_operation(double result, char operator, double operand) {
    switch (operator) {
        case '=':
            if (calculator_input_buffer[0] != '\0') {
                result = operand;
            }
            break;
        case '+':
            result += operand;
            break;
        case '-':
            result -= operand;
            break;
        case '*':
            result *= operand;
            break;
        case '/':
            if (operand == 0) {
                clear_calculator();
                if (calculator_print_active) SEND_STRING(" ERROR: Undefined");
                flash("ERROR", false);
                return result;
            } else {
                result /= operand;
            }
            break;
        case '%':
            if (operand == 0) {
                clear_calculator();
                if (calculator_print_active) SEND_STRING(" ERROR: Undefined");
                flash("ERROR", false);
                return result;
            } else {
                result = (double)((int)result % (int)operand);
            }
            break;
        case '^':
            if (calculator_input_buffer[0] == '\0') {
                // User has not entered an operand, default to 2
                operand = 2.0;
                if (calculator_print_active) {
                    // but the default never got printed. Do that now.
                    SEND_STRING("2");
                }
            }
            result = pow(calculator_current_value, operand);
            break;
        case 'r':
            if (calculator_input_buffer[0] == '\0') {
                // User has not entered an operand, default to 2
                operand = 2.0;
                if (calculator_print_active) {
                    // but the default never got printed. Do that now.
                    SEND_STRING("2");
                }
            }
            if (operand == 0) {
                clear_calculator();
                if (calculator_print_active) SEND_STRING(" ERROR: Undefined");
                flash("ERROR", false);
                return result;
            } else {
                result = pow(calculator_current_value, 1.0 / operand);
            }
            break;
    }
    return result;
}

uint32_t rgb_to_int(uint8_t r, uint8_t g, uint8_t b) {
    return (uint32_t)((r << 16) | (g << 8) | b);
}

void set_key_rgb(uint16_t keycode, uint32_t rgb) {
    uint8_t r = (rgb >> 16) & 0xFF;
    uint8_t g = (rgb >> 8) & 0xFF;
    uint8_t b = rgb & 0xFF;
    set_keycode_color(keycode, r, g, b);
}

void calculator_rgb_display(void) {

    // Default colors (direct assignment)
    uint32_t calc_key_color = rgb_to_int(RGB_BLUE);
    uint32_t calc_key_valid_color = rgb_to_int(RGB_GREEN);
    uint32_t calc_key_error_color = rgb_to_int(RGB_RED);
    uint32_t calc_hex_key_color = rgb_to_int(RGB_PINK);

    // If clear status is active, override color
    if (clear_calc_status_active && timer_elapsed(clear_calc_status_timer) < 500) {
        calc_key_color = rgb_to_int(RGB_RED);
    } else {
        clear_calc_status_active = false;
    }

    // Function as a calculator when Num Lock is disabled
    if (!host_keyboard_led_state().num_lock) {
        set_key_rgb(KC_NUM, calc_key_color);
        set_key_rgb(KC_PPLS, calc_key_color);
        set_key_rgb(KC_PMNS, calc_key_color);
        set_key_rgb(KC_PAST, calc_key_color);
        set_key_rgb(KC_PSLS, calc_key_color);
        set_key_rgb(KC_PENT, calc_key_color);

        if (calculator_print_active) {
            set_keycode_color(KC_PSCR, RGB_RED);
        } else {
            set_key_rgb(KC_PSCR, calc_key_color);
        }

        // Valid digit keys for all bases
        set_key_rgb(KC_KP_0, calc_key_color);
        set_key_rgb(KC_KP_1, calc_key_color);

        // Octal, Decimal, Hexadecimal
        if (calculator_base == 8 || calculator_base == 10 || calculator_base == 16) {
            set_key_rgb(KC_KP_2, calc_key_color);
            set_key_rgb(KC_KP_3, calc_key_color);
            set_key_rgb(KC_KP_4, calc_key_color);
            set_key_rgb(KC_KP_5, calc_key_color);
            set_key_rgb(KC_KP_6, calc_key_color);
            set_key_rgb(KC_KP_7, calc_key_color);
        }

        // Decimal, Hexadecimal
        if (calculator_base == 10 || calculator_base == 16) {
            set_key_rgb(KC_KP_8, calc_key_color);
            set_key_rgb(KC_KP_9, calc_key_color);
        }

        // Hexadecimal - Use FN + 1 - 6 for A - F
        if (calculator_base == 16) {
            set_key_rgb(KC_KP_1, calc_hex_key_color);
            set_key_rgb(KC_KP_2, calc_hex_key_color);
            set_key_rgb(KC_KP_3, calc_hex_key_color);
            set_key_rgb(KC_KP_4, calc_hex_key_color);
            set_key_rgb(KC_KP_5, calc_hex_key_color);
            set_key_rgb(KC_KP_6, calc_hex_key_color);
        }

        // Decimal
        if (calculator_base == 10) {
            set_key_rgb(KC_PDOT, calc_key_color);
        }

        // If last key status is active, override color
        if (last_key_status_active && timer_elapsed(last_key_status_timer) < 500) {
            if (calculator_last_keycode != KC_NO) {
                if (calculator_last_keycode_valid) {
                    set_key_rgb(calculator_last_keycode, calc_key_valid_color);
                } else {
                    set_key_rgb(calculator_last_keycode, calc_key_error_color);
                }
            }            
        } else {
            last_key_status_active = false;
        }
    }
}
