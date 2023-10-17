#ifndef _TERMINAL_SWITCH_H
#define _TERMINAL_SWITCH_H

#define MAX_TERMINAL_COUNT              3

int terminal_num;                               // current terminal number on display

int terminal_one_init;                          // flag if terminal 1 was initialized
int terminal_two_init;                          // flag if terminal 2 was initialized
int terminal_three_init;                        // flag if terminal 3 was intitialized


// terminal descriptor struct holds terminal data
typedef struct terminal_desc_t{
    int cursor_x;
    int cursor_y;
    int process_count;
    int processes[30];
    int base_process_num;
    int cur_process_num;
}terminal_desc_t;

// terminal descriptor array that holds each terminal's data
terminal_desc_t terminal_desc_array[MAX_TERMINAL_COUNT];

/*
    swap_terminal_to:
        DESCRIPTION: swaps terminals to the desired terminal 
        Input: destination terminal
        output: none
        Side Effects: swaps video memory and terminal data
*/
void swap_terminal_to(int terminal_dest);

/*
    init_terminal_num
        DESCRIPTION: intialize terminal_ind data
        Input: terminal index
        Output: none
        Side Effects: none
*/
void init_terminal_num(int terminal_ind);

/*
    restore_terminal_num
        DESCRIPTION: restore terminal_ind data
        Input: terminal index
        Output: none
        Side Effects: none
*/
void restore_terminal(int terminal_ind);


#endif
