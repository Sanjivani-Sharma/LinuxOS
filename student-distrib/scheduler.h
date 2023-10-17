#ifndef _SCHEDULER_H
#define _SCHEDULER_H

int cur_process_num;                    // current process running by scheduler
int cur_terminal_num;                   // current terminal running by scheduler
int reset_RTC_counters_flag;            // resets RTC counters
int num_shells_init;                    // numeber of shells left to initialize

/*
    switch_task:
        DESCRIPTION: handles process switching between terminal programs
        Input: none
        Output: none
        Side Effect: alters values of esp and ebp and reroutes the process
*/
void switch_task();

/*
    get_next_terminal_num:
        DESCRIPTION: returns next terminal number based on current terminal number in a round robin fashion
        Input: current terminal number
        Output: next terminal number
        Side Effect: none
*/
int get_next_terminal_num(int cur_terminal_num);

#endif
