/*
** SCCS ID:	@(#)defs.h	1.1	04/03/10
**
** File:	defs.h
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	Global definitions
*/

#ifndef _DEFS_H
#define _DEFS_H

#include "headers.h"

/*
** General (C and/or assembly) definitions
*/

// Boolean constants

#define	FALSE		0
#define	TRUE		1

// Generic NULL pointer

#define	NULL		0

// Status values

#define	STAT_SUCCESS		0
#define	STAT_FAILURE		(-1)
#define	STAT_NO_QNODE		(-2)
#define	STAT_EMPTY_QUEUE	(-3)
#define	STAT_NOT_FOUND		(-4)
#define	STAT_BAD_PARAM		(-5)
#define	STAT_NOT_YOURS		(-6)
#define	STAT_NO_CHILDREN	(-7)
#define	STAT_NO_SUCH_PID	(-8)
#define	STAT_EXIT_NOT_CALLED	(-9)

// size of user stacks (in longwords)

#define	STACK_SIZE	1024

// maximum number of simultaneous user processes in the system

#define	N_PCBS		24

#ifndef __ASM__20093__

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

#endif

#endif
