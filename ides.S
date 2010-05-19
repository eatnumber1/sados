#define	__ASM__20093__

#include "ide.h"

.text

.globl _callbios

_callbios:
    enter   $0, $0
    pushl   %ebx
    pushl   %ecx
    pushl   %edx

    movl    $0x49435024, %eax // $PCI
    movl    $0, %ebx
    movl    8(%ebp), %edx
//    cli
    pushl   %cs
    call    *(%edx)

    popl    %edx
    popl    %ecx
    popl    %ebx
    leave
    ret
