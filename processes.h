/*
** SCCS ID:	@(#)processes.h	1.1	04/03/10
**
** File:	processes.h
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	Process-related definitions
*/

#ifndef _PROCESSES_H
#define _PROCESSES_H

#include "headers.h"

/*
** General (C and/or assembly) definitions
*/

// process priority levels

#define	PRIO_MAX	0
#define	PRIO_HIGH	500
#define	PRIO_STD	1000
#define	PRIO_LOW	5000
#define	PRIO_MIN	((uint_t) 0x7fffffff)

// quantum values

#define	QUANTUM_SHORT	1
#define	QUANTUM_MEDIUM	2
#define	QUANTUM_LONG	3

#define	QUANTUM_DEFAULT	2

// process ID values

#define	PID_GENERIC	0
#define	PID_INIT	1
#define	PID_IDLE	2
#define	PID_FIRST_USER	3

#ifndef __ASM__20093__

#include "queues.h"
#include "stacks.h"

/*
** Start of C-only definitions
*/

/*
** Types
*/

// process states

typedef enum state {
	FREE, NEW, READY, RUNNING, BLOCKED, SLEEPING, ZOMBIE
} state_t;

// process context structure
//
// NOTE:  the order of data members here depends on the
// register save code in isr_stubs.S!!!!

typedef struct context {
	uint32_t ss;
	uint32_t gs;
	uint32_t fs;
	uint32_t es;
	uint32_t ds;
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	uint32_t vector;
	uint32_t code;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
} context_t;

// process control block

typedef struct pcb {
	context_t *context;	// ptr to process context (must be first)
	state_t state;		// current process state
	uint32_t pid;		// who we are
	uint32_t ppid;		// who created us
	uint32_t status;	// exit status of this process
	uint32_t priority;	// current process priority
	uint32_t children;	// number of non-collected children
	stack_t *stack;		// ptr to full stack
} pcb_t;

// process status info structure

typedef struct wait {
	uint_t pid;
	int status;
} wait_t;


/*
** Globals
*/

extern pcb_t _pcbs[];		// all PCBs in the system
extern pcb_t *_current;		// current process
extern uint32_t _next_pid;	// next available PID

extern queue_t _ready;			// ready-to-run processes
extern queue_t _sleeping;		// sleeping processes
extern queue_t _reading;		// processes blocked for input
extern queue_t _zombies;		// undead processes
extern queue_t _wait_generic;		// processes waiting for any child
extern queue_t _wait_specific;		// processes waiting for one child

/*
** Prototypes
*/

/*
** _pcb_alloc()
**
** allocate a PCB structure
**
** returns a pointer to the PCB, or NULL on failure
*/

pcb_t *_pcb_alloc( void );

/*
** _pcb_free(pcb)
**
** deallocate a PCB structure
**
** returns the status from the _q_insert() call
*/

int _pcb_free( pcb_t *pcb );

/*
** _pcb_find( pid )
**
** locate the non-FREE PCB with this pid
**
** returns a pointer to the PCB, or NULL
*/

pcb_t *_pcb_find( uint32_t pid );

/*
** _pcb_init()
**
** initializes all process-related data structures
*/

void _pcb_init( void );

#endif

#endif
