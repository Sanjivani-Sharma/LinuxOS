.global system_call_linkage



 #  system_call_linkage
 #  DESCRIPTION: jumps to command links
 #  INPUTS: none
 # 	 OUTPUTS: number fo copies completed
 #   SIDE EFFECTS: critical section jumps to command function
system_call_linkage:
cli
# saving registers
pushf
pushl %edi
pushl %esi
pushl %edx
pushl %ecx
pushl %ebx

# checking eax
decl %eax
cmpl $0, %eax
jl ABORT
cmpl $9, %eax
jg ABORT

# calling function
call *jump_table(, %eax, 4);
jmp DONE

ABORT: 
movl $-1, %eax

DONE:
popl %ebx
popl %ecx
popl %edx
popl %esi
popl %edi
popf
iret

jump_table:
	.long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn

