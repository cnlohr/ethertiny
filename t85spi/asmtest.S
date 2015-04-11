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

#include "eth_config.h"
#include <avr/io.h>

#define USEBR  USIDR
.global SendNLP
SendNLP:

#ifdef USES_EXTERNAL_DRIVER

	sbi PORTB, 1
	sbi PORTB, 2
#ifdef LONGNLP
	nop  //+40ns
#endif
	cbi PORTB, 2

#else //Doesn't use external driver.

#if defined( INVERTTX ) || defined( INVERTNLP )
	cbi PORTB, 1
#else
	sbi PORTB, 1
#endif
#ifdef SMARTDDR
	sbi DDRB, 1
#endif
	nop  //Makes it add to around 80ns if running with high clock rate.
#ifdef LONGNLP 
	nop  //+40ns
#endif

#ifdef SMARTDDR
	cbi DDRB, 1
#endif

#ifdef BIAS0
	cbi PORTB, 1
#endif

#endif
	ret

//

//Handles sending preamble + Packet ovr the USI
//You must set the clock to 20 MHz well in advance of calling
//this function to allow it to settle.
//You also must already have interrupts turned off.
.global SendTestASM  // ( unsigned char * ptr_to_packet, uint8_t packet_length_longs ); (longs = packet_length / 4)
SendTestASM:
	push r16
	push r17
	push r18
	push r19
	push r20
	push r21
	push r26
	push r27
	push r29
	push r30
	push r31

	//Protect the interrupt state
//	in r15, SREG
//	cli
	//Sadly, we can't protect the clock, we've just gotta be at 20 MHz :(
/*	in r14, OSCCAL
	ldi r27, OSC20
	out OSCCAL, r27*/

#ifdef USES_EXTERNAL_DRIVER
	sbi PORTB, 2
#else
#ifdef SMARTDDR
	in r21, DDRB
	ori r21, 2
#endif
#endif
	
	ldi r27,0
	ldi r26,0
	subi r26,lo8(-(ManchesterTable))
	sbci r27,hi8(-(ManchesterTable))
	//Manchester table now in X.

	//Move "Packet to send" into Z (R30, 31)
	movw r30,r24

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

	//Make sure we start with 0's.
#ifdef INVERTTX
	ldi r19, 0x05
#else
	ldi r19, 0x0a
#endif
	

//////////////////////////////////////////////////
//Special: preamble.
	ldi r20, 14   //Iterate 15 times. (the 0x5 pattern) then finish out with the 0xD pattern.
		//We put 14 here, because we will iterate one extra time beforehand, just in case with smartddr.
#ifdef INVERTTX
	ldi r19, 0b10011001  //Standard 0x55 pattern
	ldi r17, 0b10011010  //Final pattern marker.
#else
	ldi r19, 0b01100110  //Standard 0x55 pattern
	ldi r17, 0b01100101  //Final pattern marker.
#endif
	out USEBR, r19

	//Go ahead with the preamble.
	//Enable the USI
	SBI USICR, USIWM0

#if defined( SMARTDDR ) && !defined( USES_EXTERNAL_DRIVER )
	nop
	out DDRB, r21
	nop
	nop
	nop
#else
	nop
	nop
	nop
	nop
	nop
#endif

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
	andi r19,0 //Cleanup.
	nop
	out USEBR,r19
#ifdef USES_EXTERNAL_DRIVER
	cbi PORTB, 2
#elif defined( SMARTDDR )
	cbi DDRB, 1
#endif
	CBI USICR, USIWM0

//	out OSCCAL, r14
//	out SREG, r15

//	pop r14
//	pop r15
	pop r31
	pop r30
	pop r29
	pop r27
	pop r26
	pop r21
	pop r20
	pop r19
	pop r18
	pop r17
	mov r24, r16
	pop r16
	ret


#define USEBRR  USIDR

.global MaybeHaveDataASM
MaybeHaveDataASM:

	push r16
	push r22
	push r18
	push r19
	push r20

					//r22 = words in buffer.
	movw r30,r24    //r24:r25 = pointer to staging ram.

	//In here, we're operating at 30+MHz (hopefully)
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









