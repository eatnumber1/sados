/*
** SCCS ID:	@(#)queues.c	1.1	04/03/10
**
** File:	queues.c
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	Queue module
*/

#define	__KERNEL__20093__

#include "headers.h"

#include "queues.h"
#include "processes.h"

/*
** PRIVATE DEFINITIONS
*/

// number of queue nodes we need to allocate
//
// we need one per PCB, one per stack, and a few extra for safety

#define	N_QNODES	( 2 * N_PCBS + 5 )

/*
** PRIVATE DATA TYPES
*/

// nodes for our generic queue structure

typedef struct qnode {
	void *data;		// object being queued
	struct qnode *next;	// link to next object in queue
	key_t key;		// ordering key
} qnode_t;

/*
** PRIVATE GLOBAL VARIABLES
*/

// queue nodes

static qnode_t *_free_qnodes;
static qnode_t _qnodes[N_QNODES];

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

/*
** _qnode_alloc()
**
** allocate the first available queue node
**
** returns a pointer to the node, or NULL if non are available
*/

static qnode_t *_qnode_alloc( void ) {
	qnode_t *tmp;

	tmp = _free_qnodes;
	if( tmp != NULL ) {
		_free_qnodes = tmp->next;
		tmp->next = NULL;
	}

	return( tmp );

} /* _qnode_alloc() */

/*
** _qnode_free( qnode)
**
** returns a qnode to the free list
*/

static void _qnode_free( qnode_t *tmp ) {

	tmp->next = _free_qnodes;
	_free_qnodes = tmp;

} /* _qnode_free() */


/*
** _compare_uint_ascending( first, second )
**
** compare the keys from the supplied qnodes
**
** returns:
**	-1	first key < second key
**	0	first key == second key
**	1	first key > second key
*/

static int _compare_uint_ascending( void *first, void *second ) {
	qnode_t *one = first;
	qnode_t *two = second;

	if( one->key < two->key )
		return( -1 );
	else if( one->key == two->key )
		return( 0 );
	else
		return( 1 );

} /* _compare_uint_ascending() */

/*
** PUBLIC FUNCTIONS
*/


/*
** _q_insert( queue, data, key )
**
** add 'data' to 'queue', ordered by 'key'
**
** returns status of the insertion attempt
*/

int _q_insert( queue_t *queue, void *data, key_t key ) {
	qnode_t *qnode;
	qnode_t *curr, *prev;

	if( queue == NULL ) {
		_kpanic( "_q_insert, NULL queue" );
	}

	qnode = _qnode_alloc();
	if( qnode == NULL ) {
		return( STAT_NO_QNODE );
	}

	qnode->data = data;
	qnode->key = key;

	if( queue->length == 0 ) {
		queue->head = queue->tail = qnode;
		queue->length++;
		return( STAT_SUCCESS );
	}

	if( queue->compare == NULL ) {
		prev = queue->tail;
		prev->next = qnode;
		queue->tail = qnode;
		queue->length++;
		return( STAT_SUCCESS );
	}

	prev = NULL;
	curr = queue->head;

	while( curr != NULL && queue->compare(curr,qnode) <= 0 ) {
		prev = curr;
		curr = curr->next;
	}

	/*
	** prev  curr  meaning        change
	** ====  ====  =============  =====================================
	**  0     0    can't happen   ?
	**  0     !0   add at front   qnode->curr, head ->qnode
	**  !0    !0   add in middle  qnode->curr, prev->qnode
	**  !0    0    add at end     qnode->curr, prev->qnode, tail->qnode
	*/

	qnode->next = curr;  // always

	if( prev == NULL ) {

		queue->head = qnode;
	
	} else {

		prev->next = qnode;

		if( curr == 0 ) {
			queue->tail = qnode;
		}
	
	}

	queue->length++;

	return( STAT_SUCCESS );

} /* _q_insert() */


/*
** _q_remove( queue, data, key )
**
** remove the first element from 'queue'; place it into
** 'data', and the key value used to position it into
** 'key' (if 'key' is non-NULL)
**
** returns the status of the removal attempt
*/

int _q_remove( queue_t *queue, void **data, key_t *key ) {
	qnode_t *qnode;

	if( queue == NULL ) {
		_kpanic( "_q_remove, NULL queue" );
	}

	if( queue->length < 1 ) {
		return( STAT_EMPTY_QUEUE );
	}

	qnode = queue->head;
	queue->head = qnode->next;
	queue->length--;

	if( queue->length == 0 ) {
		queue->tail = NULL;
	}

	/*
	** _q_remove( queue, 0, 0 )		remove but discard
	** _q_remove( queue, data, 0 )		remove, return data
	** _q_remove( queue, data, key )	remove, return data & key
	*/

	if( data != NULL ) {
		*data = qnode->data;
	}

	if( key != NULL ) {
		*key = qnode->key;
	}

	_qnode_free( qnode );

	return( STAT_SUCCESS );

} /* _q_remove() */


/*
** _q_remove_by_key( queue, data, key )
**
** remove the first element from 'queue' which has ordering
** value 'key'; place it into 'data'.
**
** returns the status of the removal attempt
*/

int _q_remove_by_key( queue_t *queue, void **data, key_t key ) {
	qnode_t *prev, *curr;

	if( queue == NULL ) {
		/* etc */
	}

	if( queue->length < 1 ) {
		return( STAT_EMPTY_QUEUE );
	}

	prev = 0;
	curr = queue->head;

	while( curr != NULL && curr->key != key ) {
		prev = curr;
		curr = curr->next;
	}

	if( curr == NULL ) {
		return( STAT_NOT_FOUND );
	}

	if( prev == NULL ) {	// first node in queue

		queue->head = curr->next;
	
	} else {

		prev->next = curr->next;
		if( prev->next == NULL ) {
			queue->tail = prev;
		}
	
	}

	queue->length--;
	if( queue->length < 1 ) {
		queue->head = queue->tail = NULL;
	}

	if( data != NULL ) {
		*data = curr->data;
	}

	_qnode_free( curr );

	return( STAT_SUCCESS );

} /* _q_remove_by_key() */


/*
** _q_remove_if_le( queue, data, key )
**
** remove the first element from 'queue' if its key is
** less than or equal to 'key'; place it into 'data'
**
** returns the status of the removal attempt
*/

int _q_remove_if_le( queue_t *queue, void **data, key_t key ) {
	qnode_t *tmp;

	if( queue == NULL ) {
		/* etc */
	}

	if( queue->length < 1 ) {
		return( STAT_EMPTY_QUEUE );
	}

	tmp = queue->head;
	if( tmp->key > key ) {
		return( STAT_NOT_FOUND );
	}

	queue->head = tmp->next;
	queue->length--;

	if( queue->length < 1 ) {
		queue->tail = NULL;
	}

	if( data != NULL ) {
		*data = tmp->data;
	}

	_qnode_free( tmp );

	return( STAT_SUCCESS );

} /* _q_remove_if_le */


/*
** _q_reset( queue, ordering )
**
** reinitialize the specified queue using the indicated
** ordering method
*/

void _q_reset( queue_t *queue, int ordering ) {

	queue->head = queue->tail = NULL;
	queue->length = 0;

	switch( ordering ) {

		case ORDER_NONE:
			queue->compare = NULL;
			break;
		
		case ORDER_UINT_ASCENDING:
			queue->compare = _compare_uint_ascending;
			break;
		
		default:
			_kpanic( "_q_reset, bad ordering selection" );
	}

} /* _q_reset */

/*
** _queue_dump(which,queue)
**
** dump the contents of the specified queue to the console
*/

void _queue_dump( char *which, queue_t *queue ) {
	qnode_t *tmp;

	c_printf( "%s:  head %08x tail %08x len %d compare %08x\n", which,
		queue->head, queue->tail, queue->length, queue->compare );

	if( queue->length ) {
		c_puts( " i/s/p/c: " );
		tmp = queue->head;
		while( tmp ) {
			c_printf( " [%d/%d/0x%x/0x%x]",
				((pcb_t *)(tmp->data))->pid,
				((pcb_t *)(tmp->data))->state,
				((pcb_t *)(tmp->data))->priority,
				((pcb_t *)(tmp->data))->children );
			tmp = tmp->next;
		}
		c_puts( "\n" );
	}

}


/*
** _queue_init()
**
** initialize the queue module
*/


void _queue_init( void ) {
	int i;

	/*
	** Set up the free list of qnodes
	*/

	for( i = 0; i < N_QNODES; ++i ) {
		_qnode_free( &_qnodes[i] );
	}

	c_puts( " queues" );

} /* _init_queues() */
