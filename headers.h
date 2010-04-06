/*
** SCCS ID:	@(#)headers.h	1.1	04/03/10
**
** File:	headers.h
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	standard includes, pulled in by everything else
*/

#ifndef _HEADERS_H
#define _HEADERS_H

#ifndef __ASM__20093__

// only pull these in if we're not in assembly language

#include "defs.h"
#include "types.h"
#include "c_io.h"
#include "support.h"

// OS and user code have different library headers

#ifdef __KERNEL__20093__
#include "klib.h"
#else
#include "ulib.h"
#endif

#endif

#endif
