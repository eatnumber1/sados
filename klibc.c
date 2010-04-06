/*
** SCCS ID:	@(#)klibc.c	1.1	04/03/10
**
** File:	klibc.c
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	C implementations of some kernel-level library routines
*/

#define	__KERNEL__20093__

#include "headers.h"

/*
** put_char_or_code( ch )
**
** prints the character on the console, unless it is a non-printing
** character, in which case its hex code is printed
*/

void put_char_or_code( int ch ) {

	if( ch >= ' ' && ch < 0x7f ) {
		c_putchar( ch );
	} else {
		c_printf( "\\x%02x", ch );
	}

}

/*
** _memclr - initialize all bytes of a block of memory to zero
**
** usage:  _memclr( buffer, length )
*/

void _memclr( void *buffer, uint32_t length ) {
	register unsigned char *buf = (unsigned char *)buffer;
	register uint32_t num = length;

	while( num-- ) {
		*buf++ = 0;
	}

}

/*
** _memcpy - copy a block from one place to another
**
** usage:  _memcpy( dest, src, length )
**
** may not correctly deal with overlapping buffers
*/

void _memcpy( void *destination, void *source, uint32_t length ) {
	register unsigned char *dst = (unsigned char *)destination;
	register unsigned char *src = (unsigned char *)source;
	register uint32_t num = length;

	while( num-- ) {
		*dst++ = *src++;
	}

}


/*
** _kpanic(msg)
**
** kernel-level panic routine
**
** usage:  _kpanic( msg )
**
** Prefix routine for __panic() - can be expanded to do other things
** (e.g., printing a stack traceback)
*/

void _kpanic( char *msg ) {

	c_puts( "\n\n***** KERNEL PANIC *****\n\n" );

	//
	// Pretty simple, actually
	//

	__panic( msg );

}


/*
** _kprt_status - print a status value to the console
**
** the 'msg' argument should contain a %s where
** the desired status value should be printed
**
** this is identical to prt_status(), but is here so that user
** routines which invoke it won't use the same version as the 
** kernel uses, which will simplify life for people implementing
** full VM.
*/

void _kprt_status( char *msg, int stat ) {

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
