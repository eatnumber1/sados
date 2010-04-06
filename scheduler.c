/*
** SCCS ID:	@(#)scheduler.c	1.1	04/03/10
**
** File:	scheduler.c
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	Scheduler module
*/

#define	__KERNEL__20093__

#include "headers.h"

#include "scheduler.h"

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/


/*
** _schedule( pcb, priority )
**
** put PCB into the ready queue with the specified priority
**
** returns _q_insert() status
*/

int _schedule( pcb_t *pcb, uint_t priority ) {
	
	if( pcb == NULL ) {
		return( STAT_BAD_PARAM );
	}

	pcb->state = READY;

	return( _q_insert(&_ready,(void *)pcb,priority) );

}

/*
** _dispatch( void )
**
** dispatch the next process
*/

void _dispatch( void ){
	int stat;

	stat = _q_remove( &_ready, (void **) &_current, 0 );

	if( stat != STAT_SUCCESS ) {
		_kpanic( "_dispatch, queue remove bad status" );
	}

	_current->state = RUNNING;

}
