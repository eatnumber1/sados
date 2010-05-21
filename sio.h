/*
** SCCS ID:	@(#)sio.h	1.1	04/03/10
**
** File:	sio.h
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	SIO definitions
*/

#ifndef _SIO_H
#define _SIO_H

/*
** General (C and/or assembly) definitions
*/

// sio interrupt settings

#define	SIO_TX		0x01
#define	SIO_RX		0x02
#define	SIO_BOTH	(SIO_TX | SIO_RX)

#ifndef __ASM__20093__

/*
** Start of C-only definitions
*/

/*
** _isr_sio(vector, code)
**
** SIO interrupt service routine
*/

void _isr_sio( int vector, int code );

/*
** _sio_init()
**
** Initialize the UART chip.
*/

void _sio_init( void );

/*
** _sio_enable()
**
** enable/disable SIO interrupts
**
** usage:       old = _sio_enable( which )
**
** enables interrupts according to the 'which' parameter
**
** returns the prior settings
*/

uint8_t _sio_enable( uint8_t which );

/*
** _sio_disable()
**
** disable/disable SIO interrupts
**
** usage:       old = _sio_disable( which )
**
** disables interrupts according to the 'which' parameter
**
** returns the prior settings
*/

uint8_t _sio_disable( uint8_t which );

/*
** _sio_read()
**
** get the next input character
**
** usage:	ch = _sio_read()
**
** returns the next character, or -1 if no character is available
*/

int _sio_read( void );

/*
** _sio_write( ch )
**
** write a character to the serial output
**
** usage:	_sio_write( ch )
*/

void _sio_write( int ch );

/*
** _sio_dump()
**
** dump the contents of the SIO buffers
*/

void _sio_dump( void );

void _sio_puts( char *str );
void _sio_puti( int num );

#endif

#endif
