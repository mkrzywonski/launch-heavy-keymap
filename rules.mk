DYNAMIC_MACRO_ENABLE = yes
KEY_OVERRIDE_ENABLE = yes
CAPS_WORD_ENABLE = yes
SEND_STRING_ENABLE = yes
DYNAMIC_TAPPING_TERM_ENABLE = yes

SRC += process_record_handlers.c
SRC += calculator.c
SRC += rgb.c

CFLAGS += -flto=4 -fno-common
LDFLAGS += -Wl,--no-warn-rwx-segments
