/*
** SCCS ID:	@(#)user.c	1.1	04/03/10
**
** File:	user.c
**
** Author:	4003-506 class of 20093
**
** Contributor: Christine Viets (cev5122)
**
** Description:	User routines.
*/

#include "headers.h"

#include "user.h"
#include "processes.h"
//#include "ide.h"

/*
** USER PROCESSES
**
** Each is designed to test some facility of the OS:
**
**	User(s)		Tests/Features
**	=======		===============================================
**	A, B, C		Basic operation
**	D		Spawns Z and exits
**	E, F, G		Sleep for different lengths of time
**	H		Doesn't call exit()
**	J		Tries to spawn 2*N_PCBS copies of Y
**	K		Spawns several copies of X
**	L		Spawns several copies of X, changes PID
**	M		Spawns W three times, reporting PIDs
**	N		Spawns several copies of X, reports times & ppid
**	P		Iterates three times, printing system time
**	Q		Tries to execute a bogus system call (bad code)
**	R		Reading and writing
**	S		Loops forever, sleeping 30 seconds at a time
**	T		Loops, fiddles with priority
**
**	W, X, Y, Z	Print characters (spawned by other processes)
**
** Output from user processes is always alphabetic.  Uppercase 
** characters are "expected" output; lowercase are "erroneous"
** output.
**
** More specific information about each user process can be found in
** the header comment for that function (below).
**
** To spawn a specific user process, uncomment its SPAWN_x
** definition in the user.h header file.
*/

/*
** Prototypes for all one-letter user main routines
*/

void user_a_main( uint_t, ... ); void user_b_main( uint_t, ... );
void user_c_main( uint_t, ... ); void user_d_main( uint_t, ... );
void user_e_main( uint_t, ... ); void user_f_main( uint_t, ... );
void user_g_main( uint_t, ... ); void user_h_main( uint_t, ... );
void user_i_main( uint_t, ... ); void user_j_main( uint_t, ... );
void user_k_main( uint_t, ... ); void user_l_main( uint_t, ... );
void user_m_main( uint_t, ... ); void user_n_main( uint_t, ... );
void user_o_main( uint_t, ... ); void user_p_main( uint_t, ... );
void user_q_main( uint_t, ... ); void user_r_main( uint_t, ... );
void user_s_main( uint_t, ... ); void user_t_main( uint_t, ... );
void user_u_main( uint_t, ... ); void user_v_main( uint_t, ... );
void user_w_main( uint_t, ... ); void user_x_main( uint_t, ... );
void user_y_main( uint_t, ... ); void user_z_main( uint_t, ... );

/*
** Users A, B, and C are identical, except for the character they
** print out via write().  Each prints its ID, then loops 30
** times delaying and printing, before exiting.
*/

void user_a_main( uint_t n, ... ) {
	int i, j;

	c_puts( "User process A running\n" );
	write( 'A' );
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < STD_DELAY; ++j )
			continue;
		write( 'A' );
	}

	c_puts( "User process A exiting\n" );
	exit( STAT_SUCCESS );

}

void user_b_main( uint_t n, ... ) {
	int i, j;

	c_puts( "User process B running\n" );
	write( 'B' );
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < STD_DELAY; ++j )
			continue;
		write( 'B' );
	}

	c_puts( "User process B exiting\n" );
	exit( STAT_SUCCESS );

}

void user_c_main( uint_t n, ... ) {
	int i, j;

	c_puts( "User process C running\n" );
	write( 'C' );
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < STD_DELAY; ++j )
			continue;
		write( 'C' );
	}

	c_puts( "User process C exiting\n" );
	exit( STAT_SUCCESS );

	write( 'c' );	/* shouldn't happen! */

}


/*
** User D spawns user Z.
*/

void user_d_main( uint_t n, ... ) {
	int pid;

	c_puts( "User process D running\n" );
	write( 'D' );
	pid = fork( PRIO_STD );
	if( pid < 0 ) {
		write( 'd' );
	} else if( pid == 0 ) {
		exec( user_z_main, 0 );
		write( 'z' );
		c_puts( "User D, exec of Z failed\n" );
		exit( STAT_FAILURE );
	}
	write( 'D' );

	c_puts( "User process D exiting\n" );
	exit( STAT_SUCCESS );

}


/*
** Users E, F, and G test the sleep facility.
**
** User E sleeps for 10 seconds at a time.
*/

void user_e_main( uint_t n, ... ) {
	int i, pid;

	pid = getpid();
	c_printf( "User process E (%d) running\n", pid );
	write( 'E' );
	for( i = 0; i < 5 ; ++i ) {
		sleep( 10000 );
		write( 'E' );
	}

	c_puts( "User process E exiting\n" );
	exit( STAT_SUCCESS );

}


/*
** User F sleeps for 5 seconds at a time.
*/

void user_f_main( uint_t n, ... ) {
	int i, pid;

	pid = getpid();
	c_printf( "User process F (%d) running\n", pid );
	write( 'F' );
	for( i = 0; i < 5 ; ++i ) {
		sleep( 5000 );
		write( 'F' );
	}

	c_puts( "User process F exiting\n" );
	exit( STAT_SUCCESS );

}


/*
** User G sleeps for 15 seconds at a time.
*/

void user_g_main( uint_t n, ... ) {
	int i, pid;

	pid = getpid();
	c_printf( "User process G (%d) running\n", pid );
	write( 'G' );
	for( i = 0; i < 5; ++i ) {
		sleep( 15000 );
		write( 'G' );
	}

	c_puts( "User process G exiting\n" );
	exit( STAT_SUCCESS );

}


/*
** User H is like A-C except it only loops 5 times and doesn't
** call exit().
*/

void user_h_main( uint_t n, ... ) {
	int i, j;

	c_puts( "User process H running\n" );
	write( 'H' );
	for( i = 0; i < 5; ++i ) {
		for( j = 0; j < STD_DELAY; ++j )
			continue;
		write( 'H' );
	}

	c_puts( "User process H returning without exiting!\n" );

}


/*
** User J tries to spawn 2*N_PCBS copies of user_y.
*/

void user_j_main( uint_t n, ... ) {
	int i, pid;

	c_puts( "User process J running\n" );
	write( 'J' );

	for( i = 0; i < N_PCBS * 2 ; ++i ) {
		pid = fork( PRIO_STD );
		if( pid < 0 ) {
			write( 'j' );
		} else if( pid == 0 ) {
			exec( user_y_main, 0 );
			write( 'y' );
			c_puts( "User J, exec of y failed\n" );
		} else {
			write( 'J' );
		}
	}

	c_puts( "User process J exiting\n" );
	exit( STAT_SUCCESS );

}


/*
** User K prints, goes into a loop which runs three times, and exits.
** In the loop, it does a spawn of user_x, sleeps 30 seconds, and prints.
*/

void user_k_main( uint_t n, ... ) {
	int i, pid;

	c_puts( "User process K running\n" );
	write( 'K' );

	for( i = 0; i < 3 ; ++i ) {
		pid = fork( PRIO_STD );
		if( pid < 0 ) {
			write( 'k' );
		} else if( pid == 0 ) {
			exec( user_x_main, 0 );
			write( 'x' );
			c_puts( "User K, exec of x failed\n" );
		} else {
			sleep( 30000 );
			write( 'K' );
		}
	}

	c_puts( "User process K exiting\n" );
	exit( STAT_SUCCESS );

}


/*
** User L is like user K, except that it reports the ssytem time in each
** iteration of the loop.
*/

void user_l_main( uint_t n, ... ) {
	int i, pid;
	uint32_t time;

	c_puts( "User process L running, " );
	time = getticks();
	c_printf( " initial time %d\n", time );
	write( 'L' );

	for( i = 0; i < 3 ; ++i ) {
		time = getticks();
		pid = fork( PRIO_STD );
		if( pid < 0 ) {
			write( 'l' );
		} else if( pid == 0 ) {
			exec( user_x_main, 0 );
			write( 'x' );
			c_printf( "User L, exec failed at time %d\n", time );
		} else {
			c_printf( "User L, exec succeeded at time %d\n", time );
			sleep( 30000 );
			write( 'L' );
		}
	}

	time = getticks();
	c_printf( "User process L exiting at time %d\n", time );
	exit( STAT_SUCCESS );

}


/*
** User M iterates spawns three copies of user W, reporting their PIDs.
*/

void user_m_main( uint_t n, ... ) {
	int i, pid[3];

	c_puts( "User process M running\n" );
	for( i = 0; i < 3; ++i ) {
		pid[i] = fork( PRIO_STD );
		if( pid[i] < 0 ) {
			write( 'm' );
		} else if( pid[i] == 0 ) {
			exec( user_w_main, 0 );
			write( 'w' );
			c_puts( "User M, exec of W failed\n" );
		} else {
			c_printf( "User M spawned W, PID %d\n", pid[i] );
			write( 'M' );
		}
	}

	c_puts( "User process M exiting\n" );
	exit( STAT_SUCCESS );

}


/*
** User N is like user L, except that it reports its PID when starting
*/

void user_n_main( uint_t n, ... ) {
	int i, pid, ppid;
	uint32_t time;

	pid = getpid();
	ppid = getppid( pid );
	c_printf( "User process N (%d) running, parent %d\n", pid, ppid );
	write( 'N' );

	for( i = 0; i < 3 ; ++i ) {
		time = getticks();
		pid = fork( PRIO_STD );
		if( pid < 0 ) {
			write( 'n' );
		} else if( pid == 0 ) {
			exec( user_x_main, 0 );
			write( 'x' );
			c_printf( "User N, exec failed at time %d\n", time );
		} else {
			c_printf( "User N, exec succeeded at time %d\n", time );
			sleep( 30000 );
			write( 'N' );
		}
	}

	c_puts( "User process N exiting\n" );
	exit( STAT_SUCCESS );

}


/*
** User P iterates three times.  Each iteration sleeps for two seconds,
** then gets and prints the system time.
*/

void user_p_main( uint_t n, ... ) {
	uint32_t t;
	int i;

	c_printf( "User process P running, " );
	t = getticks();
	c_printf( " time %d\n", t );

	write( 'P' );

	for( i = 0; i < 3; ++i ) {
		sleep( 2000 );
		t = getticks();
		c_printf( "User process P reporting time %d\n", t );
		write( 'P' );
	}

	c_printf( "User process P exiting\n" );
	exit( STAT_SUCCESS );

}


/*
** User Q does a bogus system call
*/

void user_q_main( uint_t n, ... ) {

	c_puts( "User process Q running\n" );
	write( 'Q' );
	bogus();
	c_puts( "User process Q exiting!?!?!\n" );
	exit( STAT_SUCCESS );

}


/*
** User R loops 3 times reading/writing, then exits.
*/

void user_r_main( uint_t n, ... ) {
	int i;
	int ch = '&';

	c_puts( "User process R running\n" );
	for( i = 0; i < 3; ++i ) {
		do {
			write( 'R' );
			ch = read();
			if( ch == -1 ) {	/* wait a bit */
				sleep( 1000 );
			}
		} while( ch == -1 );
		write( ch );
	}

	c_puts( "User process R exiting\n" );
	exit( STAT_SUCCESS );

}


/*
** User S sleeps for 30 seconds at a time, and loops forever.
*/

void user_s_main( uint_t n, ... ) {

	c_puts( "User process S running\n" );
	write( 'S' );
	for(;;) {
		sleep( 30000 );
		write( 'S' );
	}

	c_puts( "User process S exiting!?!?!n" );
	exit( STAT_SUCCESS );

}


/*
** User T changes its priority back and forth several times
*/

void user_t_main( uint_t n, ... ) {
	int priority, prio2;
	int i, j, pid;

	pid = getpid();
	priority = getprio( pid );
	c_printf( "User process T (%d) running, initial prio %d\n",
		  pid, priority );
	
	write( 'T' );
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < STD_DELAY; ++j )
			continue;
		write( 'T' );
	}

	priority = setprio( pid, PRIO_HIGH );
	if( priority < 0 ) {
		write( 't' );
		prt_status( "User T, setprio() %s\n", priority );
		exit( STAT_FAILURE );
	}
	prio2 = getprio( pid );
	c_printf( "User process T, prio was %d now %d\n", priority, prio2 );
	
	write( 'T' );
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < STD_DELAY; ++j )
			continue;
		write( 'T' );
	}

	priority = setprio( pid, PRIO_LOW );
	if( priority < 0 ) {
		write( 't' );
		prt_status( "User T, setprio() %s\n", priority );
		exit( STAT_FAILURE );
	}
	prio2 = getprio( pid );
	c_printf( "User process T, prio was %d now %d\n", priority, prio2 );
	
	write( 'T' );
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < STD_DELAY; ++j )
			continue;
		write( 'T' );
	}

	priority = setprio( pid, PRIO_STD );
	if( priority < 0 ) {
		write( 't' );
		prt_status( "User T, setprio() %s\n", priority );
		exit( STAT_FAILURE );
	}
	prio2 = getprio( pid );
	c_printf( "User process T, prio was %d now %d\n", priority, prio2 );
	
	write( 'T' );
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < STD_DELAY; ++j )
			continue;
		write( 'T' );
	}

	c_puts( "User process T exiting\n" );
	exit( STAT_SUCCESS );

}


/*
** User W prints W characters 20 times, sleeping 3 seconds between.
*/

void user_w_main( uint_t n, ... ) {
	int i;
	int pid;

	c_printf( "User process W running, " );
	pid = getpid();
	c_printf( " PID %d\n", pid );
	for( i = 0; i < 20 ; ++i ) {
		write( 'W' );
		sleep( 3000 );
	}

	c_printf( "User process W exiting, PID %d\n", pid );
	exit( STAT_SUCCESS );

}


/*
** User X prints X characters 20 times.  It is spawned multiple
** times, and prints its PID when started and exiting.
*/

void user_x_main( uint_t n, ... ) {
	int i, j;
	int pid;

	c_puts( "User process X running, " );
	pid = getpid();
	c_printf( "PID %d, ", pid );

	for( i = 0; i < 20 ; ++i ) {
		write( 'X' );
		for( j = 0; j < STD_DELAY; ++j )
			continue;
	}

	c_printf( "User process X exiting, PID %d\n", pid );
	exit( STAT_SUCCESS );

}


/*
** User Y prints Y characters 10 times.
*/

void user_y_main( uint_t n, ... ) {
	int i, j;

	c_puts( "User process Y running\n" );
	for( i = 0; i < 10 ; ++i ) {
		write( 'Y' );
		for( j = 0; j < ALT_DELAY; ++j )
			continue;
		sleep( 1000 );
	}

	c_puts( "User process Y exiting\n" );
	exit( STAT_SUCCESS );

}


/*
** User Z prints Z characters 10 times.
*/

void user_z_main( uint_t n, ... ) {
	int i, j;

	c_puts( "User process Z running\n" );
	for( i = 0; i < 10 ; ++i ) {
		write( 'Z' );
		for( j = 0; j < STD_DELAY; ++j )
			continue;
	}

	c_puts( "User process Z exiting\n" );
	exit( STAT_SUCCESS );

}


/*
** SYSTEM PROCESSES
*/

void findbios_main( uint_t n, ... ) {
    int pid;
    int priority;

    pid = getpid();
    priority = getprio (pid);
    c_printf( "Find BIOS process (%d) started, prio %08x\n", pid, priority );
    findbios();
}

/*
** Idle process
*/

void idle_main( uint_t n, ... ) {
	int i;
	int pid;
	int priority;

	pid = getpid();
	priority = getprio( pid );
	c_printf( "Idle process (%d) started, prio %08x\n", pid, priority );

	write( '.' );

	for(;;) {
		for( i = 0; i < LONG_DELAY; ++i )
			continue;
		write( '.' );
	}

	c_puts( "+++ Idle process done!?!?!\n" );

	exit( STAT_FAILURE );
}


/*
** Initial process; it starts the other top-level user processes.
*/

void init_main( uint_t n, ... ) {
	int pid;
	wait_t info;
	int stat;

	c_puts( "Init started\n" );

	write( '$' );

	/*
	** Always start the idle process first
	*/

	pid = fork( PRIO_MIN );
	if( pid < 0 ) {
		c_puts( "init: can't fork idle\n" );
	} else if( pid == 0 ) {
		exec( idle_main, 0 );
		c_puts( "init: can't exec idle\n" );
		exit( STAT_FAILURE );
	}

    //pid = fork (PRIO_STD);
    //if (pid < 0) {
    //    c_puts( "init: can't fork test\n");
    //} else if (pid == 0) {
    //    exec( findbios_main, 0);
    //    c_puts ("init: can't exec findbios\n");
    //    exit(STAT_FAILURE);
    //}

#ifdef SPAWN_A
	pid = fork( PRIO_STD );
	if( pid < 0 ) {
		c_puts( "init: can't fork user A\n" );
	} else if( pid == 0 ) {
		exec( user_a_main, 0 );
		c_puts( "init: can't exec user A\n" );
		exit( STAT_FAILURE );
	}
#endif

#ifdef SPAWN_B
	pid = fork( PRIO_STD );
	if( pid < 0 ) {
		c_puts( "init: can't fork user B\n" );
	} else if( pid == 0 ) {
		exec( user_b_main, 0 );
		c_puts( "init: can't exec user B\n" );
		exit( STAT_FAILURE );
	}
#endif

#ifdef SPAWN_C
	pid = fork( PRIO_STD );
	if( pid < 0 ) {
		c_puts( "init: can't fork user C\n" );
	} else if( pid == 0 ) {
		exec( user_c_main, 0 );
		c_puts( "init: can't exec user C\n" );
		exit( STAT_FAILURE );
	}
#endif

#ifdef SPAWN_D
	pid = fork( PRIO_STD );
	if( pid < 0 ) {
		c_puts( "init: can't fork user D\n" );
	} else if( pid == 0 ) {
		exec( user_d_main, 0 );
		c_puts( "init: can't exec user D\n" );
		exit( STAT_FAILURE );
	}
#endif

#ifdef SPAWN_E
	pid = fork( PRIO_STD );
	if( pid < 0 ) {
		c_puts( "init: can't fork user E\n" );
	} else if( pid == 0 ) {
		exec( user_e_main, 0 );
		c_puts( "init: can't exec user E\n" );
		exit( STAT_FAILURE );
	}
#endif

#ifdef SPAWN_F
	pid = fork( PRIO_STD );
	if( pid < 0 ) {
		c_puts( "init: can't fork user F\n" );
	} else if( pid == 0 ) {
		exec( user_f_main, 0 );
		c_puts( "init: can't exec user F\n" );
		exit( STAT_FAILURE );
	}
#endif

#ifdef SPAWN_G
	pid = fork( PRIO_STD );
	if( pid < 0 ) {
		c_puts( "init: can't fork user G\n" );
	} else if( pid == 0 ) {
		exec( user_g_main, 0 );
		c_puts( "init: can't exec user G\n" );
		exit( STAT_FAILURE );
	}
#endif

#ifdef SPAWN_H
	pid = fork( PRIO_STD );
	if( pid < 0 ) {
		c_puts( "init: can't fork user H\n" );
	} else if( pid == 0 ) {
		exec( user_h_main, 0 );
		c_puts( "init: can't exec user H\n" );
		exit( STAT_FAILURE );
	}
#endif

#ifdef SPAWN_J
	pid = fork( PRIO_STD );
	if( pid < 0 ) {
		c_puts( "init: can't fork user J\n" );
	} else if( pid == 0 ) {
		exec( user_j_main, 0 );
		c_puts( "init: can't exec user J\n" );
		exit( STAT_FAILURE );
	}
#endif

#ifdef SPAWN_K
	pid = fork( PRIO_STD );
	if( pid < 0 ) {
		c_puts( "init: can't fork user K\n" );
	} else if( pid == 0 ) {
		exec( user_k_main, 0 );
		c_puts( "init: can't exec user K\n" );
		exit( STAT_FAILURE );
	}
#endif

#ifdef SPAWN_L
	pid = fork( PRIO_STD );
	if( pid < 0 ) {
		c_puts( "init: can't fork user L\n" );
	} else if( pid == 0 ) {
		exec( user_l_main, 0 );
		c_puts( "init: can't exec user L\n" );
		exit( STAT_FAILURE );
	}
#endif

#ifdef SPAWN_M
	pid = fork( PRIO_STD );
	if( pid < 0 ) {
		c_puts( "init: can't fork user M\n" );
	} else if( pid == 0 ) {
		exec( user_m_main, 0 );
		c_puts( "init: can't exec user M\n" );
		exit( STAT_FAILURE );
	}
#endif

#ifdef SPAWN_N
	pid = fork( PRIO_STD );
	if( pid < 0 ) {
		c_puts( "init: can't fork user N\n" );
	} else if( pid == 0 ) {
		exec( user_n_main, 0 );
		c_puts( "init: can't exec user N\n" );
		exit( STAT_FAILURE );
	}
#endif

#ifdef SPAWN_P
	pid = fork( PRIO_STD );
	if( pid < 0 ) {
		c_puts( "init: can't fork user P\n" );
	} else if( pid == 0 ) {
		exec( user_p_main, 0 );
		c_puts( "init: can't exec user P\n" );
		exit( STAT_FAILURE );
	}
#endif

#ifdef SPAWN_Q
	pid = fork( PRIO_STD );
	if( pid < 0 ) {
		c_puts( "init: can't fork user Q\n" );
	} else if( pid == 0 ) {
		exec( user_q_main, 0 );
		c_puts( "init: can't exec user Q\n" );
		exit( STAT_FAILURE );
	}
#endif

#ifdef SPAWN_R
	pid = fork( PRIO_STD );
	if( pid < 0 ) {
		c_puts( "init: can't fork user R\n" );
	} else if( pid == 0 ) {
		exec( user_r_main, 0 );
		c_puts( "init: can't exec user R\n" );
		exit( STAT_FAILURE );
	}
#endif

#ifdef SPAWN_S
	pid = fork( PRIO_STD );
	if( pid < 0 ) {
		c_puts( "init: can't fork user S\n" );
	} else if( pid == 0 ) {
		exec( user_s_main, 0 );
		c_puts( "init: can't exec user S\n" );
		exit( STAT_FAILURE );
	}
#endif

#ifdef SPAWN_T
	pid = fork( PRIO_STD );
	if( pid < 0 ) {
		c_puts( "init: can't fork user T\n" );
	} else if( pid == 0 ) {
		exec( user_t_main, 0 );
		c_puts( "init: can't exec user T\n" );
		exit( STAT_FAILURE );
	}
#endif

	write( '!' );

	/*
	** At this point, we go into an infinite loop
	** waiting for our children (direct, or inherited)
	** to exit.
	*/

	for(;;) {
		info.pid = PID_GENERIC;
		stat = wait( &info );
		if( stat == STAT_SUCCESS ) {
			c_printf( "Init: process %d exited, status %d\n",
			 	 info.pid, info.status );
		} else {
			prt_status( "init, wait status %s\n", stat );
			sleep( 5000 );
		}
	}

	/*
	** SHOULD NEVER REACH HERE
	*/

	c_printf( "*** INIT IS EXITING???\n" );
	exit( STAT_FAILURE );

}
