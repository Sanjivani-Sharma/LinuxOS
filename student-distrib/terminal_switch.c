#include "terminal_switch.h"
#include "lib.h"
#include "paging.h"
#include "Sys_Calls.h"
#include "interrupt.h"
#include "i8259.h"
#include "scheduler.h"

/*
    swap_terminal_to:
        DESCRIPTION: swaps terminals to the desired terminal 
        Input: destination terminal
        output: none
        Side Effects: swaps video memory and terminal data
*/
void swap_terminal_to(int terminal_dest)
{
    // swaps memory of the two video pages into the physical video memory
    memcpy((void*) ((VIDEO_OFFSET + terminal_num * 2) * VMEM_PAGE_SIZE), (void*) (VIDEO_OFFSET  * VMEM_PAGE_SIZE), VMEM_PAGE_SIZE);

    // saves current cursor
    int saved_x = terminal_desc_array[terminal_num - 1].cursor_x;
    int saved_y = terminal_desc_array[terminal_num - 1].cursor_y;

    // clear screen
    clear();
    
    // restore saved cursor
    terminal_desc_array[terminal_num - 1].cursor_x = saved_x;
    terminal_desc_array[terminal_num - 1].cursor_y = saved_y;

    // set new video page
    memcpy((void*) (VIDEO_OFFSET  * VMEM_PAGE_SIZE), (void*) ((VIDEO_OFFSET + terminal_dest * 2) * VMEM_PAGE_SIZE), VMEM_PAGE_SIZE);

    // update terminal_num
    terminal_num = terminal_dest;
    
    // intialize terminal data
    switch(terminal_dest)
    {
        
        case 1:
            // initializing terminal 1 data
            if(terminal_one_init == 0)
            {
                terminal_one_init = 1;
                init_terminal_num(terminal_num - 1);
                
                return;
            }
            // restoring terminal 1
            else
            {
                restore_terminal(terminal_num - 1);
                return;
            }
            break;
        
        case 2:
            // initializing terminal 2 data
            if(terminal_two_init == 0)
            {
                terminal_two_init = 1;
                init_terminal_num(terminal_num - 1);
                return;
            }
            // restoring terminal 2
            else
            {
                restore_terminal(terminal_num - 1);
                return;
            }
            break;
        case 3:
            // intializng terminal 3
            if(terminal_three_init == 0)
            {
                terminal_three_init = 1;
                init_terminal_num(terminal_num - 1);
                return;
            }
            // restoring terminal 3
            else
            {
                restore_terminal(terminal_num - 1);
                //sti();
                return;
            }
            break;
    }
    
}

/*
    init_terminal_num
        DESCRIPTION: intialize terminal_ind data
        Input: terminal index
        Output: none
        Side Effects: none
*/
void init_terminal_num(int terminal_ind)
{
    terminal_desc_array[terminal_ind].process_count = 0;
    terminal_desc_array[terminal_ind].base_process_num = 0;
    terminal_desc_array[terminal_ind].cur_process_num = 0;
}

/*
    restore_terminal_num
        DESCRIPTION: restore terminal_ind data
        Input: terminal index
        Output: none
        Side Effects: none
*/
void restore_terminal(int terminal_ind)
{
    update_cursor(terminal_desc_array[terminal_ind].cursor_x, terminal_desc_array[terminal_ind].cursor_y);
    process_num = terminal_desc_array[terminal_ind].cur_process_num;
}
