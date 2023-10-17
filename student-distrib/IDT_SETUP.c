#include "IDT_SETUP.h"
#include "x86_desc.h"
#include "exceptions.h"
#include "IDT_linkage_functions.h"
#include "RTC.h"
#include "interrupt.h"
#include "lib.h"
#include "Sys_Call_Linkage.h"
/* 
 * init_idt
 *   DESCRIPTION: Initializes the IDT by calling the functions to populate
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: calls set functions and lidt
 */
void init_idt()
{
    // Set first 32 entries in IDT as exceptions
    set_exceptions();
    // setting x80 to handle system calls
    set_system_call();
    // set RTC interrupt vector in IDT
    set_RTC();
    // set keyboard interrupt vector in IDT
    set_keyboard();
    // set PIT interrupt vector in IDT
    set_PIT();

    lidt(idt_desc_ptr);
}

/* 
 * set_system_call
 *   DESCRIPTION: Initializes the IDT with the system calls
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: calls set functions and lidt
 */
void set_system_call()
{
    idt[SYS_CALL_IND].seg_selector = KERNEL_CS;    // set up elements in the exception entry
    idt[SYS_CALL_IND].reserved4 = 0;			
	idt[SYS_CALL_IND].reserved3 = 0;		
    idt[SYS_CALL_IND].reserved2 = 1;
	idt[SYS_CALL_IND].reserved1 = 1;
	idt[SYS_CALL_IND].size = 1;
	idt[SYS_CALL_IND].reserved0 = 0;
	idt[SYS_CALL_IND].dpl = 3;          
	idt[SYS_CALL_IND].present = 1;
    SET_IDT_ENTRY(idt[SYS_CALL_IND], system_call_linkage);
}

/* 
 * system_call_handler
 *   DESCRIPTION: Handles system_call_handler error
 */
void system_call_handler()
{
    puts("SYSTEM CALL CALLED");
    while(1);
}

