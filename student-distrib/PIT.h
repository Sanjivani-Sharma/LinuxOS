#ifndef _PIT_H
#define _PIT_H


#define MODE_TWO_LO_HI_BYTE_MODE            0x36                    // macro for mode 2 (Low High Byte Mode)
#define CHANNEL0_DATA_PORT                  0x40                    // macro for Channel 0 port
#define COMMAND_REGISTER                    0x43                    // macro for command register
#define LOW_BYTE_MASK                       0xFF                    // mask to isolate lower byte
#define HIGH_BYTE_MASK                      0xFF00                  // mask to isolate higher byte
#define DEFAULT_PIT_FREQ                    100                     // Default PIT frequency

/*
	init_pit:
		DESCRIPTION: initializes the PIT
		input: none
		output: none
		Side Effect: innitializes PIT as Mode 2 (Low/High Byte mode)
*/
void init_pit();

/*
	set_pit_count:
		DESCRIPTION: sets frequency of PIT
		input:
			- frequency: desired frequency of PIT
		output: none
		SIDE EFFECTS: changes frequency of PIT
*/
// functions below credited to https://wiki.osdev.org/Programmable_Interval_Timer#Outputs
void set_pit_count(unsigned frequency);

#endif
