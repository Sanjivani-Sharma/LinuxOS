#--------------------------------
#	EXCEPTION HANDLER LINKAGES
#--------------------------------
.global div_by_zero_handler_link
.global debug_handler_link
.global nmi_handler_link
.global breakpoint_handler_link
.global overflow_handler_link
.global bound_range_exceeded_handler_link
.global invalid_opcode_handler_link
.global device_not_available_handler_link
.global double_fault_handler_link
.global coprocessor_segment_overrun_handler_link
.global invalid_tss_handler_link
.global segment_not_present_handler_link
.global stack_fault_handler_link
.global general_protection_handler_link
.global page_fault_handler_link
.global floating_point_handler_link
.global alignment_check_handler_link
.global machine_check_handler_link
.global SIMD_floating_point_handler_link
.global intel_reserved_link
.global keyboard_handler_link
.global PIT_handler_link

#---------------------------------------
#		SYSTEM CALL HANDLER LINKAGE
#---------------------------------------
.global system_call_handler_link

#--------------------------------------
#		RTC INTERRUPT HANDLER LINKAGE
#--------------------------------------
.global rtc_handler_link

#-----------------------------------------------------
#	Defining all exception handler linkage functions
#-----------------------------------------------------
div_by_zero_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call div_by_zero_handler          # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
debug_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call debug_handler          # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
nmi_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call nmi_handler          # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
breakpoint_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call breakpoint_handler          # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
overflow_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call overflow_handler          # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
bound_range_exceeded_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call bound_range_exceeded_handler          # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
invalid_opcode_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call invalid_opcode_handler          # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
device_not_available_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call device_not_available_handler          # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
double_fault_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call double_fault_handler          # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
coprocessor_segment_overrun_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call coprocessor_segment_overrun_handler          # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
invalid_tss_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call invalid_tss_handler          # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
segment_not_present_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call segment_not_present_handler          # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
stack_fault_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call stack_fault_handler          # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
general_protection_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call general_protection_handler          # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
page_fault_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call page_fault_handler          # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
floating_point_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call floating_point_handler          # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
alignment_check_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call alignment_check_handler          # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
machine_check_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call machine_check_handler          # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
SIMD_floating_point_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call SIMD_floating_point_handler          # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
intel_reserved_link:
	pushal   # push registers
	pushfl   # push flags 
	call intel_reserved             # calling reserved function
	popfl    # pop flags 
	popal    # pop registers	
	iret 

#-------------------------------------------------------------------
#	Defining system call handler function
#-------------------------------------------------------------------
system_call_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call system_call_handler             # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 

#-------------------------------------------------------------------
#	Defining RTC interrupt handler function
#-------------------------------------------------------------------
rtc_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call rtc_handler             # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 

#-------------------------------------------------------------------
#	Defining Keyboard interrupt handler function
#-------------------------------------------------------------------
keyboard_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call keyboard_handler             # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 
	
#-------------------------------------------------------------------
#	Defining PIT interrupt handler function
#-------------------------------------------------------------------
PIT_handler_link:
	pushal   # push registers
	pushfl   # push flags 
	call PIT_handler             # calling handler function
	popfl    # pop flags 
	popal    # pop registers	
	iret 

