/*
** SCCS ID:	@(#)assert.h	1.2	04/13/10
**
** File:	assert
**
** Author:	4003-506 class of 20013
**
** Contributor:
**
** Description:	Kernel assertions.
*/

#ifndef	_ASSERT_H
#define	_ASSERT_H

#include "headers.h"

/*
** General (C and/or assembly) definitions
*/

#ifndef __ASM__20093__

/*
** Start of C-only definitions
*/

#define	assert(cond,reason)	\
	if( ! (cond) ){ \
		c_printf( "\nAssertion \"%s\" failed, file %s line %d\n", \
		    #cond, __FILE__, __LINE__ ); \
		__panic( reason ); \
	}

#endif

#endif
