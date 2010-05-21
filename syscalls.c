/*
** SCCS ID:	@(#)syscalls.c	1.1	04/03/10
**
** File:	syscalls.c
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	system call handlers
*/

#define	__KERNEL__20093__

#include "headers.h"

#include "syscalls.h"

#include "bootstrap.h"
#include "clock.h"
#include "processes.h"
#include "queues.h"
#include "scheduler.h"
#include "sio.h"
#include "startup.h"
#include "relfs.h"
#include "disk.h"

// also need the exit() prototype
#include "ulib.h"

// also need the user process prototypes
#include "user.h"

#include <x86arch.h>

/*
** PRIVATE DEFINITIONS
*/

// argument N from the supplied process context

#define	ARG(n,p)	(*( ( (uint32_t *)((p)+1) ) + (n)))

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

// system call jump table

static void (*_syscalls[N_SYSCALLS])( context_t * );


/*
** PRIVATE FUNCTIONS
*/

/*
** Second-level syscall handlers
**
** All have this prototype:
**
**	static void _sys_NAME( context_t *context );
*/

static void _sys_relfs_mkfs( context_t *context ) {
	disk_t *disk = (disk_t*) ARG(1,context);
	disk_size_t inode_table_size = ARG(2,context);
	context->eax = (long) _relfs_mkfs(*disk, inode_table_size);
}

static void _sys_relfs_open( context_t *context ) {
	disk_t *disk = (disk_t*) ARG(1,context);
	context->eax = (long) _relfs_open(*disk);
}

static void _sys_relfs_alloc( context_t *context ) {
	relfs_t *fs = (relfs_t*) ARG(1,context);
	char *name = (char*) ARG(2,context);
	disk_size_t size = ARG(3,context);
	context->eax = (long) _relfs_alloc(fs, name, size);
}

static void _sys_relfs_close( context_t *context ) {
	_relfs_close((relfs_t*) ARG(1, context));
}

static void _sys_relfs_free( context_t *context ) {
	_relfs_free((disk_node_t*) ARG(1, context));
}

static void _sys_relfs_write( context_t *context ) {
	relfs_t *fs = (relfs_t*) ARG(1,context);
	disk_node_t *node = (disk_node_t*) ARG(2,context);
	char *buf = (char*) ARG(3,context);
	disk_size_t len = ARG(4,context);
	_relfs_write(fs, node, buf, len);
}

static void _sys_relfs_read( context_t *context ) {
	relfs_t *fs = (relfs_t*) ARG(1,context);
	disk_node_t *node = (disk_node_t*) ARG(2,context);
	char *buf = (char*) ARG(3,context);
	disk_size_t len = ARG(4,context);
	_relfs_read(fs, node, buf, len);
}

static void _sys_relfs_dump( context_t *context ) {
	_relfs_dump((relfs_t*) ARG(1,context));
}

static void _sys_relfs_unlink( context_t *context ) {
	relfs_t *fs = (relfs_t*) ARG(1,context);
	char *name = (char*) ARG(2,context);
	_relfs_unlink(fs, name);
}

static void _sys_relfs_retrieve( context_t *context ) {
	relfs_t *fs = (relfs_t*) ARG(1,context);
	char *name = (char*) ARG(2,context);
	context->eax = (long) _relfs_retrieve(fs, name);
}

/*
** _sys_read - read one character from the SIO
**
** usage:	ch = read()
**
** returns:
**	the character
*/

static void _sys_read( context_t *context ) {
	int ch;

	// try to get the next character

	ch = _sio_read();

	// if there was a character, return it to the process;
	// otherwise, block the process until one comes in

	if( ch >= 0 ) {

		context->eax = ch;
	
	} else {

		// no character; put this process on the
		// serial i/o input queue

		_current->status = BLOCKED;

		ch = _q_insert( &_reading, (void *) _current, _current->pid );
		if( ch != STAT_SUCCESS ) {
			_kprt_status( "sys read: enque status %s\n", ch );
			_kpanic( "sys read: can't block current process" );
		}

		// select a new current process

		_dispatch();
	
	}

}

/*
** _sys_write - write a character to the SIO
**
** usage:	write(ch)
*/

static void _sys_write( context_t *context ) {
	int ch = ARG(1,context);

	// this is almost insanely simple, but it does separate
	// the low-level device access fromm the higher-level
	// syscall implementation

	_sio_write( ch );

}


/*
** _sys_getpid - get the PID of the current process
**
** usage:       n = getpid()
**
** returns:
**      the PID of the current process
*/

void _sys_getpid( context_t *context ) {

	context->eax = _current->pid;

}


/*
** _sys_getticks - get the current system time, in ticks
**
** usage:       n = time()
**
** returns:
**      the current time, in ticks
*/

void _sys_getticks( context_t *context ) {

	context->eax = _ticks;

}


/*
** _sys_getppid - get the parent PID of the specified process
**
** usage:       n = getppid()
**
** returns:
**      the PID of the parent of the specified process, or an error status
*/

void _sys_getppid( context_t *context ) {
	uint32_t pid;
	pcb_t *pcb;

	pid = ARG( 1, context );

	pcb = _pcb_find( pid );
	if( pcb == NULL ) {
		context->eax = (uint32_t) STAT_NOT_FOUND;
	} else {
		context->eax = pcb->ppid;
	}

}

/*
** _sys_getprio - get the priority of the specified process
**
** usage:       n = getprio()
**
** returns:
**      the priority of the specified process, or an error status
*/

void _sys_getprio( context_t *context ) {
	uint32_t pid;
	pcb_t *pcb;

	pid = ARG( 1, context );

	pcb = _pcb_find( pid );
	if( pcb == NULL ) {
		context->eax = (uint32_t) STAT_NOT_FOUND;
	} else {
		context->eax = pcb->priority;
	}

}


/*
** _sys_setprio - change priority of the specified process
**
** usage:       prio = setprio( pid, prio );
**
** returns:
**      old priority of the specified process, or an error status
*/

void _sys_setprio( context_t *context ) {
	uint32_t pid, prio;
	pcb_t *pcb;

	pid  = ARG( 1, context );
	prio = ARG( 2, context );

	pcb = _pcb_find( pid );
	if( pcb == NULL ) {
		context->eax = (uint32_t) STAT_NOT_FOUND;
	} else {
		context->eax = pcb->priority;
		pcb->priority = prio;
	}

}

/*
** _sys_exit - terminate a process
**
** usage:       exit( status )
**
** doesn't return
*/

void _sys_exit( context_t *context ) {
	int i, stat;
	pcb_t *pcb;
	wait_t *info;

	/*
	** Mark the process as dead, and save its
	** exit status for later collection by
	** its parent (or init)
	*/

	_current->state  = ZOMBIE;
	_current->status = ARG(1,context);

	/*
	** We need to reparent all the children of this
	** process to the initial process.  First, find
	** init.
	*/

	pcb = _pcb_find( PID_INIT );
	if( pcb == NULL ) {
		_kpanic( "_sys_exit, cannot find init!" );
	}

	/*
	** Increment init's child count
	*/

	pcb->children += _current->children;

	/*
	** Reparent all of the zombie children of this process.
	*/

	stat = _q_remove_by_key( &_zombies, (void **)&pcb, _current->pid );
	while( stat == STAT_SUCCESS ) {

		// got one - decrement the count of remaining children
		_current->children--;

		// reparent this process
		pcb->ppid = PID_INIT;

		// put it back with the correct parent ID
		stat = _q_insert( &_zombies, (void *)pcb, PID_INIT );
		if( stat != STAT_SUCCESS ) {
			_kprt_status( "_sys_exit, zombie reque %s\n", stat );
			_kpanic( "_sys_exit, zombie reque failed" );
		}

		// get the next one
		stat = _q_remove_by_key( &_zombies, (void **)&pcb,
					 _current->pid );

	}

	/*
	** Verify that we left the loop because there were
	** no more zombie children
	*/

	if( stat != STAT_NOT_FOUND && stat != STAT_EMPTY_QUEUE ) {
		_kprt_status( "_sys_exit, zombie deque status %s\n", stat );
		_kpanic( "_sys_exit, zombie deque failed" );
	}

	/*
	** Now, iterate through all the PCBs to find any other
	** children who aren't zombies, and reparent them.
	*/

	for( i = 0; _current->children > 0 && i < N_PCBS; ++i ) {

		if( _pcbs[i].state != FREE && _pcbs[i].ppid == _current->pid ) {

			// found one!  decrement the child count
			_current->children--;

			// reparent this child
			_pcbs[i].ppid = PID_INIT;

		}

	}

	/*
	** Verify that we processed all the children
	*/

	if( _current->children > 0 ) {
		_kpanic( "_sys_exit, couldn't find all children" );
	}

	/*
	** Next, we need to notify the parent of this process.
	** Try to find it on either the "wait for specific child"
	** queue or the "wait for any child" queue.
	*/

	stat = _q_remove_by_key( &_wait_specific, (void **)&pcb,
				 _current->pid );
	if( stat != STAT_SUCCESS ) {
		stat = _q_remove_by_key( &_wait_generic, (void **)&pcb,
					 _current->ppid );
	}

	/*
	** If we found the parent, wake it up.
	*/

	if( stat == STAT_SUCCESS ) {

		// get the parent's info structure pointer
		info = (wait_t *) ARG( 1, pcb->context );

		// fix the parent's "active child" count
		pcb->children--;

		// copy the status and PID data to the parent
		info->pid = _current->pid;
		info->status = _current->status;

		// return value from parent's wait() call
		pcb->context->eax = STAT_SUCCESS;

		// put it back on the ready queue
		stat = _schedule( pcb, pcb->priority );
		if( stat != STAT_SUCCESS ) {
			_kprt_status( "_sys_exit, parent sched status %s\n",
					stat );
			_kpanic( "_sys_exit, parent sched failed" );
		}

		// done with this process - free it up
		_stack_free( _current->stack );
		_pcb_free( _current );

	} else {

		/*
		** Didn't find the parent, so it must still be running.
		** Put this process on the zombie queue so that it can
		** be collected later.
		*/

		stat = _q_insert( &_zombies, (void *)_current, (key_t)_current->ppid );
		if( stat != STAT_SUCCESS ) {
			_kprt_status( "_sys_exit, zombie insert status %s\n", stat );
			_kpanic( "_sys_exit, zombie insert failed" );
		}

		// can free up some resources now
		_stack_free( _current->stack );
		_current->stack = NULL;
		_current->context = NULL;

	}

	/*
	** Pick a new current process
	*/

	_dispatch();

}

/*
** _sys_wait - get status return from an exited child
**
** usage:       n = wait( status_structure )
**
** returns:
**	exited child found: PID and exit status in 'status_structure'
**	always:  success/failure status
*/

void _sys_wait( context_t *context ) {
	wait_t *info = (wait_t *) ARG(1,context);
	pcb_t *pcb;
	int stat;

	// if there is no children, can't get its status

	if( _current->children == 0 ) {
		context->eax = STAT_NO_CHILDREN;
		return;
	}

	/*
	** The wait_t structure contains a PID field and a status field.
	** At the time of the call, the PID field contains either the PID
	** of a specific child process, or a "generic" PID.  For the former,
	** the parent blocks until that specific child terminates; for
	** the latter, the parent blocks until any child terminates.
	*/

	if( info->pid == PID_GENERIC ) {

		/*
		** Looking for any terminated child of this parent.
		** Find the first one on the zombie queue (which is ordered
		** by the parent PID).
		*/

		stat = _q_remove_by_key( &_zombies, (void **) &pcb, _current->pid );
		/*
		** If we found one, report its status to the parent
		** without blocking the parent.
		*/

		if( stat == STAT_SUCCESS ) {

			// found one
			context->eax = STAT_SUCCESS;

			// return its information
			info->pid    = pcb->pid;
			info->status = pcb->status;

			// finish releasing the PCB of the child
			_pcb_free( pcb );

			// parent now has one fewer children
			_current->children--;

			// all done!
			return;
		}

		/*
		** We didn't find a dead child process.  Block the parent,
		** putting it on the "waiting for any child" queue.
		*/

		_current->state = BLOCKED;
		stat = _q_insert( &_wait_generic, (void **) _current,
				  _current->pid );
		if( stat != STAT_SUCCESS ) {
			_kprt_status( "_sys_wait, queue generic status %s\n",
					stat );
			_kpanic( "_sys_wait, queue generic failed" );
		}

		// we'll dispatch a new process on exit from this 'if' stmt

	} else {

		/*
		** Looking for a specific child process.  First, try to
		** find it in the system.
		*/

		pcb = _pcb_find( info->pid );
		if( pcb == NULL ) {
			context->eax = STAT_NO_SUCH_PID;
			return;
		}

		/*
		** Found it.  Now, verify that it actually came from
		** this parent process.
		*/

		if( pcb->ppid != _current->pid ) {
			context->eax = STAT_NOT_YOURS;
			return;
		}

		/*
		** OK so far.  If the child has already exited, must
		** pull it from the zombie queue.
		*/

		if( pcb->state == ZOMBIE ) {
			uint_t first = PID_GENERIC;

			/*
			** This is painful.  The zombie queue is ordered
			** by the parent's PID.  There may be more than
			** one of this parent's children on the queue;
			** however, we can only remove from the queue by
			** parent PID, so we need to iterate through them
			** all until we find the correct child.
			**
			** Start by pulling the first child from the 
			** queue.
			*/

			stat = _q_remove_by_key( &_zombies, (void **)&pcb,
						 _current->pid );

			/*
			** Iterate as long as the previous removal
			** succeeded.
			*/

			while( stat == STAT_SUCCESS ) {

				// see if this is the child we want
				if( pcb->pid == info->pid ) {

					// yes!  report success
					context->eax = STAT_SUCCESS;

					// return the status from the child
					info->status = pcb->status;

					// finish releasing the child
					_pcb_free( pcb );

					// one fewer children
					_current->children--;

					// all done!
					return;
				}

				/*
				** Not the correct child; put it back on
				** the zombie queue.
				*/

				stat = _q_insert(&_zombies,(void *)pcb,_current->pid);
				if( stat != STAT_SUCCESS ) {
					_kprt_status( "_sys_wait, queue specific status %s\n",
							stat );
					_kpanic( "_sys_wait, queue specific failed" );
				}

				/*
				** Here's where we check to see if we have
				** iterated through all the terminated
				** children of this parent.  The first time
				** through, 'first' contains the generic
				** PID; we replace that with the PID of
				** the first child we pulled from the queue,
				** and we put the child back on the queue,
				** relying on the insertion going to the
				** end of the set of PIDs who have this
				** parent.
				*/

				if( first == PID_GENERIC ) {

					first = pcb->pid;

				} else if( first == pcb->pid ) {

					/* If that child gets pulled from
					** the queue again, we have
					** iterated through the whole set
					** of children without finding the
					** one we need, which is a major
					** internal error.
					*/

					break;

				}

				// put the child back on the queue

				stat = _q_remove_by_key( &_zombies, (void **)&pcb,
						 	_current->pid );
			}

			// didn't find the zombie on the zombie queue!

			_kpanic( "_sys_wait, zombie not on queue!" );

		}

		/*
		** The child hasn't terminated yet.  Block the parent,
		** putting it on the "waiting for a specific child" queue.
		*/

		_current->state = BLOCKED;
		stat = _q_insert( &_wait_specific, (void **) _current,
				  info->pid );
		if( stat != STAT_SUCCESS ) {
			_kprt_status( "_sys_wait, queue specific status %s\n",
					stat );
			_kpanic( "_sys_wait, queue specific failed" );
		}

	}

	/*
	** If we get to this point, we know that the current process
	** was blocked, so we need to select a new one.
	*/

	_dispatch();

}

/*
** sleep - put the current process to sleep for the specified number of ms
**
** usage:       sleep( ms );
*/

void _sys_sleep( context_t *context ) {
	uint_t ms = ARG( 1, context );
	uint32_t ticks;
	int stat;

	// determine wakeup time

	ticks = MS_TO_TICKS( ms );
	ticks += _ticks;

	// add to the sleep queue

	stat = _q_insert( &_sleeping, (void *)_current, ticks );
	if( stat != STAT_SUCCESS ) {
		_kprt_status( "_sys_sleep, sleep insert %s\n", stat );
		_kpanic( "_sys_sleep, sleeper insert failed" );
	}

	// mark the process as sleeping

	_current->state = SLEEPING;

	// pick a new current process

	_dispatch();

}

/*
** fork - create a new process
**
** usage:       pid = fork( priority );
**
** returns:
**      -1 if the creation failed
**      0 in the child
**      else, the PID of the new child
*/
void _sys_fork( context_t *context ) {
	pcb_t *pcb;
	stack_t *stack;
	int offset;
	int prio = ARG( 1, context );

	/*
	** Start by allocating the necessary process data structures
	*/

	pcb = _pcb_alloc();
	if( pcb == NULL ) {
		context->eax = STAT_FAILURE;
		return;
	}

	stack = _stack_alloc();
	if( stack == NULL ) {
		_pcb_free( pcb );
		context->eax = STAT_FAILURE;
		return;
	}

	/*
	** Replicate the parent's information
	*/

	_memcpy( pcb, _current, sizeof(pcb_t) );
	_memcpy( stack, _current->stack, sizeof(stack_t) );

	/*
	** Update the fields which are different for the child
	*/

	pcb->pid   = _next_pid++;
	pcb->ppid  = _current->pid;
	pcb->stack = stack;
	pcb->children = 0;
	pcb->priority = prio;

	/*
	** We duplicated the parent's stack contents; this means that
	** the child's context pointer, ESP, and EBP are still pointing
	** into the parent's stack.  We must correct this.
	** 
	** Determine the distance between the two stacks in memory.
	** This is the adjustment value we must add to the three
	** pointers to correct them.
	*/

	offset = (void *)(pcb->stack) - (void *)(_current->stack);

	/*
	** Next, update the three pointers
	*/

	pcb->context = (context_t *) ( (uint32_t)(pcb->context) + offset );
	pcb->context->esp = ( (uint32_t)(pcb->context->esp) + offset );
	pcb->context->ebp = ( (uint32_t)(pcb->context->ebp) + offset );

	/*
	** IMPORTANT NOTE
	** 
	** We have to change EBP because that's how the compiled code for
	** the user process accesses its local variables.  If we didn't
	** change this, as soon as the child was dispatched, it would
	** start to stomp on the local variables in the parent's stack.
	** 
	** The child's ESP and EBP will be set by the context restore when
	** we dispatch it.  However, we don't fix the EBP chain, so if the
	** child doesn't immediately exec() but instead returns, it will
	** switch back over to the parent's stack.
	** 
	** None of this would be an issue if we were doing "real" virtual
	** memory, as we would be talking about virtual addresses here rather
	** than physical addresses, and all processes would share the same
	** virtual address space layout.
	*/

	/*
	** Set up return values for the parent and child
	*/
	
	pcb->context->eax = 0;
	_current->context->eax = pcb->pid;

	/*
	** Parent now has one more child
	*/

	_current->children++;

	/*
	** Philosophical issue:  should the child run immediately, or
	** should the parent continue?
	** 
	** We opt for the latter; we schedule the child first, then
	** return into the parent.
	*/

	_schedule( pcb, pcb->priority / 2 );

}

/*
** exec - replace a process with a different program
**
** usage:       n = exec( address, n, p1, p2, ..., pn );
**
** returns:
**      nothing if the exec succeeds
**      else, reason for failure
*/

void _sys_exec( context_t *context ) {
	stack_t *stack;
	uint32_t *ptr1, *ptr2;
	uint32_t addr = ARG( 1, context );
	uint32_t n = ARG( 2, context );

	/*
	** The simplest way to replace the stack structure is to just
	** duplicate what information we need into a new stack.
	*/

	stack = _stack_alloc();
	if( stack == NULL ) {
		context->eax = STAT_FAILURE;
		return;
	}

	/*
	** Eliminate any cruft left by a previous user
	*/

	_memclr( (void *) stack, sizeof(stack_t) );

	/*
	** User functions will have prototypes like this:
	**
	**	void fcn( int n, ... );
	**
	** We want to copy the value 'n' and the 'n' arguments from
	** the old stack to the bottom of the new stack.  First, create
	** pointers to the 'n' values in the two stacks.
	*/
	
	ptr1 = ((uint32_t *) (context+1)) + 2;
	ptr2 = ((uint32_t *) (stack + 1)) - (n + 2);

	/*
	** Next, copy the 'n' and the 'n' args.
	*/

	_memcpy( (void *)ptr2, (void *)ptr1, (n+1) * sizeof(uint32_t) );

	/*
	** At this point, we're done with the old stack; release
	** it, and put the new one in its place.
	*/

	_stack_free( _current->stack );
	_current->stack = stack;

	/*
	** Next, we need to push a dummy return address.  If the
	** user main() routine returns without exiting, it will
	** "return" into a special user library routine that will
	** call exit().
	**
	** Move back to where the return address needs to go, and
	** assign the dummy function address.
	*/

	ptr2--;
	*ptr2 = (uint32_t ) call_exit;

	/*
	** Finally, set up the context for the new process.
	** First, assign a context pointer.
	*/

	context = _current->context = ((context_t *) ptr2) - 1;

	// Initialize the segment register fields

	context->ss = GDT_STACK;
	context->gs = GDT_DATA;
	context->fs = GDT_DATA;
	context->es = GDT_DATA;
	context->ds = GDT_DATA;
	context->cs = GDT_CODE;

	// Set up the entry point

	context->eip = addr;

	// Set up the initial EFLAGS

	context->eflags = DEFAULT_EFLAGS;

	// Finally, set up the process' ESP

	context->esp = (uint32_t) context;

	// let it keep running

}


/*
** PUBLIC FUNCTIONS
*/

/*
** _isr_syscall()
**
** common handler for all system calls; selects the correct
** second-level routine to invoke based on contents of EAX
**
** note that any value being returned to the user process by
** the system call should be put into the process' EAX save
** area in the process context
*/

void _isr_syscall( int vector, int code ) {
	int num;

	if( _current == NULL ) {
		_kpanic( "_isr_syscall, NULL _current" );
	}

	if( _current->context == NULL ) {
		_kpanic( "_isr_syscall, NULL _current context" );
	}


	num = _current->context->eax;

	if( num >= N_SYSCALLS ) {
		c_printf( "syscall: code %d, proc %d, killed\n",
			  num, _current->pid );
		num = SYS_exit;
	}

	(*_syscalls[num])( _current->context );

	__outb( PIC_MASTER_CMD_PORT, PIC_EOI );

}


/*
** _syscall_init()
**
** initializes all syscall-related data structures
*/

void _syscall_init( void ) {

	// set up the syscall jump table (this ensures that we
	// get the pointers in the correct slots in the table)

	_syscalls[ SYS_exit     ]  =  _sys_exit;
	_syscalls[ SYS_fork     ]  =  _sys_fork;
	_syscalls[ SYS_exec     ]  =  _sys_exec;
	_syscalls[ SYS_wait     ]  =  _sys_wait;
	_syscalls[ SYS_sleep    ]  =  _sys_sleep;
	_syscalls[ SYS_read     ]  =  _sys_read;
	_syscalls[ SYS_write    ]  =  _sys_write;
	_syscalls[ SYS_getpid   ]  =  _sys_getpid;
	_syscalls[ SYS_getppid  ]  =  _sys_getppid;
	_syscalls[ SYS_getprio  ]  =  _sys_getprio;
	_syscalls[ SYS_setprio  ]  =  _sys_setprio;
	_syscalls[ SYS_getticks ]  =  _sys_getticks;
	_syscalls[ SYS_relfs_mkfs  ]  =  _sys_relfs_mkfs;
	_syscalls[ SYS_relfs_open  ]  =  _sys_relfs_open;
	_syscalls[ SYS_relfs_alloc ]  =  _sys_relfs_alloc;
	_syscalls[ SYS_relfs_close ]  =  _sys_relfs_close;
	_syscalls[ SYS_relfs_free  ]  =  _sys_relfs_free;
	_syscalls[ SYS_relfs_write ]  =  _sys_relfs_write;
	_syscalls[ SYS_relfs_read  ]  =  _sys_relfs_read;
	_syscalls[ SYS_relfs_dump  ]  =  _sys_relfs_dump;
	_syscalls[ SYS_relfs_unlink]  =  _sys_relfs_unlink;
	_syscalls[ SYS_relfs_retrieve]  =  _sys_relfs_retrieve;

	// report that we've completed this module

	c_puts( " syscalls" );

}
