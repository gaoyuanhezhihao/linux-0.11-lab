LDT=12
TASK=8
KERNEL_STK=12
ESP0=4
.globl switch_to, first_ret_from_kernel

.align 2
switch_to:
    pushl %ebp
    movl %esp, %ebp
    pushl %ecx
    pushl %ebx
    pushl %eax

    # change current task_struct
    movl TASK(%ebp), %ebx
    cmpl %ebx, current
    je 1f
    movl %ebx, %eax
    xchgl %eax, current
    
    # tss
    addl $4096, %ebx
    movl tss, %ecx
    movl %ebx, ESP0(%ecx)

    # kernel stack top
    movl %esp, KERNEL_STK(%eax) #old stack
    movl TASK(%ebp), %ebx
    movl KERNEL_STK(%ebx), %esp # next stack

    # LDT
    movl LDT(%ebp), %ecx
    lldt %cx
    
    # refresh %fs
    movl $0x17, %ecx
    mov %cx, %fs

    cmpl %eax,last_task_used_math 
    jne 1f
    clts
1: 
    popl %eax
    popl %ebx
    popl %ecx
    popl %ebp
    ret

.align 2
first_ret_from_kernel:
    popl %edx
    popl %edi
    popl %esi
    pop %gs
    pop %fs
    pop %es
    pop %ds
    iret
