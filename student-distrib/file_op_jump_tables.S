/* Jumptables for different types of file operations */

.global RTC_File_OP
.global DIR_File_OP
.global FIL_File_OP
.global TER_File_OP

# RTC file operations
RTC_File_OP:
    .long rtc_open, rtc_read, rtc_write, rtc_close

# Directory file operations
DIR_File_OP:
    .long directory_open, directory_read, directory_write, directory_close

# File file operations
FIL_File_OP:
    .long file_open, file_read, file_write, file_close

# Terminal file operations
TER_File_OP:
    .long terminal_open, terminal_read, terminal_write, terminal_close
