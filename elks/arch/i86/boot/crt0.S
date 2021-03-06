// enable the autoconfig tool

#define __ASSEMBLY__

#include <linuxmt/config.h>
#include <arch/asm-offsets.h>

//	Assembler boot strap hooks. This is called by setup

	.code16
	.text

	.global _start
	.extern	start_kernel
	.global early_printk

_start:

/*
! Setup.S already initialized DS and ES (but not SS)
! In addition, registers contain:
!   BX, Text size
!   DI  Far text size
!   SI, Data size
!   DX, BSS size
*/
	mov	%bx,_endtext
	mov	%di,_endftext
	mov	%si,_enddata
	add	%dx,%si
	mov	%si,_endbss

// Start cleaning BSS. Still using setup.S stack

	mov	_enddata,%di	// start of BSS
	mov	%dx,%cx       // CX = BSS size
	xor	%ax,%ax
	shr	$1,%cx
	cld
	rep
	stosw

// End cleaning BSS

	mov	%cs,kernel_cs
	mov	%ds,kernel_ds

// Set SS:SP to task[0] kernel stack area

	mov	%ds,%ax       // in ROMCODE stack is ready placed
	mov	%ax,%ss       // SS=ES=DS
	mov	$task + TASK_USER_AX,%sp

	call	start_kernel	// Break point if it returns
	int	$3

early_printk:
	push  %bp
	mov   %sp,%bp
	push  %si
	mov   4(%bp),%si
	cld

ep_loop:
	lodsb
	or    %al,%al
	jz    ep_end
	mov   $0x0E,%ah
	mov   $0x0007,%bx
	int   $0x10
	jmp   ep_loop

ep_end:
	pop   %si
	pop   %bp
	ret

//	Segment beginnings

// Zero for NULL pointers (near and far)
// Will be linked as first section in data segment
	.section .nildata
	.word 	0
	.word	0

	.data
	.global _endtext
	.global _endftext
	.global _enddata
	.global _endbss
	.extern	kernel_cs
	.extern	kernel_ds
	.extern task

_endtext:
	.word	0

_endftext:
	.word	0

_enddata:
	.word	0

_endbss:
	.word	0
	.bss

_sbss:
