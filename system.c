/*
** SCCS ID:	@(#)system.c	1.1	04/03/10
**
** File:	system.c
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	Miscellaneous system routines
*/

#define	__KERNEL__20093__

#include "headers.h"

#include "ulib.h"

#include "system.h"
#include "clock.h"
#include "queues.h"
#include "scheduler.h"
#include "sio.h"
#include "stacks.h"
#include "syscalls.h"
#include "user.h"

#include "bootstrap.h"
#include "startup.h"
#include "support.h"
#include "kalloc.h"

/*
** PRIVATE DATA TYPES
*/

/*
** PUBLIC FUNCTIONS
*/

/*
** _init:	system initialization routine
**
** Called by startup code immediately before returning into the
** first user-level process.  The name is taken from there.
*/

#include "disk.h"
#include "relfs.h"
#include "string.h"

void _init( void ) {
	pcb_t *pcb;
	uint32_t *ptr;
	context_t *context;

	/*
	** BOILERPLATE CODE - taken from basic framework
	**
	** Initialize interrupt stuff.
	*/

	__init_interrupts();	// IDT and PIC initialization

	/*
	** I/O system.
	*/

	c_io_init();
	c_setscroll( 0, 7, 99, 99 );
	c_puts_at( 0, 6, "================================================================================" );

	c_puts( "Init: " );

	/*
	** 20093-SPECIFIC CODE STARTS HERE
	*/

	/*
	** Initialize various OS modules
	*/

	c_puts( "Starting module init: " );

	_queue_init();		// must be first
	_pcb_init();
	_stack_init();
	_sio_init();
	_clock_init();
	_syscall_init();

	c_puts( "\n" );

	/*
	** Create the initial system ESP
	**
	** This will be the address of the next-to-last
	** longword in the system stack.
	*/

	_system_esp = ((uint32_t *) ( (&_system_stack) + 1)) - 2;

	/*
	** Install the ISRs
	*/

	__install_isr( INT_VEC_TIMER, _isr_clock );
	__install_isr( INT_VEC_SYSCALL, _isr_syscall );
	__install_isr( INT_VEC_SERIAL_PORT_1, _isr_sio );

	/*
	 * Initialize the kernel memory allocator
	 */
	_kalloc_init();

	/*
	disk_t disk;
	disk.diskno = 0;
	disk.partno = 0;
	relfs_t *fs = _relfs_mkfs(disk, (_disk_size(disk) - 1) / 2);
	disk_node_t *node = _relfs_alloc(fs, "A", 1);

	sector_t *sec = _buf_to_sector("Hello", strlen("Hello") + 1);
	_disk_write(disk, node->start, _sectors_in(strlen("Hello") + 1), sec);
	_kfree(sec);

	_relfs_dump(fs);

	_kpanic("Done");
	*/

	/*
	** Create the initial process
	**
	** Code mostly stolen from _sys_fork() and _sys_exec();
	** if either of those routines change, SO MUST THIS!!!
	**
	** First, get a PCB and a stack
	*/

	pcb = _pcb_alloc();
	if( pcb == NULL ) {
		_kpanic( "_init - can't allocate first pcb" );
	}

	pcb->stack = _stack_alloc();
	if( pcb->stack == NULL ) {
		_kpanic( "_init - can't allocate first stack" );
	}

	/*
	** Next, set up various PCB fields
	*/

	pcb->pid  = PID_INIT;
	pcb->ppid = PID_INIT;
	pcb->priority = PRIO_MAX;

	/*
	** account for the fact that init should always
	** have at least one child - the idle routine
	*/

	pcb->children = 1;

	/*
	** Set up the initial process context.
	**
	** First, clear the stack
	*/

	_memclr( (void *) pcb->stack, sizeof(stack_t) );

	/*
	** Set up the initial stack contents.
	**
	** We reserve a longword at the bottom of the stack for
	** some scratch space.  Above this are the "command-line"
	** arguments and argument count (always there); above that
	** goes a dummy "return address" so that if the process ever
	** returns from its main routine it will "return" to a routine
	** that calls exit().  Finally, above that we'll initialize a
	** context for the process to use when dispatched.
	**
	** First, find the location of the word immediately above
	** the scratch space.
	*/
	
	ptr = ((uint32_t *) (pcb->stack + 1)) - 2;

	/*
	** Copy the argument count
	*/

	*ptr = 0;

	/*
	** Next, we need to push a dummy return address.  If the
	** user main() routine returns without exiting, it will
	** "return" into a special user library routine that will
	** call exit().
	**
	** Move back to where the return address needs to go, and
	** assign the dummy function address.
	*/

	--ptr;
	*ptr = (uint32_t) call_exit;

	/*
	** Finally, set up the context for the new process.
	** First, assign a context pointer.
	*/

	context = pcb->context = ((context_t *) ptr) - 1;

	// Initialize the segment register fields

	context->ss = GDT_STACK;
	context->gs = GDT_DATA;
	context->fs = GDT_DATA;
	context->es = GDT_DATA;
	context->ds = GDT_DATA;
	context->cs = GDT_CODE;

	// Set up the entry point

	context->eip = (uint32_t) init_main;

	// Set up the initial EFLAGS

	context->eflags = DEFAULT_EFLAGS;

	// Finally, set up the process' ESP

	context->esp = (uint32_t) context;

	// Make it the "current" process

	_current = pcb;

	/*
	** Starting up the idle routine is now the responsibility
	** of the initial user process.
	*/

	/*
	** Turn on the SIO receiver (the transmitter will be turned
	** on/off as characters are being sent)
	*/

	_sio_enable( SIO_RX );

	/*
	** END OF 20093-SPECIFIC CODE
	**
	** Finally, report that we're all done.
	*/

	c_puts( "System initialization complete.\n" );

}
