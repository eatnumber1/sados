/*
** SCCS ID:	@(#)sio.c	1.1	04/03/10
**
** File:	sio.c
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	SIO module
**
** Our SIO scheme is very simple:
**
**	Input:	We maintain a buffer of incoming characters that haven't
**		yet been read by processes.  When a character comes in,
**		if there is no process waiting for it, it goes in the 
**		buffer; otherwise, the first waiting process is awakened
**		and it gets the character.
**
**		When a process invokes read(), if there is a character in
**		the input buffer, the process gets it; otherwise, it is
**		blocked until input appears.
**
**		Communication with user processes is via the _sio_read()
**		routine.  This returns the first available character (if
**		there is one), resetting the input variables if this was
**		the last character in the buffer.  If there are no
**		characters in the buffer, _sio_read() returns a -1
**		(presumably so the requesting process can be blocked).
**
**	Output:	We maintain a buffer of outgoing characters that haven't
**		yet been sent to the device, and an indication of whether
**		or not we are in the middle of a transmit sequence.  When
**		an interrupt comes in, if there is another character to
**		send we copy it to the transmitter buffer; otherwise, we
**		end the transmit sequence.
**
**		Communication with user processes is via the _sio_write()
**		routine.  If we are in the middle of a transmit sequence,
**		the character will be added to the output buffer (from
**		where it will be sent automatically); otherwise, we send
**		the character directly, and set the "sending" flag to 
**		indicate that we're expecting a transmitter interrupt.
*/

#define	__KERNEL__20093__

#include "headers.h"

#include "sio.h"

#include "queues.h"
#include "processes.h"
#include "scheduler.h"
#include "string.h"

#include "startup.h"
#include <uart.h>
#include <x86arch.h>

/*
** PRIVATE DEFINITIONS
*/

#define	BUF_SIZE	1024

/*
** PRIVATE GLOBALS
*/

	// input character buffer
static char _inbuffer[ BUF_SIZE ];
static char *_inlast;
static char *_innext;
static uint32_t _incount;

	// output character buffer
static char _outbuffer[ BUF_SIZE ];
static char *_outlast;
static char *_outnext;
static uint32_t _outcount;

	// output control flag
static int _sending;

	// interrupt register status
static uint8_t _ier;

/*
** PUBLIC FUNCTIONS
*/

/*
** _isr_sio - serial i/o ISR
*/

void _isr_sio( int vector, int code ) {
	pcb_t *pcb;
	int eir;
	int ch;
	int stat;

	//
	// Must process all pending events; loop until the EIR
	// says there's nothing else to do.
	//

	for(;;) {

		// get the "pending event" indicator
		eir = __inb( UA4_EIR ) & UA4_EIR_INT_PRI_MASK;

		// process this event
		switch( eir ) {

		   case UA4_EIR_TX_INT_PENDING:
			// if there is another character, send it
			if( _sending && _outcount > 0 ) {
				__outb( UA4_TXD, *_outnext );
				++_outnext;
				--_outcount;
			} else {
				// no more data - reset the output vars
				_outcount = 0;
				_outlast = _outnext = _outbuffer;
				_sending = 0;
				// disable TX interrupts
				_sio_disable( SIO_TX );
			}
			break;

		   case UA4_EIR_RX_INT_PENDING:
			// get the character
			ch = __inb( UA4_RXD );
			if( ch == '\r' ) {	// map CR to LF
				ch = '\n';
			}

			//
			// If there is a waiting process, this must be 
			// the first input character; give it to that
			// process and awaken the process.
			//

			if( !_q_empty(_reading) ) {

				stat = _q_remove( &_reading, (void **) &pcb, 0 );
				if( stat != STAT_SUCCESS ) {
					_kpanic( "sio isr - serial wakeup failed" );
				}
				pcb->context->eax = ch & 0xff;
				_schedule( pcb, pcb->priority );

			} else {

				//
				// Nobody waiting - add to the input buffer
				// if there is room, otherwise just ignore it.
				//

				if( _incount < BUF_SIZE ) {
					*_inlast++ = ch;
					++_incount;
				}
			
			}

			break;

		   case UA4_EIR_NO_INT:
			// nothing to do - tell the PIC we're done
			__outb( PIC_MASTER_CMD_PORT, PIC_EOI );
			return;

		   default:
		   	c_printf("eir = %x\n", eir);
			_kpanic( "sio isr - unknown device status" );

		}
	
	}

}

/*
** _sio_init()
**
** Initialize the UART chip.
*/
void _sio_init( void ) {

	/*
	** Initialize SIO variables.
	*/

	_memclr( (void *) _inbuffer, sizeof(_inbuffer) );
	_inlast = _innext = _inbuffer;
	_incount = 0;

	_memclr( (void *) _outbuffer, sizeof(_outbuffer) );
	_outlast = _outnext = _outbuffer;
	_outcount = 0;
	_sending = 0;

	/*
	** Next, initialize the UART.
	*/

	/*
	** Initialize the FIFOs
	**
	** this is a bizarre little sequence of operations
	*/

	__outb( UA4_FCR, 0x20 );
	__outb( UA4_FCR, UA5_FCR_FIFO_RESET );	// 0x00
	__outb( UA4_FCR, UA5_FCR_FIFO_EN );	// 0x01
	__outb( UA4_FCR, UA5_FCR_FIFO_EN |
			 UA5_FCR_RXSR );	// 0x03
	__outb( UA4_FCR, UA5_FCR_FIFO_EN |
			 UA5_FCR_RXSR |
			 UA5_FCR_TXSR );	// 0x07

	/*
	** disable interrupts
	**
	** note that we leave them disabled; _sio_enable() must be
	** called to switch them back on
	*/

	__outb( UA4_IER, 0 );
	_ier = 0;

	/*
	** select bank 1 and set the data rate
	*/

	__outb( UA4_LCR, UA4_LCR_BANK1 );
	__outb( UA4_LBGD_L, BAUD_LOW_BYTE( BAUD_9600 ) );
	__outb( UA4_LBGD_H, BAUD_HIGH_BYTE( BAUD_9600 ) );

	/*
	** Select bank 0, and at the same time set the LCR for our
	** data characteristics.
	*/

	__outb( UA4_LCR, UA4_LCR_BANK0 |
			 UA4_LCR_BITS_8 |
			 UA4_LCR_1_STOP_BIT |
			 UA4_LCR_NO_PARITY );
	
	/*
	** Set the ISEN bit to enable the interrupt request signal.
	*/

	__outb( UA4_MCR, UA4_MCR_ISEN | UA4_MCR_DTR | UA4_MCR_RTS );

	/*
	** Report that we're done.
	*/

	c_puts( " sio" );

}

/*
** _sio_enable()
**
** enable SIO interrupts
**
** usage:	old = _sio_enable( which )
**
** enables interrupts according to the 'which' parameter
**
** returns the prior settings
*/

uint8_t _sio_enable( uint8_t which ) {
	uint8_t old;

	// remember the current status

	old = _ier;

	// figure out what to enable

	if( which & SIO_TX ) {
		_ier |= UA4_IER_TX_INT_ENABLE;
	}

	if( which & SIO_RX ) {
		_ier |= UA4_IER_RX_INT_ENABLE;
	}

	// if there was a change, make it

	if( old != _ier ) {
		__outb( UA4_IER, _ier );
	}

	// return the prior settings

	return( old );
}

/*
** _sio_disable()
**
** disable SIO interrupts
**
** usage:	old = _sio_disable( which )
**
** disables interrupts according to the 'which' parameter
**
** returns the prior settings
*/

uint8_t _sio_disable( uint8_t which ) {
	uint8_t old;

	// remember the current status

	old = _ier;

	// figure out what to disable

	if( which & SIO_TX ) {
		_ier &= ~UA4_IER_TX_INT_ENABLE;
	}

	if( which & SIO_RX ) {
		_ier &= ~UA4_IER_RX_INT_ENABLE;
	}

	// if there was a change, make it

	if( old != _ier ) {
		__outb( UA4_IER, _ier );
	}

	// return the prior settings

	return( old );
}


/*
** _sio_read()
**
** get the next input character
**
** usage:	ch = _sio_read()
**
** returns the next character, or -1 if no character is available
*/

int _sio_read( void ) {
	int ch;

	// assume there is no character available
	ch = -1;

	// 
	// If there is a character, return it
	//

	if( _incount > 0 ) {

		// take it out of the input buffer
		ch = (int)(*_innext++) & 0xff;
		--_incount;

		// reset the buffer variables if this was the last one
		if( _incount < 1 ) {
			_inlast = _innext = _inbuffer;
		}

	}

	return( ch );

}


/*
** _sio_write( ch )
**
** write a character to the serial output
**
** usage:	_sio_write( ch )
*/

void _sio_write( int ch ){

	//
	// Must do LF -> CRLF mapping
	//

	if( ch == '\n' ) {
		_sio_write( '\r' );
	}

	//
	// If we're currently transmitting, just add this to the buffer
	//

	if( _sending ) {
		*_outlast++ = ch;
		++_outcount;
		return;
	}

	//
	// Not sending - must prime the pump
	//

	_sending = 1;
	__outb( UA4_TXD, ch );

	// Also must enable transmitter interrupts

	_sio_enable( SIO_TX );

}

/*
** _sio_dump()
**
** dump the contents of the SIO buffers
*/

void _sio_dump( void ) {
	int n;
	char *ptr;

	c_puts( "SIO buffers:  " );
	c_printf( "SIO buffers:  in %d", _incount );
	if( _incount ) {
		c_puts( " \"" );
		ptr = _innext; 
		for( n = 0; n < _incount; ++n ) 
			put_char_or_code( *ptr++ );
		c_putchar( '"' );
	}

	c_printf( " out %d", _outcount );
	if( _outcount ) {
		c_puts( " \"" );
		ptr = _outnext; 
		for( n = 0; n < _outcount; ++n ) 
			put_char_or_code( *ptr++ );
		c_putchar( '"' );
	}

	c_putchar( '\n' );

}

void _sio_puts( char *str ) {
	uint32_t count = 0, len = strlen(str);
	while( count != len ) {
		_sio_write(str[count++]);
	}
}

#define MAX_DIGITS 1024

void _sio_puti( int num ) {
	char buf[MAX_DIGITS];
	itoa(buf, MAX_DIGITS, num);
	_sio_puts(buf);
}
