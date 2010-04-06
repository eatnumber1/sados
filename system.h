/*
** SCCS ID:	@(#)system.h	1.1	04/03/10
**
** File:	system.h
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	Miscellaneous system routines
*/

#ifndef _SYSTEM_H
#define _SYSTEM_H

/*
** General (C and/or assembly) definitions
*/

#ifndef __ASM__20093__

#include "types.h"
#include "processes.h"

/*
** Start of C-only definitions
*/

/*
** Prototypes
*/

/*
** _init:	system initialization routine
**
** Called by startup code immediately before returning into the
** first user-level process.  The name is taken from there.
*/

void _init( void );

#endif

#endif
