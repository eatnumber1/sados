/*
** SCCS ID:	@(#)user.h	1.1	04/03/10
**
** File:	user.h
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	User routines.
*/

#ifndef _USER_H
#define _USER_H

/*
** General (C and/or assembly) definitions
*/

/*
** Delay loop counts
*/

#define	LONG_DELAY	100000000
#define	STD_DELAY	  2500000
#define	ALT_DELAY	  4500000

#ifndef __ASM__20093__

/*
** Start of C-only definitions
*/

/*
** User process controls.
**
** The value field of these definitions contains a list of the
** system calls this user process uses.
**
** To spawn a specific user process from the initial process,
** uncomment its entry in this list.
*/

#define	SPAWN_A		// write, exit
#define	SPAWN_B		// write, exit
#define	SPAWN_C		// write, exit
#define	SPAWN_D		// write, fork, exec, exit
#define	SPAWN_E		// write, getpid, sleep, exit
#define	SPAWN_F		// write, getpid, sleep, exit
#define	SPAWN_G		// write, getpid, sleep, exit
#define	SPAWN_H		// write
// no user i
#define	SPAWN_J		// write, fork, exec, exit
#define	SPAWN_K		// write, sleep, fork, exec, exit
#define	SPAWN_L		// write, sleep, fork, exec, getticks, exit
#define	SPAWN_M		// write, fork, exec, sleep, exit
#define	SPAWN_N		// write, sleep, fork, exec, getticks, getppid, exit
// no user O
#define	SPAWN_P		// write, sleep, getticks
#define	SPAWN_Q		// write, bogus, exit
#define	SPAWN_R		// write, read, sleep, exit
#define	SPAWN_S		// write, sleep, forever
#define	SPAWN_T		// write, getprio, setprio, exit
// no user U
// no user V

/*
** Users W-Z are spawned from other processes; they should
** should never be spawned directly.
*/

// user W:  write, sleep, getpid, exit
// user X:  write, getpid, exit
// user Y:  write, sleep, exit
// user Z:  write, exit


/*
** Prototypes for externally-visible routines
*/

void idle_main( uint_t n, ... );

void init_main( uint_t n, ... );

#endif

#endif
