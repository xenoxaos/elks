/*******************************************************************
 * memmove function for far data. The number of bytes transferred  *
 * must be <= 32767 ( ie. fit in an signed int ). That's because   *
 * of ( you guessed it... ) the braindead segmented architecture   *
 * Saku Airila 1996                                                *
 *******************************************************************/

static void blt_back();
static void blt_forth();

void far_memmove(unsigned sseg,unsigned soff,unsigned dseg,unsigned doff,
		 int bytes)
{
    unsigned long slin, dlin;

    slin = ( ( unsigned long )sseg << 4 ) + ( unsigned long )soff;
    dlin = ( ( unsigned long )dseg << 4 ) + ( unsigned long )doff;
    if( slin < dlin ) {
	/* The idea is to get most of the ptr info in the offset part. The
	 * number of bytes to transfer must be added to the pointers because
	 * here we are copying in reverse order starting from the tail.
	 */
	slin += bytes;
	dlin += bytes;
	soff = slin & 0xFFFF;
	doff = dlin & 0xFFFF;
	sseg = ( slin >> 4 ) & 0xF000;
	dseg = ( dlin >> 4 ) & 0xF000;
	while( soff < bytes + 1 ) {
	    soff += 0x1000;
	    sseg -= 0x0100;
	}
	while( doff < bytes + 1 ) {
	    doff += 0x1000;
	    dseg -= 0x0100;
	}
	blt_forth( sseg, soff, dseg, doff, ++bytes );
    } else {
	/* And here, we want most of it in the segment part */
	soff = slin & 0x0F;
	doff = dlin & 0x0F;
	sseg = ( slin >> 4 ) & 0xFFFF;
	dseg = ( dlin >> 4 ) & 0xFFFF;
	blt_back( sseg, soff, dseg, doff, bytes );
    }
}

#asm

				! blt_back( sseg, soff, dseg, doff, bytes )
				! for to < from
	.text

	.even

_blt_back:
	push	bp
	mov	bp, sp
	push	ax
	push	es
	push	ds
	push	cx	
	push	si
	push	di
	pushf
	mov	ax, [bp+4]
	mov	ds, ax
	mov	si, [bp+6]
	mov	ax, [bp+8]
	mov	es, ax
	mov	di, [bp+10]
	mov	cx, [bp+12]
	cld
	rep
	movsb
	popf
	pop	di
	pop	si
	pop	cx
	pop	ds
	pop	es
	pop	ax
	pop	bp
	ret

				! blt_forth( sseg, soff, dseg, doff, bytes )
				! for to > from 
	.text

	.even

_blt_forth:
	push	bp
	mov	bp, sp
	push	ax
	push	es
	push	ds
	push	cx	
	push	si
	push	di
	pushf
	mov	ax, [bp+4]
	mov	ds, ax
	mov	si, [bp+6]
	mov	ax, [bp+8]
	mov	es, ax
	mov	di, [bp+10]
	mov	cx, [bp+12]
	std
	rep
	movsb
	popf
	pop	di
	pop	si
	pop	cx
	pop	ds
	pop	es
	pop	ax
	pop	bp
	ret

#endasm
