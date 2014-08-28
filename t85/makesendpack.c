#include <stdio.h>
#include <string.h>
#include <zlib.h>
#include <stdint.h>
#include "parsehelp.h"

void Emit( unsigned char hv )
{

	printf( "\t // %02x\n", hv );

	int bit = 0x01;
	for( ; bit & 0xff; bit<<=1 )
	{
		if( hv & bit )
		{
			printf( "\tout PORTB, r18\n" );
			printf( "\tout PORTB, r19\n" );
		}
		else
		{
			printf( "\tout PORTB, r19\n" );
			printf( "\tout PORTB, r18\n" );
		}
	}
	printf( "\n" );
}

unsigned char sendbuf[8192];
int sbp;

uint16_t csrange( uint16_t start, uint16_t len )
{
	uint16_t i;
	uint16_t * wptr = (uint16_t*)(sendbuf + start);
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

void EndEmit()
{
	int i;
	//Alter the headers to fix up any checksum issues.

	if( sendbuf[12] == 0x08 && sendbuf[13] == 0x00 )
	{
		fprintf( stderr, "IP Packet Doing checksum\n" );

		fprintf( stderr, "Alter length to be: %d\n", sbp-14 );
		uint16_t fullpacklen = sbp-14;
		sendbuf[16] = fullpacklen>>8;
		sendbuf[17] = fullpacklen&0xff;
		sendbuf[24] = 0;
		sendbuf[25] = 0;
		uint16_t csumr;
		fprintf( stderr, "Header checksum: 0x%04x\n", csumr = csrange(14,20) );
		sendbuf[24] = csumr>>8;
		sendbuf[25] = csumr&0xff;

		if( sendbuf[23] == 0x11  )
		{
			uint16_t udppacklen = fullpacklen-20;
			sendbuf[32+6] = udppacklen>>8;
			sendbuf[32+7] = udppacklen&0xff;
			fprintf( stderr, "UDP Packet.  Doing that checksum, too. Len: 0x%04x\n", udppacklen );
			uint16_t pseudo = 0x11 + 0x8 + udppacklen-8; //UDP number + size + length (of packet)
			sendbuf[40+0] = pseudo>>8;
			sendbuf[40+1] = pseudo&0xff;
			fprintf( stderr, "UDP checksum: 0x%04x\n", csumr = csrange(26,fullpacklen-8) );
			if( csumr == 0 ) csumr = 0xffff;
			sendbuf[40+0] = csumr>>8;
			sendbuf[40+1] = csumr&0xff;
		}
/*
//	enc424j600_alter_word( 34+6, 0x11 + 0x8 + length-42 ); //UDP number + size + length (of packet)
		//XXX I THINK 

	enc424j600_start_checksum( 20+6, length - 42 + 16 ); //sta

	ppl2 = enc424j600_get_checksum();
	if( ppl2 == 0 ) ppl2 = 0xffff;

	enc424j600_alter_word( 34+6, ppl2 );
*/
	}


	uint32_t crc = crc32( 0L, Z_NULL, 0 );
	crc = crc32( crc, sendbuf, sbp );
	fprintf( stderr, "Checksum: %08x (%d)\n", crc, sbp );

	for( i = 0; i < sbp; i++ )
	{
//		fprintf( stderr, "%02x ", sendbuf[i] );
		Emit( sendbuf[i] );
	}

	Emit( crc&0xff );
	Emit( (crc>>8)&0xff );
	Emit( (crc>>16)&0xff );
	Emit( (crc>>24)&0xff );

	for( i = sbp; i < 60; i++ )
	{
		Emit( 0x00 );
	}

	sbp = 0;

	printf( "\tout PORTB, r20\n\tret\n\n" );
}

int main()
{
	char buffer[1024];
	int r;

	printf( "//Auto-generated code.\n" );
	printf( "#define _SFR_ASM_COMPAT 1\n" );
	printf( "#define __SFR_OFFSET 0\n" );
	printf( "#include <avr/io.h>\n" );
	int hassent = 0;

	while( (r = ReadBlock(stdin, ", \t\r\n", buffer, 1024)) != EOF )
	{
		if( buffer[0] == ';' )
		{
			printf( "//%s", buffer );
			while( r != EOF && r != '\n' )
			{
				printf( "%c", r );
				r = fgetc( stdin );
			}
			printf( "\n" );
			continue;
		}

		if( buffer[0] == ':' )
		{
			if( hassent )
			{
				EndEmit();
			}
			sbp = 0;
			hassent = 1;	
			printf( ".global %s\n%s:\n", &buffer[1], &buffer[1] );
			printf( "\teor r18, r18\n" );
			printf( "\teor r19, r19\n" );
			printf( "\teor r20, r20\n" );
			printf( "\tori r18, 0b00010101\n" );
			printf( "\tori r19, 0b00001101\n" );
			printf( "\tori r20, 0b00000101\n" );
			printf( "\tout PORTB, r18\n" );

			Emit( 0x55 );
			Emit( 0x55 );
			Emit( 0x55 );
			Emit( 0x55 );
			Emit( 0x55 );
			Emit( 0x55 );
			Emit( 0x55 );
			Emit( 0xD5 );


			sbp = 0;
			continue;
		}

		if( strlen( buffer ) != 2 )
		{
			fprintf	( stderr, "Error: Invalid length of element: %s\n", buffer );
		}

		//LSB First, on wire.

		const unsigned char * bf = &buffer[0];

		unsigned hv = 0;

		if( strcmp( bf, "RN" ) != 0 )
		{
			if( *bf >= '0' && *bf <= '9' ) hv |= *bf-'0';
			else if( *bf >= 'A' && *bf <= 'F' ) hv |= *bf-'A'+10;
			else if( *bf >= 'a' && *bf <= 'f' ) hv |= *bf-'a'+10;

			hv<<=4; bf++;

			if( *bf >= '0' && *bf <= '9' ) hv |= *bf-'0';
			else if( *bf >= 'A' && *bf <= 'F' ) hv |= *bf-'A'+10;
			else if( *bf >= 'a' && *bf <= 'f' ) hv |= *bf-'a'+10;
//			fprintf( stderr, " %02x(%s) ", hv, bf );
		}
		else
		{
			hv = rand()%255;
		}

		sendbuf[sbp++] = hv;
//		fprintf( stderr, "%02x\n", hv );
//		fprintf( stderr, "->%02x<-\n", hv );
	}
	EndEmit();

	return 0;
}

