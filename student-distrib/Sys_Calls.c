#include "Sys_Calls.h"
#include "filesystem.h"
#include "types.h"
#include "keyboard_driver.h"
#include "RTC.h"
#include "paging.h"
#include "x86_desc.h"
#include "enable_paging.h"
#include "Sys_Call_Linkage.h"
#include "lib.h"
#include "terminal_switch.h"
#include "scheduler.h"

// file operation jump tables
static uint32_t TER_File_OP[4] = {(int32_t) terminal_open, (int32_t) terminal_read, (int32_t) terminal_write_buffer, (int32_t) terminal_close};
static uint32_t RTC_File_OP[4] = {(int32_t) rtc_open, (int32_t) rtc_read, (int32_t) rtc_write, (int32_t) rtc_close};
static uint32_t FIL_File_OP[4] = {(int32_t) file_open, (int32_t) file_read, (int32_t) file_write, (int32_t) file_close};
static uint32_t DIR_File_OP[4] = {(int32_t) directory_open, (int32_t) directory_read, (int32_t) directory_write, (int32_t) directory_close};

typedef int32_t exe_func();

int process_count;                                          // current process count
int process_num = 0;                                        // current process number 
int halt_ret_val = 0;                                       // global halt return value
int halt_process = 0;                                       // halt process flag
int8_t EXE_ID[4] = {0x7f, 0x45, 0x4c, 0x46};                // EXE identifier

/* 
 * halt
 *   DESCRIPTION: Halts the current process
 *   INPUTS: status
 *   OUTPUTS: -1 upon complretion
 *   SIDE EFFECTS: restores parent data/paging, jumps to  
 */
int32_t halt(uint8_t status)
{
    // set vidmap flag to 0
    vidmap_flag[cur_terminal_num - 1] = 0;

    // set status to return
    halt_ret_val = status;

    // setup pcbs to work with
    pcb_t* child_pcb = (pcb_t*) (EIGHT_MEGABYTE - EIGHT_KILOBYTES*(terminal_desc_array[cur_terminal_num - 1].cur_process_num + 1));
    int32_t parent_process_number = child_pcb->parent_process_num;
    pcb_t* parent_pcb = (pcb_t*) (EIGHT_MEGABYTE - EIGHT_KILOBYTES*(parent_process_number + 1));

    // calculate parent paging address
    int32_t parent_page_addr = (EIGHT_MEGABYTE + FOUR_MEGABYTE * parent_process_number);
    
    // closing files
    int fd;
    for(fd = 0; fd < FILE_DESC_ARRAY_SIZE; fd++) // Loop through the file des array in child pcb
    {
        close(fd);
    }
    
    // saving tss values
    tss.esp0 = parent_pcb->esp0;
	tss.ss0 = parent_pcb->ss0;

    // switching to parent and setting up paging
    processes[terminal_desc_array[cur_terminal_num - 1].cur_process_num] = 0;
    terminal_desc_array[cur_terminal_num - 1].process_count--;
    process_count--;
    
    // checking if process is shell
    if(terminal_desc_array[cur_terminal_num - 1].cur_process_num == terminal_desc_array[cur_terminal_num - 1].base_process_num)
    {
        execute((uint8_t*) "shell");
        return -1;                                                          // should not reach the return statement becasue of context switch
    }

    page_directory[SYS_CALL_PAGE_DIR_IND] = ((uint32_t) parent_page_addr) | FOURMB_USER_FLAGS;
    flush_TLB();

    // resetting parent pcb RTC values
    parent_pcb->RTC_save = 2;
    parent_pcb->RTC_counter_save = 0;
    parent_pcb->RTC_ticks = 512 / 2;
    
    // setting process number to parent process number
    terminal_desc_array[cur_terminal_num - 1].cur_process_num = parent_process_number;
    if(terminal_num == cur_process_num)
        process_num = parent_process_number;

    reset_RTC_counters_flag = 1;
    // jumping to return
    asm volatile (
        "movl %0, %%esp         \n          \
         movl %1, %%ebp         \n          \
         jmp *%2"

        :   
        :   "r"(child_pcb->parent_esp), "r"(child_pcb->parent_ebp), "r"(child_pcb->halt_ret_addr)
        :	"esp", "ebp", "memory"
    );


    return -1;
}

/* 
 * execute
 *   DESCRIPTION: Executes a process/Task
 *   INPUTS: status
 *   OUTPUTS: -1 upon complretion or failure
 *   SIDE EFFECTS: creates new page directory and  
 */
int32_t execute(const uint8_t* command)
{
    cli();
    
    // setting parent process
    int parent_process = terminal_desc_array[cur_terminal_num - 1].cur_process_num;
    
    // Initialize variables 
    uint8_t exe_command[keyboard_buffer_length];
    uint8_t args[keyboard_buffer_length];

    // Execture and Print commands 
    // Command Length
    int command_length = parse_command(command, COMMAND, 0, exe_command);
    if(command_length == -1 || command_length >= MAXIMUM_LENGTH_FILENAME)
        return -1;

    // Arg Length
    int args_length = parse_command(command, ARGS, command_length, args);
    if(args_length == -1 || args_length + command_length > keyboard_buffer_length)
        return -1;

    // Command Dentry
    dentry_t command_dentry;
    int32_t dentry_found = read_dentry_by_name(exe_command, &command_dentry);
    if(dentry_found == -1)
        return -1;
    
    terminal_desc_array[cur_terminal_num - 1].process_count++;
    process_count++;
    // Page for the Process
    int process;
    for(process = 0; process < MAX_PROCESSES_NUM && terminal_desc_array[cur_terminal_num - 1].process_count <= MAX_PROCESS_PER_TERM && process_count <= MAX_PROCESSES_NUM; process++)
    {
        if(processes[process] == 0)
        {
            if(terminal_num == cur_terminal_num)
                process_num = process;
            terminal_desc_array[cur_terminal_num - 1].cur_process_num = process;
            processes[process] = 1;
            break;
        }
    }


    

    // Process Count check

    if(terminal_desc_array[cur_terminal_num - 1].process_count == 1)
    {
        terminal_desc_array[cur_terminal_num - 1].base_process_num = terminal_desc_array[cur_terminal_num - 1].cur_process_num;
    }

    if(process_count + num_shells_init > MAX_PROCESSES_NUM)
    {
        terminal_desc_array[cur_terminal_num - 1].process_count--;
        process_count--;
        terminal_desc_array[cur_terminal_num - 1].cur_process_num = parent_process;
        if(terminal_num == cur_terminal_num)
            process_num = parent_process;
        return -1;
    }

    // Process number check
    if(process_num >= MAX_PROCESSES_NUM)
    {
        terminal_desc_array[cur_terminal_num - 1].process_count--;
        process_count--;
        terminal_desc_array[cur_terminal_num - 1].cur_process_num = parent_process;
        if(terminal_num == cur_terminal_num)
            process_num = parent_process;
        return -1;
    }
    
    // setting page at adress 0x8000000
    int32_t sys_call_addr = (EIGHT_MEGABYTE + FOUR_MEGABYTE * terminal_desc_array[cur_terminal_num - 1].cur_process_num);
    page_directory[SYS_CALL_PAGE_DIR_IND] = sys_call_addr | FOURMB_USER_FLAGS;
    flush_TLB();

    uint8_t EXE_DATA[EXE_ID_DATA_SIZE];
    inode_t* program_inode = (inode_t*)(inode_ptr + command_dentry.inode_index * BLOCK_SIZE);
    int32_t read_res = read_data(command_dentry.inode_index, 0, EXE_DATA, EXE_ID_DATA_SIZE);
    if(read_res == -1 || EXE_DATA[0] != EXE_ID[0] || EXE_DATA[1] != EXE_ID[1] || EXE_DATA[2] != EXE_ID[2] || EXE_DATA[3] != EXE_ID[3])
        return -1;
     
    // bytes 24-27 represent a 32 bit address that points to start of instructions
    int32_t code_ptr = EXE_DATA[27];
    code_ptr = (code_ptr << ONE_BYTE) | EXE_DATA[26];
    code_ptr = (code_ptr << ONE_BYTE) | EXE_DATA[25];
    code_ptr = (code_ptr << ONE_BYTE) | EXE_DATA[24];

    pcb_t* program_pcb = (pcb_t *) (EIGHT_MEGABYTE - EIGHT_KILOBYTES * (terminal_desc_array[cur_terminal_num - 1].cur_process_num + 1));
    
    uint32_t file_length = program_inode->length;
    
    // reading file data
    read_res = read_data(command_dentry.inode_index, 0, (uint8_t*) PROGRAM_IMG_START, file_length);

    // shut process down if read was faulty
    if(read_res == -1)
    {
        processes[terminal_desc_array[cur_terminal_num - 1].cur_process_num] = 0;
        terminal_desc_array[cur_terminal_num - 1].process_count--;
        process_count--;
        terminal_desc_array[cur_terminal_num - 1].cur_process_num = parent_process;
        if(terminal_num == cur_terminal_num)
            process_num = parent_process;
        return -1;
    }
   // Update pcb struct 
    int free_ind;
    for(free_ind = 0; free_ind < FILE_DESC_ARRAY_SIZE; free_ind++)
    {
        program_pcb->file_desc_array[free_ind].flags = 0;
    }

    // setting stdin
    program_pcb->file_desc_array[0].file_op_table = (uint32_t*) TER_File_OP;
    program_pcb->file_desc_array[0].inode = 0;
    program_pcb->file_desc_array[0].file_position = 0;
    program_pcb->file_desc_array[0].flags = 1;
    program_pcb->file_type_array[0] = -1;

    // setting stdout
    program_pcb->file_desc_array[1].file_op_table = (uint32_t*) TER_File_OP;
    program_pcb->file_desc_array[1].inode = 0;
    program_pcb->file_desc_array[1].file_position = 0;
    program_pcb->file_desc_array[1].flags = 1;
    program_pcb->file_type_array[1] = -1;
    
    // setting process numbers of pcb
    program_pcb->parent_process_num = parent_process;
    strncpy((int8_t*) program_pcb->args, (int8_t*) args, (uint32_t) keyboard_buffer_length);
    program_pcb->next_process_num = -1;

    // setting pcb rtc settings
    program_pcb->RTC_save = 2;
    program_pcb->RTC_counter_save = 0;
    program_pcb->RTC_ticks = 512 / 2;

    // set next process
    if(process_num != 0)
    {
        pcb_t* parent_pcb = (pcb_t *) (EIGHT_MEGABYTE - EIGHT_KILOBYTES * (parent_process + 1));
        parent_pcb->next_process_num = terminal_desc_array[cur_terminal_num - 1].cur_process_num;
    }

    int n;
    // clearing other flags
    for(n = FILE_NON_TERM; n < FILE_DESC_ARRAY_SIZE; n++)
    {
        program_pcb->file_desc_array[n].flags = 0;
    }
    
    // updating tss values
    tss.ss0 = KERNEL_DS;
    tss.esp0 = EIGHT_MEGABYTE - EIGHT_KILOBYTES * terminal_desc_array[cur_terminal_num - 1].cur_process_num - 4;

    // saving tss values
    program_pcb->esp0 = tss.esp0;
    program_pcb->ss0 = tss.ss0;

    uint32_t esp = HundredTwentyEight_MB + FOUR_MEGABYTE - 4; 

    reset_RTC_counters_flag = 1;

    // setting halt jump point
    asm volatile (
        "movl %%esp, %0  \n       \
        movl %%ebp, %1  \n       \
        leal exe_halt_addr, %2"
        :"=g"(program_pcb->parent_esp), "=g"(program_pcb->parent_ebp), "=g"(program_pcb->halt_ret_addr)
        :
        :"memory"
    );
    
    // context switch
    asm volatile (
        "movl %0, %%eax \n      \
        movw %%ax, %%ds \n      \
        pushl %0       \n       \
        pushl %1        \n       \
        pushfl          \n       \
        popl %%eax      \n       \
        orl $0x200, %%eax \n     \
        pushl %%eax     \n       \
        pushl %2        \n       \
        pushl %3"

        :				
		:"g"(USER_DS), "g"(esp), "g"(USER_CS), "g"(code_ptr)				
		:"memory", "eax");
    
    asm volatile("iret");
    asm volatile("exe_halt_addr:");
    return halt_ret_val;
}

/* 
 * read
 *   DESCRIPTION: reads into buffer
 *   INPUTS: fd, buf, nbytes
 *   OUTPUTS: -1 upon failure read_res upon success
 *   SIDE EFFECTS: none  
 */
int32_t read(int32_t fd, void* buffer, int32_t nbytes)
{
    // Initialize the Variables 
    uint32_t pcb_addr = EIGHT_MEGABYTE - EIGHT_KILOBYTES*(terminal_desc_array[cur_terminal_num - 1].cur_process_num + 1);
    pcb_t* program_pcb = (pcb_t *) pcb_addr;

    // valid fd check
    if(fd < 0 || fd == 1 || fd > FILE_DESC_ARRAY_SIZE || buf == NULL || program_pcb->file_desc_array[fd].flags == 0)
        return -1;

    //update read_res
    int32_t read_res;
    if(program_pcb->file_type_array[fd] == -1)
    {   
        // terminal
        sti();
        read_res = ((exe_func*) program_pcb->file_desc_array[fd].file_op_table[1])(buffer, nbytes);
    }
    else if(program_pcb->file_type_array[fd] == 0)
    {
        // RTC
        read_res = ((exe_func*) program_pcb->file_desc_array[fd].file_op_table[1])();
    }
    else
    {
        // file or directory
        if(program_pcb->file_type_array[fd] == 1)
            dbytes_read = program_pcb->file_desc_array[fd].file_position;
        else if(program_pcb->file_type_array[fd] == 2)
            fbytes_read = program_pcb->file_desc_array[fd].file_position;
        read_res = ((exe_func*) program_pcb->file_desc_array[fd].file_op_table[1])(fd, buffer, nbytes);

        if(program_pcb->file_type_array[fd] == 1)
            program_pcb->file_desc_array[fd].file_position = dbytes_read;
        else if(program_pcb->file_type_array[fd] == 2)
            program_pcb->file_desc_array[fd].file_position = fbytes_read;
    }
    
    return read_res;
}

/* 
 * write
 *   DESCRIPTION: writes to a given file
 *   INPUTS: fd, buf, nbytes
 *   OUTPUTS: -1 upon failure, write_res upon success 
 *   SIDE EFFECTS: none 
 */
int32_t write (int32_t fd, const void* buf, int32_t nbytes)
{
    
    // initialize the local variables 
    uint32_t pcb_addr = EIGHT_MEGABYTE - EIGHT_KILOBYTES*(terminal_desc_array[cur_terminal_num - 1].cur_process_num + 1);
    pcb_t* program_pcb = (pcb_t *) pcb_addr;
    // valid fd check
    if(fd <= 0 || fd > FILE_DESC_ARRAY_SIZE || program_pcb->file_desc_array[fd].flags == 0)
    {
        return -1;
    }

    // update write_res
    int32_t write_res;
    if(program_pcb->file_type_array[fd] == -1)
    {
        write_res = ((exe_func*) program_pcb->file_desc_array[fd].file_op_table[2])(buf, nbytes);
    }
    else if(program_pcb->file_type_array[fd] == 0)
    {
        // RTC
        write_res = ((exe_func*) program_pcb->file_desc_array[fd].file_op_table[2])(*((int*) buf));
    }
    else
    {
        // file or directory
        write_res = ((exe_func*) program_pcb->file_desc_array[fd].file_op_table[2])(fd, buf, nbytes);
    }

    return write_res;
}

/* 
 * open
 *   DESCRIPTION: openss a givien file to be accessed
 *   INPUTS: fd, buf, nbytes
 *   OUTPUTS: -1 upon failure, write_res upon success 
 *   SIDE EFFECTS: creates new page directory and  
 */
int32_t open (const uint8_t* filename)
{   
    // Initialize the Variables 
    int free_ind;
    uint32_t pcb_addr = EIGHT_MEGABYTE - EIGHT_KILOBYTES*(terminal_desc_array[cur_terminal_num - 1].cur_process_num + 1);
    pcb_t* pcb = (pcb_t*) pcb_addr;


    dentry_t file_dentry;
    int32_t read_result = read_dentry_by_name(filename, &file_dentry);
    
    if(read_result == -1)
        return -1;

    // iterate through file description until one is found
    for(free_ind = 2; free_ind < FILE_DESC_ARRAY_SIZE; free_ind++)
    {
        if(pcb->file_desc_array[free_ind].flags == 0)
        {
            pcb->file_desc_array[free_ind].flags = 1;
            break;
        }
    }

    
    
    // check if free_ind is in range
    if(free_ind == FILE_DESC_ARRAY_SIZE)
        return -1;

    pcb->file_desc_array[free_ind].inode = file_dentry.inode_index;
    
    
    
    // execute open based on file type
    switch(file_dentry.file_type)
    {
        // if file is RTC
        case RTC_FILE_TYPE: 
            pcb->file_desc_array[free_ind].file_op_table = (uint32_t*) RTC_File_OP;
            pcb->file_desc_array[free_ind].inode = 0;     
            pcb->file_type_array[free_ind] = 0;  
            ((exe_func*) pcb->file_desc_array[free_ind].file_op_table[0])();
            break;
        // file is directory
        case DIR_FILE_TYPE: 
            pcb->file_desc_array[free_ind].file_op_table = (uint32_t*) DIR_File_OP;
            pcb->file_desc_array[free_ind].inode = 0;  
            pcb->file_type_array[free_ind] = 1;  
            ((exe_func*) pcb->file_desc_array[free_ind].file_op_table[0])(filename);
            break;
        // file is file
        case FILE_FILE_TYPE:
            pcb->file_desc_array[free_ind].file_op_table = (uint32_t*) FIL_File_OP;
            pcb->file_desc_array[free_ind].inode = file_dentry.inode_index;
            pcb->file_type_array[free_ind] = 2; 
            ((exe_func*) pcb->file_desc_array[free_ind].file_op_table[0])(filename);
            break;
        default:
            return -1;
    }
    pcb->file_desc_array[free_ind].file_position = 0;

    return free_ind;
}

/*
 * close
 *   DESCRIPTION: closes a givien file
 *   INPUTS: fd - file descriptor index
 *   OUTPUTS: -1 upon failure, 0 upon success 
 *   SIDE EFFECTS: closes file
*/
int32_t close (int32_t fd)
{
    
    uint32_t pcb_addr = EIGHT_MEGABYTE - EIGHT_KILOBYTES*(terminal_desc_array[cur_terminal_num - 1].cur_process_num + 1);
    pcb_t* pcb = (pcb_t*) pcb_addr;

    //check if fd is valid
    if (fd < FILE_NON_TERM || fd >= FILE_DESC_ARRAY_SIZE){
        return -1;
    }

    //check if fd is not in use
    if(pcb->file_desc_array[fd].flags == 0)
        return -1;

    //do not allow closing of defualt descriptors
    if(fd == 0 || fd ==1)
        return -1;
    

    //clear  file descript
    pcb->file_desc_array[fd].flags = 0;
    

    return 0;

}

/* 
 * getargs
 *   DESCRIPTION: openss a givien file to be accessed
 *   INPUTS: buf - pointer to where arguments should be stored, nbytes- number of bytes to be coppied
 *   OUTPUTS: -1 upon failure, 0 upon success 
 *   SIDE EFFECTS: Changes the contents of buf 
 */
int32_t getargs (uint8_t* buf, int32_t nbytes)
{
    // getting pcb
    uint32_t pcb_addr = EIGHT_MEGABYTE - EIGHT_KILOBYTES*(terminal_desc_array[cur_terminal_num - 1].cur_process_num + 1);
    pcb_t* pcb = (pcb_t*) pcb_addr;

    // checking if buf is valid
    if(buf == NULL || nbytes == 0)
        return -1;
    
    //copying into buf
    strncpy((int8_t*) buf, (int8_t*) pcb->args, (uint32_t) nbytes);

    // checking if there are args
    if(strlen((int8_t*) buf) == 0)
        return -1;

    return 0;
}

/* 
 * vidmap
 *   DESCRIPTION: maps process video memory to screen start
 *   INPUTS: screen_start - pointer to physical addr of screen memory
 *   OUTPUTS: -1 upon failure, 0 upon success 
 *   SIDE EFFECTS: Changes the contents of screen_start
 */
int32_t vidmap (uint8_t** screen_start)
{
    uint32_t ret;
    
    // checking screen start validity
    if (screen_start < (uint8_t**) (HundredTwentyEight_MB) || screen_start >= (uint8_t**) (VID_MEM_START)){ // Address Validity Check
        return -1; 
    }

    vidmap_flag[cur_terminal_num - 1] = 1;

    // setting up paging
    ret = VIDEO_OFFSET * VMEM_PAGE_SIZE; // Video Memory Constant
    *(vid_page_table[cur_terminal_num - 1]) =  ret | SUPERVISER_EN;
    
    page_directory[FISH_VIDMAP_VIRTUAL] = ((unsigned int) vid_page_table[cur_terminal_num - 1]) | SUPERVISER_EN;
    flush_TLB();

    vidmap_flag[cur_terminal_num - 1] = 1;

    // setting screen start
    *screen_start = (uint8_t*) VID_MEM_START; // setting screen start loc
    return 0;
}

int32_t set_handler (int32_t signum, void* handler_address)
{
    return -1;
}

int32_t sigreturn (void)
{
    return -1;
}

/* 
 * parse_command
 *   DESCRIPTION: helper function that parses through list
 *   INPUTS: command, command_type, start, parsed data
 *   OUTPUTS: char_count 
 *   SIDE EFFECTS: creates new page directory and  
 */
int parse_command(const uint8_t* command, int command_type, int start, uint8_t* parsed_data)
{
    // Initialize local variables
    int c = start;
    int c_read = 0;
    int char_count = 0;
    int arg_flag = 0;

    // loop thorugh command and parse
    while(command[c] != '\0' && command[c] != '\n' && (command[c] != ' ' || command_type == ARGS))
    {
        // check char validity
        if(char_count >= keyboard_buffer_length)
            return -1;
        char_count++;

        // check if reading args
        if(command_type == ARGS && ((arg_flag == 1 && command[c] != ' ') || arg_flag == 2))
        {
            arg_flag = 2;
            *(parsed_data + c_read) = command[c];
            c_read++;
        }
        else if(arg_flag == 0 && command[c] != ' ')
        {
            // reading command
            *(parsed_data + c_read) = command[c];
            c_read++;
        }
        else if(arg_flag == 0 && command[c] == ' ')
        {
            // start reading args
            arg_flag = 1;
        }
        c++;
    }
    *(parsed_data + c_read) = '\0';
    return char_count;
}

// dummy function for testing purposes
void dummy_test()
{
    printf("dummy\n");
    while(1);
}
