#include "net_compat.h"
#include <avr/pgmspace.h>
#include <string.h>
#include "hlprocess.h"
#include "packetmater.h"

#define NOOP asm volatile("nop" ::)



unsigned char ETbuffer[ETBUFFERSIZE];
unsigned short ETsendplace;
uint16_t sendbaseaddress;
unsigned short ETchecksum;


//Do not split this across byte-addressing boundaries.
//We do some fancy stuff when we send the manchester out.
char ManchesterTable[16] __attribute__ ((aligned (16))) = {
	0b10101010, 0b01101010, 0b10011010, 0b01011010,
	0b10100110, 0b01100110, 0b10010110, 0b01010110,
	0b10101001, 0b01101001, 0b10011001, 0b01011001,
	0b10100101, 0b01100101, 0b10010101, 0b01010101,
};



//Internal functs

uint32_t crc32b(uint32_t crc, unsigned char *message, int len) {
   int i, j;
   uint32_t mask;
	uint8_t byte;

   i = 0;
//   crc = 0xFFFFFFFF;
	crc = ~crc;
   while (i < len) {
      byte = message[i];            // Get next byte.
      crc = crc ^ byte;
      for (j = 7; j >= 0; j--) {    // Do eight times.
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
      }
      i = i + 1;
   }
   return ~crc;
}

uint16_t internet_checksum( const unsigned char * start, uint16_t len )
{
	uint16_t i;
	const uint16_t * wptr = (uint16_t*) start;
	uint32_t csum = 0;
	for (i=1;i<len;i+=2)
	{
		csum = csum + (uint32_t)(*(wptr++));	
	}
	if( len & 1 )  //See if there's an odd number of bytes?
	{
		uint8_t * tt = (uint8_t*)wptr;
		csum += *tt;
	}
	while (csum>>16)
		csum = (csum & 0xFFFF)+(csum >> 16);
	csum = (csum>>8) | ((csum&0xff)<<8);
	return ~csum;
}



//From the ASM file.
void SendTestASM( const unsigned char * c, uint8_t len );
int MaybeHaveDataASM( unsigned char * c, uint8_t lenX2 ); //returns the number of pairs.

/*
//Attempt to return rough estimate of processing time.
int GotPack( unsigned char * machesterized, int estlen, uint16_t mlen )
{
	int byr = 0;

	byr = Demanchestrate( machesterized, mlen );
	et_receivecallback( 

	//Don't do anything yet...
	//XXX TODO THIS will be some good stuff in here.

	return byr;
}
*/

void waitforpacket( unsigned char * buffer, uint16_t len, int16_t ltime )
{
	OSCCAL = OSCHIGH;

	//Make sure we're not walking in on something.
	//Not sure if we should do this or not :-/
/*	while( ltime-- > 0 )
	{
		if( USIBR == 0x00 ) break;
		if( USIBR == 0xFF ) break;
		NOOP;
	}
*/

	while( ltime-- > 0 )
	{
		if( USIBR && (USIBR != 0xFF ) )
		{
			int r = MaybeHaveDataASM( buffer, len );
			if( r > 10 )
			{
				buffer[ETBUFFERSIZE-1] = 0; //Force an end-of-packet.
				int16_t byr = Demanchestrate( buffer, r );
				if( byr > 32 )
				{
					uint32_t cmpcrc = crc32b( 0, buffer, byr - 4 );
					OSCCAL = OSC20; //Drop back to 20 MHz if we need to send.
					if( cmpcrc == *((uint32_t*)&buffer[byr-4]) )
					{
						//LRP( "%d->%d\n", r, byr );

						//If you ever get to this code, a miracle has happened.
						//Pray that many more continue.
						ETsendplace = 0;
						et_receivecallback( byr - 4 );
					}
					OSCCAL = OSCHIGH;
				}
			}
			ltime-=(len-r)*4+3; //About how long the function takes to execute.
			break;
		}
	}


	while( ltime-- > 0 )
	{
		NOOP;
		NOOP;
		NOOP;
		NOOP;
		NOOP;
	}

	OSCCAL = OSC20;
}









//
uint16_t et_pop16()
{
	uint16_t ret = et_pop8();
	return (ret<<8) | et_pop8();
}

void et_popblob( uint8_t * data, uint8_t len )
{
	while( len-- )
	{
		*(data++) = et_pop8();
	}
}

void et_pushpgmstr( const char * msg )
{
	uint8_t r;
	do
	{
		r = pgm_read_byte(msg++);
		if( !r ) break;
		et_push8( r );
	} while( 1 );
}

void et_pushpgmblob( const uint8_t * data, uint8_t len )
{
	while( len-- )
	{
		et_push8( pgm_read_byte(data++) );
	}
}


void et_pushstr( const char * msg )
{
	for( ; *msg; msg++ ) 
		et_push8( *msg );
}

void et_pushblob( const uint8_t * data, uint8_t len )
{
	while( len-- )
	{
		et_push8( *(data++) );
	}
}

void et_push16( uint16_t p )
{
	et_push8( p>>8 );
	et_push8( p&0xff );
}


//return 0 if OK, otherwise nonzero.
int8_t et_init( const unsigned char * macaddy )
{
	MyMAC[0] = macaddy[0];
	MyMAC[1] = macaddy[1];
	MyMAC[2] = macaddy[2];
	MyMAC[3] = macaddy[3];
	MyMAC[4] = macaddy[4];
	MyMAC[5] = macaddy[5];

	PLLCSR = _BV(PLLE) | _BV( PCKE );
	PLLCSR |= _BV(LSM); //Should have no effect.
	OSCCAL = OSC20;


	//Setup timer 0 to speed along.
	//It will become useless for any other uses.
	TCCR0A = _BV(WGM01); //CTC mode.
	TCCR0B = _BV(CS00);  //Use system clock with no divisor.
	OCR0A = 0;

	USICR = _BV(USIWM0) | _BV(USICS0) | _BV(USITC);

	//setup port B
	PORTB &= ~_BV(0); 
	DDRB &= ~_BV(0);
	PORTB &= ~_BV(1);
	USICR &= ~_BV(USIWM0);  //Disable USICR

	DDRB |= _BV(1);

	//Optional, changes bias.
	PORTB |= _BV(0);

	return 0;
}

int8_t et_xmitpacket( uint16_t start, uint16_t len )
{
	//If we're here, ETbuffer[start] points to the first byte (dst MAC address)
	//Gotta calculate the checksum.

	//First, round up the length and make sure it meets minimum requirements.
	if( len < 60 ) len = 60;
	len = ((len-1) & 0xfffc) + 4; //round up to 4.

	uint8_t  * buffer = &ETbuffer[start];
	uint32_t crc = crc32b( 0, buffer, len );
	uint16_t i = start + len;
	
	buffer[i++] = crc & 0xff;
	buffer[i++] = (crc>>8) & 0xff;
	buffer[i++] = (crc>>16) & 0xff;
	buffer[i++] = (crc>>24) & 0xff;

	//Actually emit the packet.
	SendTestASM( buffer, (len>>2) + 1 ); //Don't forget the CRC!

	return 0;
}

//This waits for 8ms, sends an autoneg notice, then waits for 8 more ms.
unsigned short et_recvpack()
{

#define LIMITSIZE  sizeof( ETbuffer )/2-30
//#define LIMITSIZE 10

		waitforpacket(&ETbuffer[0], LIMITSIZE, 20000); //wait for 2048 cycles (30MHz/8 = 3.75MHz / 30000 = 8ms)
//		_delay_ms(8);
#ifdef SMARTPWR
		DDRB |= _BV(1);
#endif
		PORTB|=_BV(1);
		NOOP;
		NOOP;
		PORTB &=~_BV(1);
#ifdef SMARTPWR
		DDRB &= ~_BV(1);
#endif
		waitforpacket(&ETbuffer[0], LIMITSIZE, 20000); //wait for 2048  (30MHz/8 = 3.75MHz / 30000 = 8ms)
// 		_delay_ms(8);

	return 0;
}

void et_start_checksum( uint16_t start, uint16_t len )
{
	uint16_t i;
	const uint16_t * wptr = (uint16_t*)&ETbuffer[start];
	uint32_t csum = 0;
	for (i=1;i<len;i+=2)
	{
		csum = csum + (uint32_t)(*(wptr++));	
	}
	if( len & 1 )  //See if there's an odd number of bytes?
	{
		uint8_t * tt = (uint8_t*)wptr;
		csum += *tt;
	}
	while (csum>>16)
		csum = (csum & 0xFFFF)+(csum >> 16);
	csum = (csum>>8) | ((csum&0xff)<<8);
	ETchecksum = ~csum;
}



void et_copy_memory( uint16_t to, uint16_t from, uint16_t length, uint16_t range_start, uint16_t range_end )
{
	uint16_t i;
	if( to == from )
	{
		return;
	}
	else if( to < from )
	{
		for( i = 0; i < length; i++ )
		{
			ETbuffer[to++] = ETbuffer[from++];
		}
	}
	else
	{
		to += length;
		from += length;
		for( i = 0; i < length; i++ )
		{
			ETbuffer[to--] = ETbuffer[from--];
		}
	}
}

void et_write_ctrl_reg16( uint8_t addy, uint16_t value )
{
	switch (addy )
	{
		case EERXRDPTL:
		case EEGPWRPTL:
			ETsendplace = value;
		default:
			break;
	}
}

uint16_t et_read_ctrl_reg16( uint8_t addy )
{
	switch( addy )
	{
		case EERXRDPTL:
		case EEGPWRPTL:
			return ETsendplace;
		default:
			return 0;
	}
}



/*
     Modification of "avrcraft" IP Stack.
    Copyright (C) 2014 <>< Charles Lohr
		CRC Code from: http://www.hackersdelight.org/hdcodetxt/crc.c.txt

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
