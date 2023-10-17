#include "scheduler.h"
#include "i8259.h"
#include "interrupt.h"
#include "Sys_Calls.h"
#include "paging.h"
#include "enable_paging.h"
#include "x86_desc.h"
#include "terminal_switch.h"
#include "lib.h"

int cur_process_num = -1;                           // current process running by scheduler
int cur_terminal_num = -1;                          // current terminal running by scheduler

int term1_saved = 0;                                // terminal 1 saved flag
int term2_saved = 0;                                // terminal 2 saved flag
int term3_saved = 0;                                // terminal 3 saved flag

int num_shells_init = 2;                            // number of shells left to initialize

/*
    switch_task:
        DESCRIPTION: handles process switching between terminal programs
        Input: none
        Output: none
        Side Effect: alters values of esp and ebp and reroutes the process
*/
// structure credited to https://wiki.osdev.org/User:Mariuszp/Scheduler_Tutorial
void switch_task()
{
    // setting terminal 1 saved flag
    term1_saved = 1;
    
    // checking if this is first time ever scheduling is running
    if(cur_terminal_num == -1)
        cur_terminal_num = terminal_num;

    // getting current terminal
    terminal_desc_t this_terminal = terminal_desc_array[cur_terminal_num - 1];
    cur_process_num = this_terminal.cur_process_num;

    // getting next terminal number and next terminal
    int next_terminal_num = get_next_terminal_num(cur_terminal_num);
    terminal_desc_t next_terminal = terminal_desc_array[next_terminal_num - 1];

    // getting id of next process
    int next_process_num = next_terminal.cur_process_num;
    pcb_t* process_pcb = (pcb_t *) (EIGHT_MEGABYTE - EIGHT_KILOBYTES * (cur_process_num + 1));

    /* copy the saved registers into the current_proc structure */
    asm volatile(
        "movl %%esp, %0             \n  \
         movl %%ebp, %1"    
        :"=r"(process_pcb->esp_save), "=r"(process_pcb->ebp_save)
        :
        :"memory"
    );

    /* now go onto the next task - if there isn't one, go back to the start
     of the queue. */
    
    // updating cur_terminal_num
    cur_terminal_num = next_terminal_num;
    current_terminal_num = cur_terminal_num;

    // checking if next process's ebp and esp was previously saved
    if((next_terminal_num == 1 && term1_saved == 1) || (next_terminal_num == 2 && term2_saved == 1) || (next_terminal_num == 3 && term3_saved == 1))
    {
        // rerouting video address appropriately depending on what terminal scheduler is running
        uint32_t vid_addr = (VIDEO_OFFSET + next_terminal_num * 2) * VMEM_PAGE_SIZE;

        // if terminal is displayed and is not a video
        if(terminal_num == cur_terminal_num && vidmap_flag[cur_terminal_num - 1] == 0)
        {
            // mapping virtual video mem to physical video mem
            page_table[VIDEO_OFFSET] = (VIDEO_OFFSET * VMEM_PAGE_SIZE) | SUPERVISER_EN;
        }
        // if terminal is displayed and is a video
        else if(terminal_num == cur_terminal_num && vidmap_flag[cur_terminal_num - 1] == 1)
        {
            // setting video page table
            *(vid_page_table[cur_terminal_num - 1]) =  (VIDEO_OFFSET * VMEM_PAGE_SIZE) | SUPERVISER_EN;
            
            // setting virtual video offset to hardware video offset
            page_table[VIDEO_OFFSET] = (VIDEO_OFFSET * VMEM_PAGE_SIZE) | SUPERVISER_EN;
            page_directory[FISH_VIDMAP_VIRTUAL] = ((unsigned int) vid_page_table[cur_terminal_num - 1]) | SUPERVISER_EN;
        }
        // if terminal is not displayed and is not video
        else if(vidmap_flag[cur_terminal_num - 1] == 0)
        {
            // map to background page
            page_table[VIDEO_OFFSET] = vid_addr | SUPERVISER_EN;
        }
        // if terminal is not displayed and is video
        else if(vidmap_flag[cur_terminal_num - 1] == 1)
        {
            // set video page table
            *(vid_page_table[cur_terminal_num - 1]) =  vid_addr | SUPERVISER_EN;
            
            // map virtual video offset to hardware video offset
            page_table[VIDEO_OFFSET] = vid_addr | SUPERVISER_EN;
            page_directory[FISH_VIDMAP_VIRTUAL] = ((unsigned int) vid_page_table[cur_terminal_num - 1]) | SUPERVISER_EN;
        }
        // flush tlb
        flush_TLB();

        

        int32_t sys_call_addr = (EIGHT_MEGABYTE + FOUR_MEGABYTE * next_process_num);
        // setting page at adress 0x8000000
        page_directory[SYS_CALL_PAGE_DIR_IND] = sys_call_addr | FOURMB_USER_FLAGS;
        flush_TLB();
    
        // getting pcb for next process
        process_pcb = (pcb_t *) (EIGHT_MEGABYTE - EIGHT_KILOBYTES * (next_process_num + 1));

        // updating tss
        tss.ss0 = process_pcb->ss0;
        tss.esp0 = process_pcb->esp0;

        // updating esp and ebp
        uint32_t esp = process_pcb->esp_save;
        uint32_t ebp = process_pcb->ebp_save;

        // sending eoi
        send_eoi(PIT_PIC_IRQ);

        /* now hack the registers! */
        asm volatile(
            "movl %0, %%esp             \n  \
            movl %1, %%ebp"
            :
            :"r"(esp), "r"(ebp)
            :"esp", "ebp"
        );
        
    }
    // if terminal 1 has not been saved execute terminal 1
    else if(next_terminal_num == 1)
    {
        term1_saved = 1;
        page_table[VIDEO_OFFSET] = (VIDEO_OFFSET * VMEM_PAGE_SIZE) | SUPERVISER_EN;
        flush_TLB();
        clear();
        num_shells_init = 2;
        send_eoi(PIT_PIC_IRQ);
        execute((uint8_t*) "shell");
    }
    // if terminal 2 has not been saved execute terminal 2
    else if(next_terminal_num == 2)
    {
        term2_saved = 1;
        page_table[VIDEO_OFFSET] = (VIDEO_OFFSET * VMEM_PAGE_SIZE) | SUPERVISER_EN;
        flush_TLB();
        clear();
        num_shells_init--;
        send_eoi(PIT_PIC_IRQ);
        execute((uint8_t*) "shell");
    }
    // if terminal 2 has not been saved execute terminal 2
    else if(next_terminal_num == 3)
    {
        term3_saved = 1;
        page_table[VIDEO_OFFSET] = (VIDEO_OFFSET * VMEM_PAGE_SIZE) | SUPERVISER_EN;
        flush_TLB();
        clear();
        num_shells_init--;
        send_eoi(PIT_PIC_IRQ);
        execute((uint8_t*) "shell");
    }

    // send PIT eoi
    send_eoi(PIT_PIC_IRQ);
}

/*
    get_next_terminal_num:
        DESCRIPTION: returns next terminal number based on current terminal number in a round robin fashion
        Input: current terminal number
        Output: next terminal number
        Side Effect: none
*/
int get_next_terminal_num(int cur_terminal_num)
{
    // different results depending on current terminal number
    switch(cur_terminal_num)
    {
        // current terminal number is 1
        case 1:
            // if terminal 2 is initialized
            if(terminal_two_init == 1)
                return 2;
            // if terminal 3 is initialized
            else if(terminal_three_init == 1)
                return 3;
            else 
                return 1;
        // current terminal number is 2
        case 2:
            // if terminal three is initialized
            if(terminal_three_init == 1)
                return 3;
            // if terminal 1 is inititlaized
            else if(terminal_one_init == 1)
                return 1;
            else 
                return 2;
        // current terminal number is 3
        case 3:
            // if terminal one is initialized
            if(terminal_one_init == 1)
                return 1;
            // if terminal two is initialized
            else if(terminal_two_init == 1)
                return 2;
            else 
                return 3;
    }
    return -1;
}
