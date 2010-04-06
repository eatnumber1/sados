/*
** SCCS ID:	@(#)klib.h	1.1	04/03/10
**
** File:	klib.h
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	Prototypes for kernel-level library routines
**
** Note that some of these are defined in the klibs.S source file,
** and others in the klibc.c source file.
*/

#ifndef _KLIB_H
#define _KLIB_H

#ifndef __ASM__20093__

#include "types.h"

/*
** Start of C-only definitions
*/

/*
** Definitions
*/

/*
** Prototypes
*/

/*
** put_char_or_code( ch )
**
** prints the character on the console, unless it is a non-printing
** character, in which case its hex code is printed
*/

void put_char_or_code( int ch );

/*
** _get_ebp - return current contents of EBP at the time of the call
**
** Could be used, e.g., by _kpanic() to print a traceback
*/

uint32_t _get_ebp( void );

/*
** _memclr - initialize all bytes of a block of memory to zero
**
** usage:  _memclr( buffer, length )
*/

void _memclr( void *buffer, uint32_t length );

/*
** _memcpy - copy a block from one place to another
**
** usage:  _memcpy( dest, src, length )
**
** may not correctly deal with overlapping buffers
*/

void _memcpy( void *destination, void *source, uint32_t length );

/*
** _kpanic - kernel-level panic routine
*/

void _kpanic( char *msg );

/*
** _kprt_status - print a status value to the console
**
** the 'msg' argument should contain a %s where
** the desired status value should be printed
*/

void _kprt_status( char *msg, int stat );

#endif

#endif
