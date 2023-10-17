#include "x86_desc.h"
#include "exceptions.h"
#include "lib.h"
#include "IDT_linkage_functions.h"
#include "Sys_Calls.h"
#include "scheduler.h"

int EXCEPTION_SAT = 256;
/* 
 * set_exceptions
 *   DESCRIPTION: Initializes the IDT 
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: Set the entry of the IDT and call SET_IDT_ENTRY
 */
void set_exceptions()
{
    int i;
    int exception_count = TOT_EXCEPTIONS;
    for(i = 0; i < exception_count; i++)
    {
        idt[i].seg_selector = KERNEL_CS;    // set up elements in the exception entry
		idt[i].reserved4 = 0;			
		idt[i].reserved3 = 0;		
		idt[i].reserved2 = 1;
		idt[i].reserved1 = 1;
		idt[i].size = 1;
		idt[i].reserved0 = 0;
		idt[i].dpl = 0;          
		idt[i].present = 1;
        SET_IDT_ENTRY(idt[i], intel_reserved_link);
    }

    SET_IDT_ENTRY(idt[0], div_by_zero_handler_link);                            // interrupt vector for divide by 0
	SET_IDT_ENTRY(idt[1], debug_handler_link);
	SET_IDT_ENTRY(idt[2], nmi_handler_link);
	SET_IDT_ENTRY(idt[3], breakpoint_handler_link);
	SET_IDT_ENTRY(idt[4], overflow_handler_link);
	SET_IDT_ENTRY(idt[5], bound_range_exceeded_handler_link);
	SET_IDT_ENTRY(idt[6], invalid_opcode_handler_link);
	SET_IDT_ENTRY(idt[7], device_not_available_handler_link);
	SET_IDT_ENTRY(idt[8], double_fault_handler_link);
	SET_IDT_ENTRY(idt[9], coprocessor_segment_overrun_handler_link);
	SET_IDT_ENTRY(idt[10], invalid_tss_handler_link);
	SET_IDT_ENTRY(idt[11], segment_not_present_handler_link);
	SET_IDT_ENTRY(idt[12], stack_fault_handler_link);
	SET_IDT_ENTRY(idt[13], general_protection_handler_link);
	SET_IDT_ENTRY(idt[14], page_fault_handler_link);
	SET_IDT_ENTRY(idt[16], floating_point_handler_link);
	SET_IDT_ENTRY(idt[17], alignment_check_handler_link);
	SET_IDT_ENTRY(idt[18], machine_check_handler_link);
	SET_IDT_ENTRY(idt[19], SIMD_floating_point_handler_link);
}

/* 
 * divide by zero handler
 *   DESCRIPTION: Handles divide by zero error 
 */
void div_by_zero_handler()
{
    printf("Divide By Zero Exception\n");
    halt(EXCEPTION_SAT);
}

/* 
 * debug_handler
 *   DESCRIPTION: Handles debug_handler error
 */
void debug_handler()
{
    printf("Debug Exception\n");
    halt(EXCEPTION_SAT);
}

/* 
 * nmi_handler
 *   DESCRIPTION: Handles nmi_handler error
 */
void nmi_handler()
{
    printf("non-maskable interrupt\n");
    halt(EXCEPTION_SAT);
}

/* 
 * breakpoint_handler
 *   DESCRIPTION: Handles breakpoint_handler error
 */
void breakpoint_handler()
{
    printf("Breakpoint Exception\n");
    halt(EXCEPTION_SAT);
}

/* 
 * overflow_handler
 *   DESCRIPTION: Handles overflow_handler error
 */
void overflow_handler()
{
    printf("Overflow Exception\n");
    halt(EXCEPTION_SAT);
}

/* 
 * bound_range_exceeded_handler
 *   DESCRIPTION: Handles bound_range_exceeded_handler error
 */
void bound_range_exceeded_handler()
{
    printf("Bound Range Exceeded Exception\n");
    halt(EXCEPTION_SAT);
}

/* 
 * invalid_opcode_handler
 *   DESCRIPTION: Handles invalid_opcode_handler error
 */
void invalid_opcode_handler()
{
    printf("Invalid opcode Exception\n");
    halt(EXCEPTION_SAT);
}

/* 
 * device_not_available_handler
 *   DESCRIPTION: Handles device_not_available_handler error
 */
void device_not_available_handler()
{
    printf("Device Not Available Exception\n");
    halt(EXCEPTION_SAT);
}

/* 
 * double_fault_handler
 *   DESCRIPTION: Handles double_fault_handler error
 */
void double_fault_handler()
{
    printf("Double fault Exception\n");
    halt(EXCEPTION_SAT);
}

/* 
 * coprocessor_segment_overrun_handler
 *   DESCRIPTION: Handles coprocessor_segment_overrun_handler error
 */
void coprocessor_segment_overrun_handler()
{
    printf("Coprocessor Segment Overrun Exception\n");
    halt(EXCEPTION_SAT);
}

/* 
 * invalid_tss_handler
 *   DESCRIPTION: Handles invalid_tss_handler error
 */
void invalid_tss_handler()
{
    printf("Invalid TSS Exception\n");
    halt(EXCEPTION_SAT);
}

/* 
 * segment_not_present_handler
 *   DESCRIPTION: Handles segment_not_present_handler error
 */
void segment_not_present_handler()
{
    printf("Segment not present Exception\n");
    halt(EXCEPTION_SAT);
}

/* 
 * stack_fault_handler
 *   DESCRIPTION: Handles stack_fault_handler error
 */
void stack_fault_handler()
{
    printf("Stack Fault Exception\n");
    halt(EXCEPTION_SAT);
}

/* 
 * general_protection_handler
 *   DESCRIPTION: Handles general_protection_handler error
 */
void general_protection_handler()
{
    printf("General Protection Exception\n");
    halt(EXCEPTION_SAT);
}

/* 
 * page_fault_handler
 *   DESCRIPTION: Handles page_fault_handler error
 */
void page_fault_handler()
{
    printf("Page Fault Exception\n");
    halt(EXCEPTION_SAT);
}

/* 
 * floating_point_handler
 *   DESCRIPTION: Handles floating_point_handler error
 */
void floating_point_handler()
{
    printf("Floating Point Exception\n");
    halt(EXCEPTION_SAT);
}

/* 
 * alignment_check_handler
 *   DESCRIPTION: Handles alignment_check_handler error
 */
void alignment_check_handler()
{
    printf("Alignment Check Exception\n");
    halt(EXCEPTION_SAT);
}

/* 
 * machine_check_handler
 *   DESCRIPTION: Handles machine_check_handler error
 */
void machine_check_handler()
{
    printf("Machine Check Exception\n");
    halt(EXCEPTION_SAT);
}

/* 
 * SIMD_floating_point_handler
 *   DESCRIPTION: Handles SIMD_floating_point_handler error
 */
void SIMD_floating_point_handler()
{
    printf("SIMD floating point Exception\n");
    halt(EXCEPTION_SAT);
}

/* 
 * intel_reserved
 *   DESCRIPTION: Handles intel_reserved error
 */
void intel_reserved()
{
    printf("INTEL RESERVED EXCEPTION");
    halt(EXCEPTION_SAT);
}
