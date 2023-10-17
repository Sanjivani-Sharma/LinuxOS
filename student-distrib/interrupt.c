#include "enable_paging.h"
#include "interrupt.h"
#include "IDT_linkage_functions.h"
#include "x86_desc.h"
#include "RTC.h"
#include "lib.h"
#include "i8259.h"
#include "keyboard_driver.h"
#include "terminal_switch.h"
#include "scheduler.h"
#include "paging.h"
#include "Sys_Calls.h"


int caps_lock_count = 0;										// keep track of how many times caps_lock was interacted with

int lshift_pressed = 0;											// 1 if left shift is pressed
int rshift_pressed = 0;											// 1 if right shift is pressed
int lcntrl_pressed = 0;											// 1 if left control is pressed

int key_count = 0;												// how many keys are registered in keyboard buffer

// testing flags
int test_fl = 0;
int test_f = 0;

int lalt_pressed = 0;											// 1 if left alt was pressed

int f1_pressed = 0;												// 1 if f1 is pressed
int f2_pressed = 0;												// 1 if f2 is pressed
int f3_pressed = 0;												// 1 if f3 is pressed

// scan codes for select keys
unsigned char scan_code_ascii_data[ASCII_ARRAY_SIZE] = 
{
	'\0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'0', '-', '=', '\0', '\0', 'q', 'w', 'e', 'r', 't',
	'y', 'u', 'i', 'o', 'p', '[', ']', '\0', '\0', 'a',
	's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', 
	'`', '\0', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
	',', '.', '/', '\0', '\0', '\0', ' '
};

// shift scan codes for select keys
unsigned char scan_code_ascii_shift_data[ASCII_ARRAY_SIZE] = 
{
	'\0', '!', '@', '#', '$', '%', '^', '&', '*', '(',
	')', '_', '+', '\0', '\0', 'Q', 'W', 'E', 'R', 'T',
	'Y', 'U', 'I', 'O', 'P', '{', '}', '\0', '\0', 'A',
	'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', 
	'~', '\0', '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',
	'<', '>', '?', '\0', '\0', '\0', ' '
};

/* 
 * set_RTC
 *   DESCRIPTION: Initializes the RTC in the IDT 
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: Set the entry of the IDT and call SET_IDT_ENTRY
 */
void set_RTC()
{
    idt[RTC_IND].seg_selector = KERNEL_CS;   // set up elements in the RTC entry
	idt[RTC_IND].reserved4 = 0;			
	idt[RTC_IND].reserved3 = 0;		
	idt[RTC_IND].reserved2 = 1;
	idt[RTC_IND].reserved1 = 1;
	idt[RTC_IND].size = 1;
	idt[RTC_IND].reserved0 = 0;
	idt[RTC_IND].dpl = 0;          
	idt[RTC_IND].present = 1;
	SET_IDT_ENTRY(idt[RTC_IND], rtc_handler_link);   // set idt entry for RTC
}

/* 
 * set_keyboard
 *   DESCRIPTION: Initializes the Keyboard in the IDT 
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: Set the entry of the IDT and call SET_IDT_ENTRY
 */
void set_keyboard()
{
	idt[KEYBOARD_IND].seg_selector = KERNEL_CS;   // set up elements in the keyboard entry
	idt[KEYBOARD_IND].reserved4 = 0;			
	idt[KEYBOARD_IND].reserved3 = 0;		
	idt[KEYBOARD_IND].reserved2 = 1;
	idt[KEYBOARD_IND].reserved1 = 1;
	idt[KEYBOARD_IND].size = 1;
	idt[KEYBOARD_IND].reserved0 = 0;
	idt[KEYBOARD_IND].dpl = 0;          
	idt[KEYBOARD_IND].present = 1;
	SET_IDT_ENTRY(idt[KEYBOARD_IND], keyboard_handler_link);
}

/* 
 * set_pit
 *   DESCRIPTION: Initializes the PIT in the IDT 
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: Set the entry of the IDT and call SET_IDT_ENTRY
 */
void set_PIT()
{
	idt[PIT_IND].seg_selector = KERNEL_CS;   // set up elements in the keyboard entry
	idt[PIT_IND].reserved4 = 0;			
	idt[PIT_IND].reserved3 = 0;		
	idt[PIT_IND].reserved2 = 1;
	idt[PIT_IND].reserved1 = 1;
	idt[PIT_IND].size = 1;
	idt[PIT_IND].reserved0 = 0;
	idt[PIT_IND].dpl = 0;          
	idt[PIT_IND].present = 1;
	SET_IDT_ENTRY(idt[PIT_IND], PIT_handler_link);
}

/*
	rtc_handler:
		DESCRIPTION: Handles RTC interrupts
		INPUT: none
		OUTPUT: none
		SIDE EFFECTS: when frequency/counter conditions are met, it actiates interrupt flag for respective termina;
*/
/* ending portion of code credited to https://wiki.osdev.org/RTC#Interrupts_and_Register_C */
void rtc_handler()
{
	// looping through terminals
	int i;
	for(i = 0; i < MAX_TERMINAL_COUNT; i++)
	{
		// getting pcb
		uint32_t pcb_addr = EIGHT_MEGABYTE - EIGHT_KILOBYTES*(terminal_desc_array[i].cur_process_num + 1);
		pcb_t* rtc_pcb = (pcb_t*) pcb_addr;
		
		// incrementing counter
		rtc_pcb->RTC_counter_save++;
		
		// if counter > required ticks, disable interrupt
		if(rtc_pcb->RTC_counter_save > rtc_pcb->RTC_ticks * 2)
		{
			// reset counter and turn off interrupt flag for respective terminal
			rtc_pcb->RTC_counter_save = 0;
			interrupt[i] = 0;
		}
	}
	
	// testing RTC interrupt
	if(test_f == 1)
		test_interrupts();

	send_eoi(RTC_PIC_IRQ);
	outb(REG_C, IND_PORT);	// select register C
	inb(IND_DATA_PORT);		// just throw away contents
}

/* 
 * keyboard_handler
 *   DESCRIPTION: Handles keyboard_handler interrupt (when keyboard is pressed)
 *   INPUTS: none
 * 	 OUTPUTS: none
 *   SIDE EFFECTS: Reads scan code input of keyboard and prints to screen
 *  
 */
void keyboard_handler()
{
	// Reads Scan code 
	unsigned char scan_code; 
	scan_code = inb(KEYBOARD_DATA_PORT);

	// save physical address of current video memory indirection to restore later
	uint32_t save_vid = page_table[VIDEO_OFFSET];
	uint32_t save_cur_terminal_num = current_terminal_num;

	// mapping virtual video address to physical video address
	page_table[VIDEO_OFFSET] = (VIDEO_OFFSET * VMEM_PAGE_SIZE) | SUPERVISER_EN;
	flush_TLB();

	// disable interrupts and process scan code
	cli();
	current_terminal_num = terminal_num;
	switch(scan_code)
	{
		// if left alt was pressed
		case LALT_PRESSED_SCAN:
			// set left alt pressed flag
			lalt_pressed = 1;
			
			// restore video buffer address
			page_table[VIDEO_OFFSET] = save_vid;
			flush_TLB();
			current_terminal_num = save_cur_terminal_num;

			// enable interrupts
			sti();

			// return and send eoi
            send_eoi(KEYBOARD_PIC_IRQ);
			return;

		// if left alt was released
		case LALT_RELEASED_SCAN:
			// release left alt pressed flag
			lalt_pressed = 0;

			// restore video buffer address
			page_table[VIDEO_OFFSET] = save_vid;
			flush_TLB();
			current_terminal_num = save_cur_terminal_num;

			// restore interrupts
			sti();

			// send eoi and return
            send_eoi(KEYBOARD_PIC_IRQ);
			return;
		
		// if f1 is pressed
		case F1_PRESSED_SCAN:
			// check if left alt is pressed
			if(lalt_pressed == 1)
			{
				// swap to terminal 1
				swap_terminal_to(1);
			}
			// retore physical buffer video page
			page_table[VIDEO_OFFSET] = save_vid;
			flush_TLB();
			current_terminal_num = save_cur_terminal_num;
			// enable interrupts
			sti();

			// return and send eoi
			send_eoi(KEYBOARD_PIC_IRQ);
			
			return;
		
		// if f1 was released
		case F1_RELEASED_SCAN:
			f1_pressed = 0;
			page_table[VIDEO_OFFSET] = save_vid;
			flush_TLB();
			current_terminal_num = save_cur_terminal_num;
			sti();
			send_eoi(KEYBOARD_PIC_IRQ);
			return;
		// if f2 was pressed
		case F2_PRESSED_SCAN:
			
			if(lalt_pressed == 1)
			{
				swap_terminal_to(2);
			}
			page_table[VIDEO_OFFSET] = save_vid;
			flush_TLB();
			current_terminal_num = save_cur_terminal_num;
			sti();
			send_eoi(KEYBOARD_PIC_IRQ);
			return;
		// if f2 was released
		case F2_RELEASED_SCAN:
			f2_pressed = 0;
			page_table[VIDEO_OFFSET] = save_vid;
			flush_TLB();
			current_terminal_num = save_cur_terminal_num;
			sti();
			send_eoi(KEYBOARD_PIC_IRQ);
			return;
		// if f3 was pressed
		case F3_PRESSED_SCAN:
			
			if(lalt_pressed == 1)
			{
				swap_terminal_to(3);
			}
			page_table[VIDEO_OFFSET] = save_vid;
			flush_TLB();
			current_terminal_num = save_cur_terminal_num;
			sti();
			send_eoi(KEYBOARD_PIC_IRQ);
			return;
		// if f3 was released
		case F3_RELEASED_SCAN:
			f3_pressed = 0;
			page_table[VIDEO_OFFSET] = save_vid;
			flush_TLB();
			current_terminal_num = save_cur_terminal_num;
			sti();
			send_eoi(KEYBOARD_PIC_IRQ);
			return;
		// if tab was pressed
		case KEYBOARD_TAB_SCAN:
			printf("    ");
			key_count+=4;
			page_table[VIDEO_OFFSET] = save_vid;
			flush_TLB();
			current_terminal_num = save_cur_terminal_num;
			sti();
			send_eoi(KEYBOARD_PIC_IRQ);
			return;
		// if left shift was pressed
		case KEYBOARD_LSHIFT_PRESSED_SCAN:
			lshift_pressed = 1;
			page_table[VIDEO_OFFSET] = save_vid;
			flush_TLB();
			current_terminal_num = save_cur_terminal_num;
			sti();
			send_eoi(KEYBOARD_PIC_IRQ);
			return;
		// if right shift was pressed
		case KEYBOARD_RSHIFT_PRESSED_SCAN:
			rshift_pressed = 1;
			page_table[VIDEO_OFFSET] = save_vid;
			flush_TLB();
			current_terminal_num = save_cur_terminal_num;
			sti();
			send_eoi(KEYBOARD_PIC_IRQ);
			return;
		// if left shift was released
		case KEYBOARD_LSHIFT_RELEASED_SCAN:
			lshift_pressed = 0;
			page_table[VIDEO_OFFSET] = save_vid;
			flush_TLB();
			current_terminal_num = save_cur_terminal_num;
			sti();
			send_eoi(KEYBOARD_PIC_IRQ);
			return;
		// if keyboard right shift was rleased
		case KEYBOARD_RSHIFT_RELEASED_SCAN:
			rshift_pressed = 0;
			page_table[VIDEO_OFFSET] = save_vid;
			flush_TLB();
			current_terminal_num = save_cur_terminal_num;
			sti();
			send_eoi(KEYBOARD_PIC_IRQ);
			return;
		// if left control was pressed
		case KEYBOARD_LCNTRL_PRESSED_SCAN:
			lcntrl_pressed = 1;
			page_table[VIDEO_OFFSET] = save_vid;
			flush_TLB();
			current_terminal_num = save_cur_terminal_num;
			sti();
			send_eoi(KEYBOARD_PIC_IRQ);
			return;
		// if keyboard lefr control was released
		case KEYBOARD_LCNTRL_RELEASED_SCAN:
			lcntrl_pressed = 0;
			page_table[VIDEO_OFFSET] = save_vid;
			flush_TLB();
			current_terminal_num = save_cur_terminal_num;
			sti();
			send_eoi(KEYBOARD_PIC_IRQ);
			return;
		// if enter was pressed
		case KEYBOARD_ENTER_SCAN:
			// check and implement scroll screen if necessary
			scroll_screen();
			printf("\n");
			keyboard_buffer[key_count] = '\n';
			key_count++;

			// reading keyboard input into terminal buffer
			terminal_read_kb(keyboard_buffer, key_count);

			// testing
			if(test_fl == 1)
			{
				printf("buf: ");
				terminal_write();
				putc('\n');
			}
			
			// reset keycount and raise the read flag for respective terminal
			key_count = 0;
			read_flags[terminal_num - 1] = 1;
			
			// reset page for video buffer
			page_table[VIDEO_OFFSET] = save_vid;
			flush_TLB();
			current_terminal_num = save_cur_terminal_num;
			sti();
			send_eoi(KEYBOARD_PIC_IRQ);
			return;
		// if backspace was clicked
		case KEYBOARD_BACKSPACE_PRESSED_SCAN:
			// check if backspace should be allowed
			if(key_count > 0)
			{
				// remove last typed key from screen and keyboard buffer
				remc(key_count);
				key_count--;
			}
			// restoring background video page
			page_table[VIDEO_OFFSET] = save_vid;
			flush_TLB();
			current_terminal_num = save_cur_terminal_num;
			sti();
			send_eoi(KEYBOARD_PIC_IRQ);
			return;
		// if capslock was pressed
		case KEYBOARD_CAPS_SCAN:
			// increment capslock count
			caps_lock_count++;

			// restore background video page
			page_table[VIDEO_OFFSET] = save_vid;
			flush_TLB();
			current_terminal_num = save_cur_terminal_num;
			sti();
			send_eoi(KEYBOARD_PIC_IRQ);
			return;
		default:
			break;
	}
	
	// if scan_code can be displayed on screen
	if(scan_code <= ASCII_ARRAY_SIZE) // If scan code not initialized
	{
		// check if screen clean request was entered
		if(lcntrl_pressed == 1 && scan_code_ascii_shift_data[scan_code - 1] == 'L')
		{
			key_count = 0;
			//read_flag = 1;
			clear();
			update_cursor(0,0);
		}
		// check if visible lettter was typed
		else if(key_count < keyboard_buffer_length - 1)
		{
			// check for capital
			if((lshift_pressed == 1 || rshift_pressed == 1 || caps_lock_count % 2 == 1) && scan_code_ascii_shift_data[scan_code - 1] != '\0')
			{
				// print capital letters
				keyboard_buffer[key_count] = scan_code_ascii_shift_data[scan_code - 1];
				key_count++;
				printf("%c", scan_code_ascii_shift_data[scan_code - 1]);
			}
			else if(scan_code_ascii_shift_data[scan_code - 1] != '\0')
			{
				// print loercase letters
				keyboard_buffer[key_count] = scan_code_ascii_data[scan_code - 1];
				key_count++;
				printf("%c", scan_code_ascii_data[scan_code - 1]);
			}
		}
	}
	
	// restoring background video page
	page_table[VIDEO_OFFSET] = save_vid;
	flush_TLB();
	current_terminal_num = save_cur_terminal_num;
	sti();
	send_eoi(KEYBOARD_PIC_IRQ);
}

/* 
 * PIT_handler
 *   DESCRIPTION: Handles PIT_handler interrupt
 *   INPUTS: none
 * 	 OUTPUTS: none
 *   SIDE EFFECTS: none
 *  
 */
void PIT_handler()
{
	//sti();
	switch_task();
}

void test_rw()
{
	test_fl = 1;
}

void rtc_test_func(int i)
{
    test_f = 1;
	rtc_write(i);
}
