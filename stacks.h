/*
** SCCS ID:	@(#)stacks.h	1.1	04/03/10
**
** File:	stacks.h
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	Stack-related definitions
*/

#ifndef _STACKS_H
#define _STACKS_H

#include "headers.h"

/*
** General (C and/or assembly) definitions
*/

// need one stack per PCB

#define	N_STACKS	N_PCBS

#ifndef __ASM__20093__

/*
** Start of C-only definitions
*/

/*
** Types
*/

// stack data type

typedef uint32_t stack_t[STACK_SIZE];

/*
** Globals
*/

extern stack_t _stacks[];	// all user stacks
extern stack_t _system_stack;	// OS stack
extern uint32_t *_system_esp;	// stack pointer for OS

/*
** Prototypes
*/

/*
** _stack_alloc()
**
** allocate a stack structure
**
** returns a pointer to the stack, or NULL on failure
*/

stack_t *_stack_alloc( void );

/*
** _stack_free(stack)
**
** deallocate a stack structure
**
** returns the status from the _q_insert() call
*/

int _stack_free( stack_t *stack );

/*
** _stack_init()
**
** initializes all process-related data structures
*/

void _stack_init( void );

#endif

#endif
