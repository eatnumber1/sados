#define	__ASM__20093__

#include "ide.h"

.data

//str:    .ascii "$PCI"


.text

.globl _callbios

_callbios:
//    enter   $0, $0
    pushl   %ebx
    pushl   %ecx
    pushl   %edx
//    pushl   %cs

    movl    $0x49435024, %eax // $PCI
    movl    $0, %ebx
    movl    4(%ebp), %edx
//    cli
    call    *(%edx)

    popl    %edx
    popl    %ecx
    popl    %ebx
    leave
    ret
