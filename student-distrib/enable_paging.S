.global loadPageDirectory
.global enablePaging
.global flush_TLB

#--------------------------------------------
#loadPageDirectory
#   parameters: pointer to page directory
#   outputs:    none
#   side effects:   loads cr3 with pointer
#---------------------------------------------
loadPageDirectory:
    # updating ebp
    pushl %ebp
    movl %esp, %ebp

    # getting parameter
    movl 8(%esp), %eax

    # updating cr3
    movl %eax, %cr3

    # leave and returning
    movl %ebp, %esp
    popl %ebp
    ret

#---------------------------------------------------------------
#enablePaging
#   parameters: none
#   outputs:    none
#   side effects:   changes enable bits in cr4 and cr0
#---------------------------------------------------------------
enablePaging:
    # updating ebp
    pushl %ebp
    movl %esp, %ebp	

    # enabling bit in cr4
    movl %cr4, %eax		
 	orl $0x00000010, %eax		
 	movl %eax, %cr4	

    # enabling bit in cr0		
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0

    # leave and ret
    movl %ebp, %esp
    popl %ebp
    ret

#---------------------------------------------------------------
#flush_TLB
#   parameters: none
#   outputs:    none
#   side effects:   flushes the TLB for paging
#---------------------------------------------------------------
flush_TLB:
    movl %cr3, %eax
    movl %eax, %cr3
    ret
