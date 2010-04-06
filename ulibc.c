/*
** SCCS ID:	@(#)ulibc.c	1.1	04/03/10
**
** File:	ulibc.c
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	C implementations of some user-level library routines
*/

#include "headers.h"

/*
** call_exit - dummy "return to" routine for user main functions
**
** never called directly; its reason for existance is to call
** exit() properly
*/

void call_exit( void ) {

	exit( STAT_EXIT_NOT_CALLED );

}

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

void prt_status( char *msg, int stat ) {

	if( msg == NULL ) {
		return;
	}

	switch( stat ) {
		case STAT_SUCCESS:
			c_printf( msg, "STAT_SUCCESS" );
			break;

		case STAT_FAILURE:
			c_printf( msg, "STAT_FAILURE" );
			break;

		case STAT_NO_QNODE:
			c_printf( msg, "STAT_NO_QNODE" );
			break;

		case STAT_EMPTY_QUEUE:
			c_printf( msg, "STAT_EMPTY_QUEUE" );
			break;

		case STAT_NOT_FOUND:
			c_printf( msg, "STAT_NOT_FOUND" );
			break;

		case STAT_BAD_PARAM:
			c_printf( msg, "STAT_BAD_PARAM" );
			break;

		case STAT_NOT_YOURS:
			c_printf( msg, "STAT_NOT_YOURS" );
			break;

		case STAT_NO_CHILDREN:
			c_printf( msg, "STAT_NO_CHILDREN" );
			break;

		case STAT_NO_SUCH_PID:
			c_printf( msg, "STAT_NO_SUCH_PID" );
			break;

		case STAT_EXIT_NOT_CALLED:
			c_printf( msg, "STAT_EXIT_NOT_CALLED" );
			break;

		default:
			c_printf( msg, "STAT_????" );
			c_printf( "Status code %d\n", stat );
			break;
	
	}

}
