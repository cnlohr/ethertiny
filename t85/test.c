/*

	WARNING: Connecting an AVR to ethernet without magnetics is VERY DANGEROUS.
	It may cause high voltages to go through the AVR and destroy it in DANGEROUS
	that may harm personal safety.  Do only at your own risk.

*/

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


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

void delay_ms(uint32_t time) {
  uint32_t i;
  for (i = 0; i < time; i++) {
    _delay_ms(1);
  }
}

#define NOOP asm volatile("nop" ::)

void SendTick();
void SendAPacket();
void SendBPacket();
void SendCPacket();

static void setup_clock( void )
{
	/*Examine Page 33*/

	CLKPR = 0x80;	/*Setup CLKPCE to be receptive*/
	CLKPR = 0x00;	/*No scalar*/

	PLLCSR = _BV(PLLE) | _BV( PCKE );
	PLLCSR |= _BV(LSM);
		//0xB7 to 0xC0 permissable 0xBB in middle.
//	OSCCAL = 0xbc;
	OSCCAL = 0xbb;

}


int main( )
{
	int i;
	unsigned char wasa, wasb, isa, isb;
	cli();

	setup_clock();

	DDRB = 0;
	DDRB |= _BV(3) | _BV(4) | _BV(1);
	PORTB &= ~_BV(1);

	wasa = 0;
	wasb = 0;

	int suppress = 10;

	for( ;; )
	{
		_delay_ms(8);
		SendTick();
		_delay_ms(8);

//			SendCPacket();
//continue;
		isa = !(PINB & _BV(0));
		isb = !(PINB & _BV(2));

		i++;


/*		if( !suppress )
		{
			if( isa != wasa )
			{
				if( isa )
					SendAPacket();
				wasa = isa;
				continue;
			}
			if( isb != wasb )
			{
				if( isb )
					SendBPacket();
				wasb = isb;
				continue;
			}
		}*/

		if( i == 1 )
		{
			if( isa )
				SendAPacket();
		}
		if( i == 2 )
		{
			if( isb )
				SendBPacket();
		}

		if( i == 3 )
		{
			SendCPacket();
			if( suppress ) suppress--;
			i = 0;
		}
	}

	
	return 0;
} 
