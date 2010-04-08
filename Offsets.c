/*
** SCCS ID:	@(#)Offsets.c	1.1	04/03/10
**
** File:		Offsets.c
**
** Author:		Warren R. Carithers
**
** Description:		Print byte offsets for fields in various structures.
**
** This program exists to simplify life.  If/when fields in a structure are
** changed, this can be modified, recompiled and executed to come up with
** byte offsets for use in accessing structure fields from assembly language.
**
*/

#include <stdio.h>
#undef NULL

#include "headers.h"

#include "processes.h"
#include "queues.h"

queue_t queue;
context_t context;
pcb_t pcb;

int main( void ) {

    printf( "Offsets into Queue (%d bytes):\n", sizeof(queue) );
    printf( "   head:\t%d\n", (char *)&queue.head - (char *)&queue );
    printf( "   tail:\t%d\n", (char *)&queue.tail - (char *)&queue );
    printf( "   length:\t%d\n", (char *)&queue.length - (char *)&queue );
    printf( "   compare:\t%d\n", (char *)&queue.compare - (char *)&queue );
    putchar( '\n' );

    printf( "Offsets into Context (%d bytes):\n", sizeof(context) );
    printf( "   ss:\t\t%d\n", (char *)&context.ss - (char *)&context );
    printf( "   gs:\t\t%d\n", (char *)&context.gs - (char *)&context );
    printf( "   fs:\t\t%d\n", (char *)&context.fs - (char *)&context );
    printf( "   es:\t\t%d\n", (char *)&context.es - (char *)&context );
    printf( "   ds:\t\t%d\n", (char *)&context.ds - (char *)&context );
    printf( "   edi:\t\t%d\n", (char *)&context.edi - (char *)&context );
    printf( "   esi:\t\t%d\n", (char *)&context.esi - (char *)&context );
    printf( "   ebp:\t\t%d\n", (char *)&context.ebp - (char *)&context );
    printf( "   esp:\t\t%d\n", (char *)&context.esp - (char *)&context );
    printf( "   ebx:\t\t%d\n", (char *)&context.ebx - (char *)&context );
    printf( "   edx:\t\t%d\n", (char *)&context.edx - (char *)&context );
    printf( "   ecx:\t\t%d\n", (char *)&context.ecx - (char *)&context );
    printf( "   eax:\t\t%d\n", (char *)&context.eax - (char *)&context );
    printf( "   vector:\t%d\n",(char *)&context.vector - (char *)&context);
    printf( "   code:\t%d\n", (char *)&context.code - (char *)&context );
    printf( "   eip:\t\t%d\n", (char *)&context.eip - (char *)&context );
    printf( "   cs:\t\t%d\n", (char *)&context.cs - (char *)&context );
    printf( "   eflags:\t%d\n",(char *)&context.eflags - (char *)&context);
    putchar( '\n' );

    printf( "Offsets into Pcb (%d bytes):\n", sizeof(pcb) );
    printf( "   context:\t%d\n", (char *)&pcb.context - (char *)&pcb );
    printf( "   state:\t%d\n", (char *)&pcb.state - (char *)&pcb );
    printf( "   pid:\t\t%d\n", (char *)&pcb.pid - (char *)&pcb );
    printf( "   ppid:\t%d\n", (char *)&pcb.ppid - (char *)&pcb );
    printf( "   status:\t%d\n", (char *)&pcb.status - (char *)&pcb );
    printf( "   prio:\t%d\n", (char *)&pcb.priority - (char *)&pcb );
    printf( "   children:\t%d\n", (char *)&pcb.children - (char *)&pcb );
    printf( "   stack:\t%d\n", (char *)&pcb.stack - (char *)&pcb );

    return( 0 );

}
