#include "keyboard_driver.h"
#include "scheduler.h"
#include "lib.h"


/* 
 *   terminal_open
 *   DESCRIPTION: opens terminal and does nothing
 *   INPUTS: NONE
 * 	 OUTPUTS: NONE
 *   SIDE EFFECTS: NONE
 */
int terminal_open()
{
    return 0;
}

/* 
 *   terminal_close
 *   DESCRIPTION: closes terminal and does nothing
 *   INPUTS: NONE
 * 	 OUTPUTS: NONE
 *   SIDE EFFECTS: NONE
 */
int terminal_close()
{
    return 0;
}

/* 
 *   terminal_read
 *   DESCRIPTION: reads from buffer and stores it into buf
 *   INPUTS: buffer -- incomming data from terminal 
 *           buffer_size -- size of incomming buffer data
 * 	 OUTPUTS: size of buffer data read
 *   SIDE EFFECTS: NONE
 */
int terminal_read_kb(const char* buffer, int buffer_size)
{
    //printf("reading %s from %d\n", buffer, terminal_num);
    int i;
    // copying data to buf
    for(i = 0; i < buffer_size; i++)
        *(buf[terminal_num - 1] + i) = buffer[i];
    
    buf_size = buffer_size;
    return buffer_size;
}

/* 
 *   terminal_read
 *   DESCRIPTION: reads from buffer and stores it into buf
 *   INPUTS: buffer -- incomming data from terminal 
 *           buffer_size -- size of incomming buffer data
 * 	 OUTPUTS: number fo copies completed
 *   SIDE EFFECTS: NONE
 */
int terminal_read(char* buffer, int buffer_size)
{
    //printf("buf size: %d", buffer_size);
    while(read_flags[cur_terminal_num - 1] == 0);
    while(cur_terminal_num != terminal_num);
    cli();
    read_flags[cur_terminal_num - 1] = 0;
    int i;
    // copying data from buf
    for(i = 0; i < buffer_size && i < buf_size && i < keyboard_buffer_length - 1; i++)
        buffer[i] = *(buf[cur_terminal_num - 1] + i);
    
    if(i == keyboard_buffer_length - 1)
        buffer[i] = '\0';

    return i;
}

/* 
 *   terminal_write
 *   DESCRIPTION: prints buffer to screen
 *   INPUTS: NONE
 * 	 OUTPUTS: size of buffer printed
 *   SIDE EFFECTS: prints string to screen
 */
int terminal_write_buffer(const uint8_t* buffer, int32_t nbytes)
{
    int i;
    for(i = 0; i < nbytes; i++)
        putc(buffer[i]);
    
    return nbytes;
}

/* 
 *   terminal_write
 *   DESCRIPTION: prints buf to screen
 *   INPUTS: NONE
 * 	 OUTPUTS: size of buffer printed
 *   SIDE EFFECTS: prints string to screen
 */
int terminal_write()
{
    int i;
    for(i = 0; i < buf_size; i++)
        putc(*(buf[terminal_num - 1] + i));
    
    return buf_size;
}

