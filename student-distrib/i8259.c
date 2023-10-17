/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
/* Code Credited to https://wiki.osdev.org/8259_PIC   */
void i8259_init(void) {
	outb(ICW1, MASTER_8259_PORT);                           // starts the initialization sequence (in cascade mode)
	outb(ICW1, SLAVE_8259_PORT);
	outb(ICW2_MASTER, MASTER_8259_DATA_PORT);                 // ICW2: Master PIC vector offset
	outb(ICW2_SLAVE, SLAVE_8259_DATA_PORT);                 // ICW2: Slave PIC vector offset
	outb(ICW3_MASTER, MASTER_8259_DATA_PORT);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(ICW3_SLAVE, SLAVE_8259_DATA_PORT);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
 
	outb(ICW4, MASTER_8259_DATA_PORT);
	outb(ICW4, SLAVE_8259_DATA_PORT);
 
    master_mask = ~(ICW3_MASTER);
    slave_mask = ~(0x00);                       // slave mask should have all bits high so that no device interrupt is enabled
	outb(master_mask, MASTER_8259_DATA_PORT);   // restore saved masks.
	outb(slave_mask, SLAVE_8259_DATA_PORT);
}

/* Enable (unmask) the specified IRQ */
/* Code Credited to https://wiki.osdev.org/8259_PIC   */
void enable_irq(uint32_t irq_num) {
    // checking irq_num validity
    if(irq_num < PIC_PORT_Lower_BOUND || irq_num > PIC_PORT_UPPER_BOUND)
        return;
    
    // port and value data
    uint16_t port;
    uint8_t value;
 
    // setting port depending on irq_num
    if(irq_num < SLAVE_IRQ_NUM_MIN) {
        port = MASTER_8259_DATA_PORT;
    } else {
        outb(inb(MASTER_8259_DATA_PORT) & ~(ICW3_MASTER), MASTER_8259_DATA_PORT);  

        port = SLAVE_8259_DATA_PORT;
        irq_num -= SLAVE_IRQ_NUM_MIN;
    }
    // sending mask to port
    value = inb(port) & ~(1 << irq_num); 
    outb(value, port);  
}

/* Disable (mask) the specified IRQ */
/* Code Credited to https://wiki.osdev.org/8259_PIC   */
void disable_irq(uint32_t irq_num) {

    // checking irq_num validity
    if(irq_num < PIC_PORT_Lower_BOUND || irq_num > PIC_PORT_UPPER_BOUND)
        return;

    // port and value data
    uint16_t port;
    uint8_t value;
 
    // setting port depending on irq_num
    if(irq_num < SLAVE_IRQ_NUM_MIN) {
        port = MASTER_8259_DATA_PORT;
    } else {
        port = SLAVE_8259_DATA_PORT;
        irq_num -= SLAVE_IRQ_NUM_MIN;
    }
    // sending mask to port
    value = inb(port) | (1 << irq_num);
    outb(value, port); 
}

/* Send end-of-interrupt signal for the specified IRQ */
/* Code Credited to https://wiki.osdev.org/8259_PIC   */
void send_eoi(uint32_t irq_num) 
{
    // checking irq_num validity
    if(irq_num < PIC_PORT_Lower_BOUND || irq_num > PIC_PORT_UPPER_BOUND)
        return;
        
    // sending data depending on irq_num
    if(irq_num >= SLAVE_IRQ_NUM_MIN)
    {
		outb(EOI | (irq_num - SLAVE_IRQ_NUM_MIN), SLAVE_8259_PORT);
	    outb(EOI | SLAVE_MASTER_IRQ_NUM, MASTER_8259_PORT);
    }
    else
    {
        outb(EOI | irq_num, MASTER_8259_PORT);
    }
}
