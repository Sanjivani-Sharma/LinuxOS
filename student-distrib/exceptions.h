#define TOT_EXCEPTIONS      32;

#define EXCEPTION_STAT      256;

/* 
 * set_exceptions
 *   DESCRIPTION: Initializes the IDT 
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: Set the entry of the IDT and call SET_IDT_ENTRY
 */
void set_exceptions();

/* 
 * divide by zero handler
 *   DESCRIPTION: Handles divide by zero error 
 */
void div_by_zero_handler();

/* 
 * debug_handler
 *   DESCRIPTION: Handles debug_handler error
 */
void debug_handler();

/* 
 * nmi_handler
 *   DESCRIPTION: Handles nmi_handler error
 */
void nmi_handler();

/* 
 * breakpoint_handler
 *   DESCRIPTION: Handles breakpoint_handler error
 */
void breakpoint_handler();

/* 
 * overflow_handler
 *   DESCRIPTION: Handles overflow_handler error
 */
void overflow_handler();

/* 
 * bound_range_exceeded_handler
 *   DESCRIPTION: Handles bound_range_exceeded_handler error
 */
void bound_range_exceeded_handler();

/* 
 * invalid_opcode_handler
 *   DESCRIPTION: Handles invalid_opcode_handler error
 */
void invalid_opcode_handler();

/* 
 * device_not_available_handler
 *   DESCRIPTION: Handles device_not_available_handler error
 */
void device_not_available_handler();

/* 
 * double_fault_handler
 *   DESCRIPTION: Handles double_fault_handler error
 */
void double_fault_handler();

/* 
 * coprocessor_segment_overrun_handler
 *   DESCRIPTION: Handles coprocessor_segment_overrun_handler error
 */
void coprocessor_segment_overrun_handler();

/* 
 * invalid_tss_handler
 *   DESCRIPTION: Handles invalid_tss_handler error
 */
void invalid_tss_handler();

/* 
 * segment_not_present_handler
 *   DESCRIPTION: Handles segment_not_present_handler error
 */
void segment_not_present_handler();
/* 
 * stack_fault_handler
 *   DESCRIPTION: Handles stack_fault_handler error
 */
void stack_fault_handler();


/* 
 * general_protection_handler
 *   DESCRIPTION: Handles general_protection_handler error
 */
void general_protection_handler();

/* 
 * page_fault_handler
 *   DESCRIPTION: Handles page_fault_handler error
 */
void page_fault_handler();

/* 
 * floating_point_handler
 *   DESCRIPTION: Handles floating_point_handler error
 */
void floating_point_handler();

/* 
 * alignment_check_handler
 *   DESCRIPTION: Handles alignment_check_handler error
 */
void alignment_check_handler();

/* 
 * machine_check_handler
 *   DESCRIPTION: Handles machine_check_handler error
 */
void machine_check_handler();

/* 
 * SIMD_floating_point_handler
 *   DESCRIPTION: Handles SIMD_floating_point_handler error
 */
void SIMD_floating_point_handler();

/* 
 * intel_reserved
 *   DESCRIPTION: Handles intel_reserved error
 */
void intel_reserved();
