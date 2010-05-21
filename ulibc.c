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
#include "string.h"

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

#define BACKSPACE 127

uint32_t sio_gets( char *str, uint32_t size ) {
	// TODO: Handle if the user passes in a size <= 0.
	uint32_t count = 0;
	char c;
	bool buf_full = false;
	while( true ) {
		c = read();
		if( c == BACKSPACE ) {
			if( count > 0 ) {
				count--;
				sio_puts("\b \b");
				buf_full = false;
			}
			continue;
		}
		if( !buf_full ) str[count] = c;
		if( !buf_full || c == '\n' ) write(c);
		if( c == '\n' ) break;
		if( !buf_full ) count++;
		if( count == size - 1 ) buf_full = true;
	}
	str[count] = '\0';
	return count;
}

void sio_puts( char *str ) {
	uint32_t count = 0, len = strlen(str);
	while( count != len ) {
		write(str[count++]);
	}
}

void memclr( void *buffer, uint32_t length ) {
	register unsigned char *buf = (unsigned char *)buffer;
	register uint32_t num = length;

	while( num-- ) {
		*buf++ = 0;
	}

}
