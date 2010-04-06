/*
** SCCS ID:	@(#)queues.h	1.1	04/03/10
**
** File:	queues.h
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	Queue module public definitions
*/

#ifndef _QUEUES_H
#define _QUEUES_H

#include "headers.h"

/*
** General (C and/or assembly) definitions
*/

// queue ordering options

#define	ORDER_NONE		0
#define	ORDER_UINT_ASCENDING	1

#ifndef __ASM__20093__

/*
** Start of C-only definitions
*/

// pseudo-function:  is a queue empty?

#define	_q_empty(q)	((q).length < 1)

/*
** Types
*/

// ordering key

typedef uint32_t key_t;

// queue header

typedef struct queue {
	void *head;		// first node in the queue
	void *tail;		// last node in the queue
	uint_t length;		// number of elements in the queue
	int (*compare) ( void *first, void *second );	// ordering routine
} queue_t;

/*
** Globals
*/

/*
** Prototypes
*/

/*
** _q_insert( queue, data, key )
**
** add 'data' to 'queue', ordered by 'key'
**
** returns status of the insertion attempt
*/

int _q_insert( queue_t *queue, void *data, key_t key );

/*
** _q_remove( queue, data, key )
**
** remove the first element from 'queue'; place it into
** 'data', and the key value used to position it into
** 'key' (if 'key' is non-NULL)
**
** returns the status of the removal attempt
*/

int _q_remove( queue_t *queue, void **data, key_t *key );

/*
** _q_remove_by_key( queue, data, key )
**
** remove the first element from 'queue' which has ordering
** value 'key'; place it into 'data'.
**
** returns the status of the removal attempt
*/

int _q_remove_by_key( queue_t *queue, void **data, key_t key );

/*
** _q_remove_if_le( queue, data, key )
**
** remove the first element from 'queue' if its key is
** less than or equal to 'key'; place it into 'data'
**
** returns the status of the removal attempt
*/

int _q_remove_if_le( queue_t *queue, void **data, key_t key );

/*
** _q_reset( queue, ordering )
**
** reinitialize the specified queue using the indicated
** ordering method
*/

void _q_reset( queue_t *queue, int ordering );

/*
** _queue_init()
**
** initialize the queue module
*/

void _queue_init( void );

/*
** _queue_dump( queue )
**
** dump the contents of the specified queue to the console
*/

void _queue_dump( char *which, queue_t *queue );

#endif

#endif
