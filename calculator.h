#pragma once
#include "custom_keycodes.h"
#include "rgb.h"

#define BUFFER_SIZE 32

extern char message[256];

// Function declarations
bool handle_calculator_input(uint16_t keycode, keyrecord_t *record);
bool is_calculator_keycode(uint16_t keycode);
void clear_calculator(void);
bool handle_digits(uint16_t keycode);
void handle_base_conversion(uint16_t keycode);
void handle_operators(uint16_t keycode);
void print_debug(uint16_t keycode);
double string_to_double(const char *buffer, uint16_t base);
void double_to_string(double value, uint16_t base, char *buffer, size_t buffer_size, int precision);
void handle_enter(uint16_t keycode);
double perform_operation(double result, char operator, double operand);
void calculator_rgb_display(void);
void print_result(void);

static const uint8_t key_numeric_value[] = {
    [KC_P0] = 0,
    [KC_P1] = 1,
    [KC_P2] = 2,
    [KC_P3] = 3,
    [KC_P4] = 4,
    [KC_P5] = 5,
    [KC_P6] = 6,
    [KC_P7] = 7,
    [KC_P8] = 8,
    [KC_P9] = 9,
    [C_HEXA] = 10,
    [C_HEXB] = 11,
    [C_HEXC] = 12,
    [C_HEXD] = 13,
    [C_HEXE] = 14,
    [C_HEXF] = 15
};

static const char key_char_value[] = {
    [KC_P0] = '0',
    [KC_P1] = '1',
    [KC_P2] = '2',
    [KC_P3] = '3',
    [KC_P4] = '4',
    [KC_P5] = '5',
    [KC_P6] = '6',
    [KC_P7] = '7',
    [KC_P8] = '8',
    [KC_P9] = '9',
    [C_HEXA] = 'A',
    [C_HEXB] = 'B',
    [C_HEXC] = 'C',
    [C_HEXD] = 'D',
    [C_HEXE] = 'E',
    [C_HEXF] = 'F'
};

static const uint8_t base_key_value[] = {
    [C_BIN] = 2,
    [C_OCT] = 8,
    [C_DEC] = 10,
    [C_HEX] = 16
};

static const char operator_key_value[] = {
    [KC_PPLS] = '+',
    [KC_PMNS] = '-',
    [KC_PAST] = '*',
    [KC_PSLS] = '/'
};

static const uint8_t digit_numeric_value[] = {
    ['0'] = 0,
    ['1'] = 1,
    ['2'] = 2,
    ['3'] = 3,
    ['4'] = 4,
    ['5'] = 5,
    ['6'] = 6,
    ['7'] = 7,
    ['8'] = 8,
    ['9'] = 9,
    ['A'] = 10,
    ['B'] = 11,
    ['C'] = 12,
    ['D'] = 13,
    ['E'] = 14,
    ['F'] = 15
};