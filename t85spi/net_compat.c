#include "net_compat.h"
#include <avr/pgmspace.h>
#include <string.h>

#define NOOP asm volatile("nop" ::)


unsigned char MyMAC[6];
unsigned char ETbuffer[ETBUFFERSIZE+PREAMBLE];
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

//From the ASM file.
void SendTestASM( const unsigned char * c, uint8_t len );
int MaybeHaveDataASM( unsigned char * c, uint8_t lenX2 ); //returns the number of pairs.

//Attempt to return rough estimate of processing time.
int GotPack( unsigned char * machesterized, int estlen, uint16_t mlen )
{
	int byr = 0;

	byr = Demanchestrate( machesterized, mlen );

	//Don't do anything yet...
	//XXX TODO THIS will be some good stuff in here.

	return byr;
}

void waitforpacket( unsigned char * buffer, uint16_t len, int16_t ltime )
{
	OSCCAL = OSCHIGH;

	//Make sure we're not walking in on something.
	while( ltime-- > 0 )
	{
		if( USIBR == 0x00 ) break;
		if( USIBR == 0xFF ) break;
		NOOP;
	}


	while( ltime-- > 0 )
	{
		if( USIBR && (USIBR != 0xFF ) )
		{
			int r = MaybeHaveDataASM( buffer, len );
			if( r > 1 )
			{
				r += GotPack( buffer, r, len );
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
	PLLCSR |= _BV(LSM);
	OSCCAL = OSC20;


	//Setup timer 0 to speed along.
	//Yes, this means you can't use it.
	TCCR0A = _BV(WGM01);
	TCCR0B = _BV(CS00);
	OCR0A = 0;

	USICR = _BV(USIWM0) | _BV(USICS0) | _BV(USITC);

	//setup port B

	PORTB &= ~_BV(0); 
	DDRB &= ~_BV(0);
	PORTB &= ~_BV(1);
	USICR &= ~_BV(USIWM0);  //Disable USICR

	return 0;
}

int8_t et_xmitpacket( uint16_t start, uint16_t len )
{
	//XXX !!! TODO
}

//This waits for 8ms, sends an autoneg notice, then waits for 8 more ms.
unsigned short et_recvpack()
{

#define LIMITSIZE  sizeof( ETbuffer )/2-30
//#define LIMITSIZE 10

		waitforpacket(&ETbuffer[PREAMBLE+40], LIMITSIZE, 20000); //wait for 2048 cycles (30MHz/8 = 3.75MHz / 30000 = 8ms)
//		_delay_ms(8);
#ifdef SMARTPWR
		DDRB |= _BV(1);
#endif
		PORTB|=_BV(1);
		NOOP;
		PORTB &=~_BV(1);
#ifdef SMARTPWR
		DDRB &= ~_BV(1);
#endif
		waitforpacket(&ETbuffer[PREAMBLE+40], LIMITSIZE, 20000); //wait for 2048  (30MHz/8 = 3.75MHz / 30000 = 8ms)
// 		_delay_ms(8);

}

void et_start_checksum( uint16_t start, uint16_t len )
{
//	ETchecksum = uint16_t internet_checksum( &ETbuffer[start], uint16_t len );
	//XXX !!! TODO
}

void et_copy_memory( uint16_t to, uint16_t from, uint16_t length, uint16_t range_start, uint16_t range_end )
{
	//XXX !!! TODO
}


