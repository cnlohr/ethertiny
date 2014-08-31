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

#define USEBR  USIDR
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


//Handles sending preamble + Packet ovr the USI
.global SendTestASM  // ( unsigned char * ptr_to_packet, uint8_t packet_length_longs ); (longs = packet_length / 4)
SendTestASM:
	push r16
	push r17
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
	ld r18,Z+
	mov r26, r18 
	andi r26, 0x0f
	subi r26,lo8(-(ManchesterTable)) //1  (This sets X up)
SendLoop:
	tst r16
	brne .SkipD
	rjmp .Done
.SkipD:


//////////////////////////////////////////////////
//Special: preamble.
	ldi r20, 15   //Iterate 15 times. (the 0x55 pattern) then finish out with the 0xD5 pattern.
	ldi r19, 0b01100110  //Standard 0x55 pattern
	ldi r17, 0b01100101  //Final pattern marker.
	//Go ahead with the preamble.
	//Enable the USI
	SBI USICR, USIWM0

.PreambleLoop:
	out USEBR, r19
	nop
	nop
	nop
	nop
	dec r20
	brne .PreambleLoop
    nop
	out USEBR, r17
	nop

	nop
	nop

KeepGoing:			///subi r26,lo8(-(ManchesterTable))

	//Heavily interleved code.  Need a new OUTB every 8 cycles exactly.
    nop           //1 //Look, ma ONE EXTRA CYCLE!
	//nop

	ld r19, X      //2
	swap r18       //1
	out USEBR, r19 //1  START HERE
	mov r26, r18   //1
	andi r26, 0x0f //1
	subi r26,lo8(-(ManchesterTable)) //1
	ld r19, X      //2
	ld r18,Z+      //2  //Read off the bytes.
	out USEBR, r19 //1 ** OK
	mov r26, r18   //1


	andi r26, 0x0f //1
	subi r26,lo8(-(ManchesterTable)) //1
	ld r19, X      //2
	swap r18       //1
	mov r26, r18   //1
	out USEBR, r19 //1  ** OK
	andi r26, 0x0f //1
	subi r26,lo8(-(ManchesterTable)) //1
	ld r19, X      //2
	ld r18,Z+      //2  //Read off the bytes.
	mov r26, r18   //1
	out USEBR, r19 //1  **OK


	andi r26, 0x0f //1
	subi r26,lo8(-(ManchesterTable)) //1
	ld r19, X      //2
	swap r18       //1
	mov r26, r18   //1
	andi r26, 0x0f //1
	out USEBR, r19 //1  ** OK 
	subi r26,lo8(-(ManchesterTable)) //1
	ld r19, X      //2
	ld r18,Z+      //2  //Read off the bytes.
	mov r26, r18   //1
	andi r26, 0x0f //1
	out USEBR, r19 //1 ** OK
	subi r26,lo8(-(ManchesterTable)) //1
	ld r19, X      //2
	swap r18       //1
	mov r26, r18   //1
	andi r26, 0x0f //1
	subi r26,lo8(-(ManchesterTable)) //1
	out USEBR, r19 //1 ** OK
	ld r19, X      //2
	ld r18,Z+      //2  //Read off the bytes.
	mov r26, r18   //1
	andi r26, 0x0f //1
	subi r26,lo8(-(ManchesterTable)) //1
	out USEBR, r19 //1 ** OK

	dec r16         //1
	brne KeepGoing  //2
.Done:
	nop
	nop
	nop
	nop
	andi r19,0
	nop
	out USEBR,r19 
	CBI USICR, USIWM0

	pop r20
	pop r19
	pop r18
	pop r17
	mov r24, r16
	pop r16
	ret


#define USEBRR  USIDR
//#define USEBRR  ADCL

.global MaybeHaveDataASM
MaybeHaveDataASM:

	push r16
	push r22
	push r18
	push r19
	push r20

					//r22 = words in buffer.
	movw r30,r24    //r24:r25 = pointer to staging ram.

	//In here, we're operating at 30 ~MHz (hopefully)
	//We want to peel the data off the SPI bus as fast as possible!

	tst r22
	breq rlend
rloop:

///XXX TODO: Scrutenize the "breq" 
	in r19,USEBRR  //1  ****
	inc r19			//1
    breq rlend    //2???
    nop           //1????
	dec r19		//1
	breq rlend  //2????
	nop         //1
	nop         //1

	in r18,USEBRR  //1  ****
	st Z+, r19    //2
	st Z+, r18    //2
	dec r22       //1
	brne rloop    //2

rlend:
	mov r24,r22
	pop r20
	pop r19
	pop r18
	pop r22
	pop r16
	ret









