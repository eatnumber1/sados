/*
** SCCS ID:	@(#)clock.h	1.1	04/03/10
**
** File:	clock.h
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	clock handler
*/

#ifndef _CLOCK_H
#define _CLOCK_H

#include "headers.h"

// clock interrupts per second

#define	CLOCK_FREQUENCY		100

#ifndef __ASM__20093__

/*
** Start of C-only definitions
*/

// pseudo-function to convert milliseconds into ticks

// assuming 100 ticks/sec:  1 tick = 10 ms
// #define	MS_TO_TICKS(n)		( ((n) + 9) / 10 )

// assuming 18 ticks/sec:  1 tick = 55.5555 ms
#define	MS_TO_TICKS(n)		( ((n) + 54) / 55)

/*
** Timing constants
**
** Approximately every N seconds, we want to dump the queues.
**
** Assuming a clock frequency of 100, "N seconds" will be "N00"
** ticks.  For simplicity, we actually dump after tick counts
** which are powers of 2:
**
**		Seconds		Ticks		Mask
**		=======		=====		=====
**		2.5		256		0x00ff
**		5		512		0x01ff
**		10		1024		0x03ff
**		20		2048		0x07ff
**		40		4096		0x0fff
*/

// #define	CLK_SEC_2_5		0x00ff
// #define	CLK_SEC_5		0x01ff
// #define	CLK_SEC_10		0x03ff
// #define	CLK_SEC_20		0x07ff
// #define	CLK_SEC_40		0x0fff

/*
** Assuming a clock frequency of 18, "N seconds" will be N*18
** ticks.  For simplicity, we actually dump after tick counts
** which are powers of 2:
**
**		Seconds		Ticks		Mask
**		=======		=====		=====
**		7		128		0x007f
**		14		256		0x008f
**		28		512		0x01ff
**		56		1024		0x03ff
**		113		2048		0x07ff
**		227		4096		0x0fff
*/

#define	CLK_SEC_7		0x00ff
#define	CLK_SEC_14		0x00ff
#define	CLK_SEC_28		0x01ff
#define	CLK_SEC_56		0x03ff
#define	CLK_SEC_113		0x07ff
#define	CLK_SEC_227		0x0fff

/*
** Globals
*/

extern uint32_t _ticks;

/*
** _isr_clock - clock ISR
*/

void _isr_clock( int vector, int code );

/*
** _clock_init()
**
** initialize all clock-related variables
*/

void _clock_init( void );

#endif

#endif
