/*
** SCCS ID:	@(#)stacks.c	1.1	04/03/10
**
** File:	stacks.c
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	Stack-related routines
*/

#define	__KERNEL__20093__

#include "headers.h"

#include "stacks.h"
#include "queues.h"

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

static queue_t _free_stacks;

/*
** PUBLIC GLOBAL VARIABLES
*/

stack_t _stacks[ N_STACKS ];	// all user stacks
stack_t _system_stack;		// OS stack
uint32_t *_system_esp;		// stack pointer for OS

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

/*
** _stack_alloc()
**
** allocate a stack structure
**
** returns a pointer to the stack, or NULL on failure
*/

stack_t *_stack_alloc( void ) {
	stack_t *stack;
	int stat;

	// remove the first entry from the queue of free stacks

	stat = _q_remove( &_free_stacks, (void *) &stack, 0 );

	// verify that the removed succeeded

	if( stat == STAT_EMPTY_QUEUE ) {
		return( NULL );
	} else if( stat != STAT_SUCCESS ) {
		_kpanic( "_stack_alloc, deque failed" );
	}

	if( stack == NULL ) {
		_kpanic( "_stack_alloc, NULL stack from free queue" );
	}

	return( stack );

} /* _stack_alloc() */


/*
** _stack_free(stack)
**
** deallocate a stack structure
**
** returns the status from the _q_insert() call
*/

int _stack_free( stack_t *stack ) {

	if( stack != NULL ) {

		return( _q_insert( &_free_stacks, (void *) stack, 0 ) );
	
	}

	return( STAT_BAD_PARAM );

} /* _stack_dealloc() */


/*
** _stack_init()
**
** initializes all stack-related data structures
*/

void _stack_init( void ) {
	int i, stat;

	// empty out all the stacks

	_memclr( (void *) _stacks, sizeof(_stacks) );

	// initialize the queue of available stacks

	_q_reset( &_free_stacks, ORDER_NONE );

	// iterate through the stack array, deallocating
	// each one to put it into the free queue
	//
	// NOTE:  THIS ASSUMES the qnodes have been reset as well

	for( i = 0; i < N_STACKS; ++i ) {
		stat =_stack_free( &_stacks[i] );
		if( stat != STAT_SUCCESS ) {
			break;
		}
	}

	// check the last insertion to be sure it worked

	if( stat != STAT_SUCCESS ) {
		_kprt_status( "*** stack init, last insert status %s\n", stat);
		_kpanic( "_init_stacks, _stack_free() failed" );
	}

	// report that we have finished

	c_puts( " stacks" );

} /* _stack_init() */
