/*
    Provides Jumptables to file operations for different file types
*/
#ifndef _FILE_OP_JUMP_TABLES_H
#define _FILE_OP_JUMP_TABLES_H

extern void RTC_File_OP();                      // Jumptable to RTC File Operations
extern void DIR_File_OP();                      // Jumptable to Directory File Operations
extern void FIL_File_OP();                      // Jumptable to File File Operations
extern void TER_File_OP();                      // Jumptable to Terminal Operations

#endif
