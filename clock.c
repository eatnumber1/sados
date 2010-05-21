/*
** SCCS ID:	@(#)clock.c	1.1	04/03/10
**
** File:	clock.c
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	clock handler
*/

#define	__KERNEL__20093__

#include "headers.h"

#include "clock.h"

#include "processes.h"
#include "queues.h"
#include "scheduler.h"
#include "sio.h"
#include "startup.h"

#include <uart.h>
#include <x86arch.h>

/*
** PRIVATE GLOBAL VARIABLES
*/

	// pinwheel control variables
static uint32_t _pinwheel;
static uint32_t _pindex;

/*
** PUBLIC GLOBAL VARIABLES
*/

	// system time (in ticks)

uint32_t _ticks;

/*
** PUBLIC FUNCTIONS
*/

/*
** _isr_clock - clock ISR
*/

void _isr_clock( int vector, int code ) {
	pcb_t *pcb;
	int stat;

	// increment system time

	++_ticks;

	// spin the pinwheel

	++_pinwheel;
	if( _pinwheel == (CLOCK_FREQUENCY / 10) ) {
		_pinwheel = 0;
		++_pindex;
		c_putchar_at( 79, 0, "|/-\\"[ _pindex & 3 ] );
	}

	// wake up any sleepers whose time has come

	stat = _q_remove_if_le( &_sleeping, (void *) &pcb, _ticks );
	while( stat == STAT_SUCCESS ) {
		stat = _schedule( pcb, pcb->priority );
		if( stat != STAT_SUCCESS ) {
			_kprt_status( "wakeup sched failure: %s", stat );
			_kpanic( "_isr_clock, wakeup curr failed" );
		}
		stat = _q_remove_if_le( &_sleeping, (void *) &pcb, _ticks );
	}

	// no explicit quantum - we preempt after each clock tick
	//
	// put the current process back on the ready queue

	stat = _schedule( _current, _current->priority );
	if( stat != STAT_SUCCESS ) {
		_kprt_status( "clock sched failure: %s", stat );
		_kpanic( "_isr_clock, sched curr failed" );
	}

	/*
	** Approcimately every 20 seconds, dump the queues,
	** and print the contents of the SIO buffers
	*/

/*
	if( (_ticks & CLK_SEC_28) == 0 ) {
		c_printf( "Queue contents @%08x\n", _ticks );
		_queue_dump( "ready   ", &_ready );
		_queue_dump( "sleeping", &_sleeping );
		_queue_dump( "reading ", &_reading );
		_queue_dump( "zombies ", &_zombies );
		_queue_dump( "wait_g  ", &_wait_generic );
		_queue_dump( "wait_s  ", &_wait_specific );
		_sio_dump();
	}
*/

	// select a new current process

	_dispatch();

	// tell the PIC that we're done

	__outb( PIC_MASTER_CMD_PORT, PIC_EOI );

}

/*
** _clock_init()
**
** initialize all clock-related variables
*/

void _clock_init( void ) {

	// start the pinwheel

	_pinwheel = _pindex = 0;

	// reset the system time to the epoch

	_ticks = 0;

	// set the clock to tick at CLOCK_FREQUENCY Hz.

	__outb( TIMER_0, ( TIMER_FREQUENCY / CLOCK_FREQUENCY ) & 0xff );
	__outb( TIMER_1, ( TIMER_FREQUENCY / CLOCK_FREQUENCY ) >>   8 );


	// announce that we have initialized the clock module

	c_puts( " clock" );

}
