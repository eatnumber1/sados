/*
** sccs id:	@(#)klibs.S	1.1	04/03/10
**
** File:	?
**
** Author:	4003-506 class of 20093
**
** Contributor:
**
** Description:	?
*/

#define	__ASM__20093__

/*
** _get_ebp - return current contents of EBP at the time of the call
**
** Primarily used only by _kpanic
*/

	.globl	_get_ebp
_get_ebp:
	movl	%ebp, %eax
	ret
