/*
** SCCS ID:	@(#)scheduler.h	1.1	04/03/10
**
** File:	scheduler.h
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	Scheduler module
*/

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "headers.h"

/*
** General (C and/or assembly) definitions
*/

#ifndef __ASM__20093__

#include "processes.h"

/*
** Start of C-only definitions
*/

/*
** Types
*/

/*
** Globals
*/

/*
** Prototypes
*/

/*
** _schedule( pcb, priority )
**
** put PCB into the ready queue with the specified priority
**
** returns _q_insert() status
*/

int _schedule( pcb_t *pcb, uint_t priority );

/*
** _dispatch( void )
**
** dispatch the next process
*/

void _dispatch( void );

#endif

#endif
