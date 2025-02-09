# System76 Launch Heavy 3 Custom Keymap

This is my personal keymap for the System76 Launch Heavy 3 keyboard.

## Layers

### Layer 0: Base
![Layer 0](./layout.png)
Shinethrough keycaps from [FKcustom](https://fkcaps.com/custom/EQU8WA)
- Standard QWERTY layout
- Modifier keys in ergonomic positions
- Tap Esc key for Escape, hold for Fn
- Macro play keys
- Layer toggle keys
- Tap both shift keys for Caps-Word
- Shift-Bcksp for Delete
- Shift-Delete for Insert

### Layer 1: Function Keys
![Layer 1](./fn_layer.png)
- F12-F24 keys
- Layer selection on nav cluster
- Select RGB Effects on number row
- Adjust RGB lighting controls on num pad / media keys
- Macro record
- Static macro keys
- Calculator Binary/Octal/Decimal/Hexadecimal/Pi
- Fn + R to repeat character
  - Fn + R
  - Enter repeat count on numpad
  - Type character to repeat
- Fn + I to play random idle-timeout message
- Fn + Up to increase Dynamic Tap Timeout
- Fn + Down to decrease Dynamic Tap Timeout
- Fn + Right to print Dynamic Tap Timeout

## Macros
### Dynamic Macro Recording
- Fn-Macro to start recording macro
- Type keys to record
- Fn-Macro to stop recording
- Press macro key to play dynamic macro

### Static Macros
- Copy static_macros_example.h to static_macros.h when building firmware
- Edit static_macros.h to define macro values
- Fn-E to type email address
- Fn-A to type address
- Fn-F to type first name
- Fn-L to type last name
- Fn-N to type full name

## Calculator Mode
![Calculator](./calculator.png)
- Num-Lock to enable/disable calculator mode
- Print Screen to toggle typing/stealth input
- Current result will flash on number row
- Shift-Enter to type result to computer
- Shift-/ for modulus
- Shift-* for exponents (defaults to ^2)
- Shift-- for root (defaults to square root)
- Shift-0 to clear entry (or Fn + C)
- Shift-0 again to clear all
- Fn-D for Decimal
- Fn-O for Octal
- Fn-H for Hexadecimal
- Fn-B for Binary

## Building

To build this keymap:

```bash
cp static_macros_example.h static_macros.h
nano static_macros.h
qmk compile -kb system76/launch_heavy_3 -km mkrzywonski
