/*
** SCCS ID:	@(#)syscalls.h	1.1	04/03/10
**
** File:	syscalls.h
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	system call handlers
*/

#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include "headers.h"

/*
** General (C and/or assembly) definitions
*/

#include <x86arch.h>

// system call codes

#define	SYS_exit		0
#define	SYS_fork		1
#define	SYS_exec		2
#define	SYS_wait		3
#define	SYS_sleep		4
#define	SYS_read		5
#define	SYS_write		6
#define	SYS_getpid		7
#define	SYS_getppid		8
#define	SYS_getprio		9
#define	SYS_setprio		10
#define	SYS_getticks		11
#define SYS_relfs_mkfs		12
#define SYS_relfs_open		13
#define SYS_relfs_alloc		14
#define SYS_relfs_close		15
#define SYS_relfs_free		16
#define SYS_relfs_write		17
#define SYS_relfs_read		18
#define SYS_relfs_dump		19
#define SYS_relfs_unlink	20
#define SYS_relfs_retrieve	21

// number of "real" system calls

#define	N_SYSCALLS		22

// dummy system call code to test the syscall ISR

#define	SYS_bogus		0xbadc0de

// system call interrupt vector

#define	INT_VEC_SYSCALL		0x80

// default contents of user EFLAGS register

#define	DEFAULT_EFLAGS		(EFLAGS_MB1 | EFLAGS_IF)

#ifndef __ASM__20093__

/*
** Start of C-only definitions
*/

/*
** Prototypes
*/

/*
** _isr_syscall()
**
** system call ISR
*/

void _isr_syscall( int vector, int code );

/*
** _syscall_init()
**
** initializes all syscall-related data structures
*/

void _syscall_init( void );

#endif

#endif
