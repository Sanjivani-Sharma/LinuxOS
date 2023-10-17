#include "PIT.h"
#include "lib.h"

/*
	init_pit:
		DESCRIPTION: initializes the PIT
		input: none
		output: none
		Side Effect: innitializes PIT as Mode 2 (Low/High Byte mode)
*/
void init_pit()
{
    cli();
    outb(MODE_TWO_LO_HI_BYTE_MODE, COMMAND_REGISTER);
    sti();
}

/*
	set_pit_count:
		DESCRIPTION: sets frequency of PIT
		input:
			- frequency: desired frequency of PIT
		output: none
		SIDE EFFECTS: changes frequency of PIT
*/
// function credited to https://wiki.osdev.org/Programmable_Interval_Timer#Outputs
void set_pit_count(unsigned frequency) {
	// Disable interrupts
	cli();
 
	// Set low byte
	outb(frequency & LOW_BYTE_MASK, CHANNEL0_DATA_PORT);		// Low byte
	outb((frequency & HIGH_BYTE_MASK) >> 8, CHANNEL0_DATA_PORT);	// High byte

    sti();
} 
