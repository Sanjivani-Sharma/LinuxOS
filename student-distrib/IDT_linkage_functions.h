#ifndef _IDT_LINKAGE_FUNCTIONS_H
#define _IDT_LINKAGE_FUNCTIONS_H

// exception handler linkages (Each are used to call the c handlers)
extern void div_by_zero_handler_link();
extern void debug_handler_link();
extern void nmi_handler_link();
extern void breakpoint_handler_link();
extern void overflow_handler_link();
extern void bound_range_exceeded_handler_link();
extern void invalid_opcode_handler_link();
extern void device_not_available_handler_link();
extern void double_fault_handler_link();
extern void coprocessor_segment_overrun_handler_link();
extern void invalid_tss_handler_link();
extern void segment_not_present_handler_link();
extern void stack_fault_handler_link();
extern void general_protection_handler_link();
extern void page_fault_handler_link();
extern void floating_point_handler_link();
extern void alignment_check_handler_link();
extern void machine_check_handler_link();
extern void SIMD_floating_point_handler_link();
extern void intel_reserved_link();

// system call handler linkage
extern void system_call_handler_link();

// RTC interrupt handler linkage
extern void rtc_handler_link();

// Keyboard interrupt handler linkage
extern void keyboard_handler_link();

// PIT interrupt handler linkage
extern void PIT_handler_link();

#endif
