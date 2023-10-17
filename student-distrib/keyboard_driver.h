#ifndef _KEYBOARD_DRIVER_H
#define _KEYBOARD_DRIVER_H

#include "types.h"
#include "terminal_switch.h"

#define keyboard_buffer_length      128



char keyboard_buffer[keyboard_buffer_length];               // holds keyboard input
char buf[MAX_TERMINAL_COUNT][keyboard_buffer_length];       // terminal buffer
int buf_size;                                               // terminal buffer size
int read_flags[MAX_TERMINAL_COUNT];

// terminal functions
int terminal_open();
int terminal_close();
int terminal_read(char* buffer, int buffer_size);
int terminal_read_kb(const char* buffer, int buffer_size);
int terminal_write();
int terminal_write_buffer(const uint8_t* buffer, int32_t nbytes);

#endif
