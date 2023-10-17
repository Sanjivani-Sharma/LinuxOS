#ifndef _IDT_SETUP_H
#define _IDT_SETUP_H

#define SYS_CALL_IND        0x80

/* 
 * init_idt
 *   DESCRIPTION: Initializes the IDT by calling the functions to populate
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: calls set functions and lidt
 */
void init_idt();

/* 
 * set_system_call
 *   DESCRIPTION: Initializes the IDT with the system calls
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: calls set functions and lidt
 */
void set_system_call();

/* 
 * system_call_handler
 *   DESCRIPTION: Handles system_call_handler error
 */
void system_call_handler();

#endif
