/*
** SCCS ID:	@(#)processes.c	1.1	04/03/10
**
** File:	processes.c
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	Process-related routines
*/

#define	__KERNEL__20093__

#include "headers.h"

#include "processes.h"

#include "queues.h"

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

static queue_t _free_pcbs;

/*
** PUBLIC GLOBAL VARIABLES
*/

pcb_t _pcbs[ N_PCBS ];		// all PCBs in the system
pcb_t *_current;		// current process
uint32_t _next_pid;		// next available PID

queue_t _ready;			// ready-to-run processes
queue_t _sleeping;		// sleeping processes
queue_t _reading;		// processes blocked for input
queue_t _zombies;		// undead processes
queue_t _wait_generic;		// processes waiting for any child
queue_t _wait_specific;		// processes waiting for one child

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

/*
** _pcb_alloc()
**
** allocate a PCB structure
**
** returns a pointer to the PCB, or NULL on failure
*/

pcb_t *_pcb_alloc( void ) {
	pcb_t *pcb;
	int stat;

	// remove the first entry from the queue of free PCBs

	stat = _q_remove( &_free_pcbs, (void *) &pcb, 0 );

	// verify that the removed succeeded

	if( stat == STAT_EMPTY_QUEUE ) {
		return( NULL );
	} else if( stat != STAT_SUCCESS ) {
		_kpanic( "_pcb_alloc, deque failed" );
	}

	if( pcb == NULL ) {
		_kpanic( "_pcb_alloc, NULL pcb from free queue" );
	}

	// we got something, mark it as non-FREE

	pcb->status = NEW;

	return( pcb );

} /* _pcb_alloc() */


/*
** _pcb_free(pcb)
**
** deallocate a PCB structure
**
** returns the status from the _q_insert() call
*/

int _pcb_free( pcb_t *pcb ) {

	if( pcb != NULL ) {

		pcb->status = FREE;
		return( _q_insert( &_free_pcbs, (void *) pcb, 0 ) );
	
	}

	return( STAT_BAD_PARAM );

} /* _pcb_dealloc() */


/*
** _pcb_find( pid )
**
** locate the non-FREE PCB with this pid
**
** returns a pointer to the PCB, or NULL
*/

pcb_t *_pcb_find( uint32_t pid ) {
	int i;

	for( i = 0; i < N_PCBS; ++i ) {
		if( _pcbs[i].status != FREE && _pcbs[i].pid == pid ) {
			return( &_pcbs[i] );
		}
	}

	return( NULL );
}


/*
** _pcb_init()
**
** initializes all process-related data structures
*/

void _pcb_init( void ) {
	int i, stat;

	// empty out all the PCBs

	_memclr( (void *) _pcbs, sizeof(_pcbs) );

	// initialize the process queues
	//
	// first, the ordered queues

	_q_reset( &_ready, ORDER_UINT_ASCENDING );
	_q_reset( &_sleeping, ORDER_UINT_ASCENDING );
	_q_reset( &_wait_generic, ORDER_UINT_ASCENDING );
	_q_reset( &_wait_specific, ORDER_UINT_ASCENDING );

	// next, the FIFO queues

	_q_reset( &_reading, ORDER_NONE );
	_q_reset( &_zombies, ORDER_NONE );
	_q_reset( &_free_pcbs, ORDER_NONE );


	// iterate through the pcb array, deallocating
	// each one to put it into the free queue
	//
	// NOTE:  THIS ASSUMES the qnodes have been reset as well

	for( i = 0; i < N_PCBS; ++i ) {
		stat =_pcb_free( &_pcbs[i] );
		if( stat != STAT_SUCCESS ) {
			break;
		}
	}

	// check the last insertion to be sure it worked

	if( stat != STAT_SUCCESS ) {
		_kprt_status( "*** pcb init, last insert status %s\n", stat);
		_kpanic( "_init_pcbs, _pcb_free() failed" );
	}

	// no current process

	_current = 0;

	// reset the PID counter
	//
	// the first process to be started should be the idle routine

	_next_pid = PID_IDLE;

	// report that we have finished

	c_puts( " processes" );

} /* _pcb_init() */
