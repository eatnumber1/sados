#define	__ASM__20093__

#include "ide.h"

.data

    regstring: .string "esp: %x"
.text


.globl _callbios

_callbios:
    enter   $0, $0
    pushl   %ebx
    pushl   %ecx
    pushl   %edx
    pushl   %edi

    movl    $0x49435024, %eax   // $PCI
    movl    $0, %ebx
    movl    8(%ebp), %ebp
    pushl   %cs // because function returns with retf, which pops IP then CS

    pushl   %esp
    pushl   $regstring
    call    c_printf
    addl    $8, %esp

    call    %ebp

    pushl   %esp
    pushl   $regstring
    call    c_printf
    addl    $8, %esp

    cli
label:
    jmp     label

    //movl    $96, %edi
    //lcall   *(%edi)

    popl    %edi
    popl    %edx
    popl    %ecx
    popl    %ebx
    leave
    ret
