// author - Christine Viets (cev5122)

#define	__ASM__20093__

#include "ide.h"

.data

    regstring: .string "esp: %x"
.text


.globl _callbios

_callbios:
label0:
    enter   $0, $0
    pushl   %ebx
    pushl   %ecx
    pushl   %edx
    pushl   %edi
    pushl   %esp

label1:

    movl    $0x49435024, %eax   // $PCI
    movl    $0, %ebx
    movl    12(%ebp), %ebp

    //pushl   %esp
    //pushl   $regstring
    //call    c_printf
    //addl    $8, %esp
    cli
label2:
    pushl   %cs // because function returns with retf, which pops IP then CS
    call    %ebp
    //lcall    (%ebp)
label3:
    //pushl   %esp
    //pushl   $regstring
    //call    c_printf
    //addl    $8, %esp

//esp is +4
    //movl    $96, %edi
    //lcall   *(%edi)

    popl    %esp
    popl    %edi
    popl    %edx
    popl    %ecx
    popl    %ebx
    leave
label4:
    //cli
//label:
    //jmp     label

    ret
