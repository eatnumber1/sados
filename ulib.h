/*
** SCCS ID:	@(#)ulib.h	1.1	04/03/10
**
** File:	ulib.h
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	User library routine definitions
*/

#ifndef _ULIB_H
#define _ULIB_H

#include "headers.h"

#include "processes.h"

/*
** General (C and/or assembly) definitions
*/

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


/*
** fork - create a new process at a specific priority
**
** usage:	pid = fork( prio );
**
** returns:
**	-1 if the creation failed
**	0 in the child
**	else, the PID of the new child
*/

int fork( uint_t prio );

/*
** exec - replace a process with a different program
**
** usage:	n = exec( address, n, p1, p2, ..., pn );
**
** returns:
**	nothing if the exec succeeds
**	else, reason for failure
*/

int exec( void (*fcn)(uint_t,...), uint_t nargs, ... );

/*
** exit - terminate the current process and clean up after it
**
** usage:	exit( status );
*/

void exit( int status );

/*
** wait - block until a child process exits
**
** usage:	n = wait( status_structure )
**
** returns on success:
**	in parameter, exit status of any or specified child
**	STAT_SUCCESS
**
** returns on failure:
**	reason for failure
*/

int wait( wait_t *stat );

/*
** sleep - put the current process to sleep for the specified number of ms
**
** usage:	sleep( ms );
*/

void sleep( int sec );

/*
** read - read one character from the SIO
**
** usage:	ch = read();
**
** returns:
**	the next input character
*/

int read( void );

/*
** write - write one character to the SIO
**
** usage:	write( ch );
**
** based on a simple interrupt-driven SIO model taken from SP1
*/

void write( int ch );

/*
** getpid - retrieve pid of the current process
**
** usage:	n = getpid();
**
** returns:
**	the current process' pid
*/

uint32_t getpid( void );

/*
** getppid - retrieve parent pid of the specified process
**
** usage:	pid = getppid( pid );
**
** returns:
**	PID of the parent of the specified process, or an error status
*/

int getppid( uint32_t pid );

/*
** getprio - retrieve priority of the specified process
**
** usage:	prio = getprio( pid );
**
** returns:
**	prioiryt of the specified process, or an error status
*/

int getprio( uint32_t pid );

/*
** setprio - change priority of the specified process
**
** usage:	prio = setprio( pid, prio );
**
** returns:
**	old priority of the specified process, or an error status
*/

int setprio( uint32_t pid, uint32_t prio );

/*
** getticks - retrieve the current system time
**
** usage:	n = getticks();
**
** returns:
**	the current system time
*/

uint32_t getticks( void );

/*
** bogus - a bogus system call
**
** should force the calling process to exit
*/

void bogus( void );

/*
** call_exit - dummy "return to" routine for user main functions
**
** never called directly; its reason for existance is to call
** exit() properly
*/

void call_exit( void );

/*
** prt_status - print a status value to the console
**
** the 'msg' argument should contain a %s where
** the desired status value should be printed
**
** this is identical to _kprt_status(), but is here so that user
** routines which invoke it won't use the same version as the
** kernel uses, which will simplify life for people implementing
** full VM.
*/

void prt_status( char *msg, int stat );

#endif

#endif
