#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#define RTC_PIC_IRQ         8
#define RTC_IND             0x28

#define PIT_PIC_IRQ         0
#define PIT_IND             0x20

#define KEYBOARD_PIC_IRQ    1
#define KEYBOARD_IND        0x21   
#define KEYBOARD_DATA_PORT  0x60  

#define ASCII_ARRAY_SIZE    57



#define KEYBOARD_LSHIFT_PRESSED_SCAN     0x2A
#define KEYBOARD_RSHIFT_PRESSED_SCAN     0x36
#define KEYBOARD_ENTER_SCAN              0x1C
#define KEYBOARD_CAPS_SCAN               0x3A
#define KEYBOARD_TAB_SCAN               0x0F

#define KEYBOARD_LSHIFT_RELEASED_SCAN    0xAA
#define KEYBOARD_RSHIFT_RELEASED_SCAN    0xB6

#define KEYBOARD_LCNTRL_PRESSED_SCAN     0x1D
#define KEYBOARD_LCNTRL_RELEASED_SCAN    0x9D

#define KEYBOARD_BACKSPACE_PRESSED_SCAN  0xE

#define LALT_PRESSED_SCAN                 0x38
#define LALT_RELEASED_SCAN                0xB8

#define F1_PRESSED_SCAN                   0x3B
#define F1_RELEASED_SCAN                  0xBB

#define F2_PRESSED_SCAN                   0x3C
#define F2_RELEASED_SCAN                  0xBC

#define F3_PRESSED_SCAN                   0x3D
#define F3_RELEASED_SCAN                  0xBD


/* 
 * set_RTC
 *   DESCRIPTION: Initializes the RTC in the IDT 
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: Set the entry of the IDT and call SET_IDT_ENTRY
 */
void set_RTC();

/* 
 * set_keyboard
 *   DESCRIPTION: Initializes the Keyboard in the IDT 
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: Set the entry of the IDT and call SET_IDT_ENTRY
 */
void set_keyboard();

/* 
 * set_pit
 *   DESCRIPTION: Initializes the PIT in the IDT 
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: Set the entry of the IDT and call SET_IDT_ENTRY
 */
void set_PIT();

/* code credited to https://wiki.osdev.org/RTC#Interrupts_and_Register_C */
void rtc_handler();

/* 
 * keyboard_handler
 *   DESCRIPTION: Handles keyboard_handler interrupt (when keyboard is pressed)
 *   INPUTS: none
 * 	 OUTPUTS: none
 *   SIDE EFFECTS: Reads scan code input of keyboard and prints to screen
 *  
 */
void keyboard_handler();

/* 
 * PIT_handler
 *   DESCRIPTION: Handles PIT_handler interrupt
 *   INPUTS: none
 * 	 OUTPUTS: none
 *   SIDE EFFECTS: none
 *  
 */
void PIT_handler();

void test_rw();
void rtc_test_func(int i);


#endif
