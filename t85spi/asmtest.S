/*
    Copyright (C) 2014 <>< Charles Lohr


    Permission is hereby granted, free of charge, to any person obtaining a
	copy of this software and associated documentation files (the "Software"),
	to deal in the Software without restriction, including without limitation
	the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
	in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/



#define _SFR_ASM_COMPAT 1
#define __SFR_OFFSET 0

#include <avr/io.h>

.global SendTick
SendTick:
	ori r18, 0b00010101
	ori r19, 0b00001101
	ori r20, 0b00000101
	out PORTB, r19
	out PORTB, r19
	out PORTB, r18
	out PORTB, r18
	out PORTB, r20
	ret

.global SendTestASM  // ( unsigned char * ptr_to_packet, uint8_t packet_length_longs ); (longs = packet_length / 4)
SendTestASM:
	push r16
	push r18
	push r19
	push r20
	ldi r27,0
	ldi r26,0
	subi r26,lo8(-(ManchesterTable))
	sbci r27,hi8(-(ManchesterTable))
//	mov r17, r26 // For backup
	  //Manchester table now in X.

	//Move "Packet to send" into Z (R30, 31)
	movw r30,r24
//	add r22,r24
//	adc r23,r25

	//r22 = how big to send.
	
	mov r16, r22
	ld r18,Z+      //2  //Read off the bytes.
	mov r26, r18   //1  R26 = To be read Manchester lookup
	andi r26, 0x0f //1
	subi r26,lo8(-(ManchesterTable)) //1
SendLoop:
	tst r16
	breq Done
KeepGoing:			///subi r26,lo8(-(ManchesterTable))

	//Heavily interleved code.  Need a new OUTB every 8 cycles exactly.
    nop           //1 //Look, ma ONE EXTRA CYCLE!
	//nop

	ld r19, X      //2
	swap r18       //1
	out USIDR, r19 //1  START HERE
	mov r26, r18   //1
	andi r26, 0x0f //1
	subi r26,lo8(-(ManchesterTable)) //1
	ld r19, X      //2
	ld r18,Z+      //2  //Read off the bytes.
	out USIDR, r19 //1 ** OK
	mov r26, r18   //1


	andi r26, 0x0f //1
	subi r26,lo8(-(ManchesterTable)) //1
	ld r19, X      //2
	swap r18       //1
	mov r26, r18   //1
	out USIDR, r19 //1  ** OK
	andi r26, 0x0f //1
	subi r26,lo8(-(ManchesterTable)) //1
	ld r19, X      //2
	ld r18,Z+      //2  //Read off the bytes.
	mov r26, r18   //1
	out USIDR, r19 //1  **OK


	andi r26, 0x0f //1
	subi r26,lo8(-(ManchesterTable)) //1
	ld r19, X      //2
	swap r18       //1
	mov r26, r18   //1
	andi r26, 0x0f //1
	out USIDR, r19 //1  ** OK 
	subi r26,lo8(-(ManchesterTable)) //1
	ld r19, X      //2
	ld r18,Z+      //2  //Read off the bytes.
	mov r26, r18   //1
	andi r26, 0x0f //1
	out USIDR, r19 //1 ** OK
	subi r26,lo8(-(ManchesterTable)) //1
	ld r19, X      //2
	swap r18       //1
	mov r26, r18   //1
	andi r26, 0x0f //1
	subi r26,lo8(-(ManchesterTable)) //1
	out USIDR, r19 //1 ** OK
	ld r19, X      //2
	ld r18,Z+      //2  //Read off the bytes.
	mov r26, r18   //1
	andi r26, 0x0f //1
	subi r26,lo8(-(ManchesterTable)) //1
	out USIDR, r19 //1 ** OK

	dec r16         //1
	brne KeepGoing  //2
Done:
	pop r20
	pop r19
	pop r18
	pop r16
	ret



.global MaybeHaveDataASM
MaybeHaveDataASM:


	push r16
	push r18
	push r19
	push r20

					//r22:r23 = bytes in buffer.
	movw r30,r24    //r24:r25 = pointer to staging ram.

	//In here, we're operating at 30 ~MHz (hopefully)
	//We want to peel the data off the SPI bus as fast as possible!

	//First, let's make sure it wasn't just a glitch.
	in r18,USIBR  
	tst r18
	breq rlend


	tst r22
	breq rlend
rloop:
	in r18,USIDR  //1  ****
	st Z+, r18    //2
//	tst r18       //1
	inc r18			//1
    breq rlend    //2
    nop           //1
    nop           //1
	in r18,USIDR  //1  ****
	st Z+, r18    //2
	dec r22       //1
    nop           //
    nop           //1
	brne rloop    //2

rlend:
	pop r20
	pop r19
	pop r18
	pop r16
	ret









