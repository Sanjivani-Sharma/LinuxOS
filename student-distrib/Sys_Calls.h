#ifndef _SYS_CALLS_H
#define _SYS_CALLS_H

#include "types.h"
#include "keyboard_driver.h"
#include "filesystem.h"

#define FISH_VIDMAP_VIRTUAL         33

#define FILE_DESC_ARRAY_SIZE        8
#define FILE_NON_TERM               2
#define EIGHT_MEGABYTE              0x00800000
#define HundredTwentyEight_MB       0x08000000
#define FOUR_MEGABYTE               EIGHT_MEGABYTE / 2
#define EIGHT_KILOBYTES             0x00002000
#define FOUR_KILOBYTES              4096
#define ONE_BYTE                    8

#define ESP                         0x0083FFFFC

#define RTC_FILE_TYPE               0
#define DIR_FILE_TYPE               1
#define FILE_FILE_TYPE              2

#define COMMAND                     0
#define ARGS                        1

#define EXE_ID_DATA_SIZE            40
#define SYS_CALL_PAGE_DIR_IND       32

#define PROGRAM_IMG_OFF             0x00048000
#define PROGRAM_IMG_START           HundredTwentyEight_MB + PROGRAM_IMG_OFF
#define PROGRAM_IMG_LENGTH          FOUR_MEGABYTE - PROGRAM_IMG_OFF

#define KERNEL_STACK_OFF            4

#define MAX_PROCESSES_NUM           6
#define MAX_PROCESS_PER_TERM        4

#define ERROR_STAT                  -1

#define VID_MEM_START               0x8400000

int halt_ret_val;

int32_t processes[30];
int process_num;

int vidmap_flag[MAX_TERMINAL_COUNT];

// file descriptor struct for files
typedef struct file_desc_t{
    uint32_t * file_op_table;
    uint32_t inode;
    uint32_t file_position;
    uint32_t flags;
}file_desc_t;

// PCB struct for processes
typedef struct pcb_t{
    file_desc_t file_desc_array[FILE_DESC_ARRAY_SIZE];
    int32_t file_type_array[FILE_DESC_ARRAY_SIZE];
    int32_t parent_process_num;
    uint32_t parent_esp;
    uint32_t parent_ebp;
    uint32_t halt_ret_addr;
    uint32_t esp0;
    uint16_t ss0;
    uint8_t args[keyboard_buffer_length];
    int32_t next_process_num;
    uint32_t esp_save;
    uint32_t ebp_save;
    int32_t RTC_save;
    int32_t RTC_counter_save;
    int32_t RTC_ticks;
    int32_t RTC_flag;
}pcb_t;

// system call functions
int32_t halt(uint8_t status);
int32_t execute(const uint8_t* command);
int32_t read(int32_t fd, void* buffer, int32_t nbytes);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t open (const uint8_t* filename);
int32_t close (int32_t fd);
int32_t getargs (uint8_t* buf, int32_t nbytes);
int32_t vidmap (uint8_t** screen_start);
int32_t set_handler (int32_t signum, void* handler_address);
int32_t sigreturn (void);

void dummy_test();


int parse_command(const uint8_t* command, int command_type, int start, uint8_t* parsed_data);

#endif
