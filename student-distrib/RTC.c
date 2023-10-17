#include "RTC.h"
#include "Sys_Calls.h"
#include "lib.h"
#include "scheduler.h"
#include "terminal_switch.h"


/*
    init_RTC:
    parameters: none
    return: none
    function: initialize the RTC by modifying ports
    side effects: modifies RTC ports
*/
/* code credited to https://wiki.osdev.org/RTC#Interrupts_and_Register_C */
void init_RTC()
{
    //printf("RTC initializing... \n");
    cli();                                  // disabling interrupts to properly program RTC
    outb(STATUS_REG_B, IND_PORT);		// select register B, and disable NMI
    char prev = inb(IND_DATA_PORT);	    // read the current value of register B
    outb(STATUS_REG_B, IND_PORT);		// set the index again (a read will reset the index to register D)
    outb(prev | 0x40, IND_DATA_PORT);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
    sti();                                  // enabling interrupts

    int rate = 7;

    rate &= 0x0F;			                // rate must be above 2 and not over 15 (masking bits > 15 because 0xF = 15)
    cli();                                  // disabling interrupts to properly program RTC
    outb(STATUS_REG_A, IND_PORT);		// set index to register A, disable NMI
    prev = inb(IND_DATA_PORT);	// get initial value of register A
    outb(STATUS_REG_A, IND_PORT);		// reset index to A
    outb((prev & 0xF0) | rate, IND_DATA_PORT); //write only our rate to A. Note, rate is the bottom 4 bits.
    sti();                              // enabling interrupts
}

/* 
 * rtc read
 *   DESCRIPTION: Returns return 0 on RTC interrupt
 *   INPUTS: none
 *   OUTPUTS: 0
 *   SIDE EFFECTS: waits for interrupt flag to be 0
 */
int rtc_read()
{
    int saved_terminal_num = cur_terminal_num;
    sti();
    interrupt[cur_terminal_num - 1] = 1; // Set interupt flag 
    while(interrupt[cur_terminal_num - 1]); // Wait for interrupt flag
    while(current_terminal_num != saved_terminal_num);
    cli();
    return 0;
}

/* 
 * rtc write
 *   DESCRIPTION: Changes frequency of the RTC
 *   INPUTS: Freq (integer)
 *   OUTPUTS: number of bytes written
 *   SIDE EFFECTS: waits for interrupt flag to be 0
 */
int rtc_write(int freq)
{
    // getting pcb for current process
    uint32_t pcb_addr = EIGHT_MEGABYTE - EIGHT_KILOBYTES*(terminal_desc_array[cur_terminal_num - 1].cur_process_num + 1);
    pcb_t* pcb = (pcb_t*) pcb_addr;

    // setting frequency and counter for pcb RTC
    pcb->RTC_save = freq;
    pcb->RTC_ticks = 512 / freq;
    return 0;
}

int rtc_write_sched(int freq)
{
    int ret = -1; // Initialize the return variable
    int rate; // for loop flag

    if (freq > 1024 || freq <= 0) // Frequency too high
        return ret;

    rate = 1; //initialize the rate value
    while (freq < max_freq){ // Calculate rate and check if freq is power of 2
        freq = freq << 1; // 
        rate++; // Calculate rate
        if (freq > max_freq) // Frequency not a power of 2
            return ret;
    }
    rate++;
    rate &= 0x0F;			                // rate must be above 2 and not over 15 (masking bits > 15 because 0xF = 15)
    cli();                                  // disabling interrupts to properly program RTC
    outb(STATUS_REG_A, IND_PORT);		// set index to register A, disable NMI
    char prev = inb(IND_DATA_PORT);	        // get initial value of register A
    outb(STATUS_REG_A, IND_PORT);		// reset index to A7
    outb((prev & 0xF0) | rate, IND_DATA_PORT); //write only our rate to A. Note, rate is the bottom 4 bits.
    sti();                              // enabling interrupts

    ret = 0;

    return ret;
}

/* 
 * rtc open
 *   DESCRIPTION: initializes RTC to 2HZ, return 0
 *   INPUTS: none
 *   OUTPUTS: 0
 *   SIDE EFFECTS: Calls Init_RTC()
 */
int rtc_open()
{
    // getting pcb of current process
    uint32_t pcb_addr = EIGHT_MEGABYTE - EIGHT_KILOBYTES*(terminal_desc_array[cur_terminal_num - 1].cur_process_num + 1);
    pcb_t* pcb = (pcb_t*) pcb_addr;

    // setting pcb settings
    pcb->RTC_save = 2;
    pcb->RTC_counter_save = 0;
    pcb->RTC_ticks = 512 / 2;
    pcb->RTC_flag = 1;
    return 0; 
}


/* 
 * rtc close
 *   DESCRIPTION: nothing for now
 *   INPUTS: none
 *   OUTPUTS: 0
 *   SIDE EFFECTS: nothing
 */
int rtc_close()
{
    // getting pcb for current process
    uint32_t pcb_addr = EIGHT_MEGABYTE - EIGHT_KILOBYTES*(terminal_desc_array[cur_terminal_num - 1].cur_process_num + 1);
    pcb_t* pcb = (pcb_t*) pcb_addr;

    // setting settings for RTC
    pcb->RTC_save = 2;
    pcb->RTC_counter_save = 0;
    pcb->RTC_ticks = 512 / 2;
    pcb->RTC_flag = 0;
    return 0;
}


