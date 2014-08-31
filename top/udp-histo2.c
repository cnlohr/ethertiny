//http://www.cs.ucsb.edu/~almeroth/classes/W01.176B/hw2/examples/udp-server.c


/* Sample UDP server */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>

//Note: -1 indicates unmatched short pulse.  I.e. quick transitions must pair up.
int16_t ParseEthernet( unsigned char * raw_data_packet, uint16_t max_packet_data_len )
{
	uint8_t in_preamble = 4;
	uint8_t t;
	uint8_t mask;

	uint8_t bit;
	uint8_t lastbit = 0;
	int8_t count = 0;
	uint16_t plen = 0;
	uint8_t shortflag = 0;

	uint8_t cemitbit = 1; //current emitting bit.
	uint8_t cemitbyte = 0;
	uint8_t cemitmask = 1;

	uint8_t * output = raw_data_packet;

	while( 1 )
	{
		t = *(raw_data_packet++);
		for( mask = 0x80; mask; mask>>=1 )
		{
			bit = (t&mask)?1:0;
			if( bit != lastbit )
			{
				//Ignore first two changes.
				if( in_preamble > 2 && count > 1 )
				{
					in_preamble--;
				}
				else if( in_preamble <= 2 && count < 2 && in_preamble )
				{
					in_preamble--;
				}
				else if( !in_preamble )
				{
					uint8_t mark_emit = 0;

					//Something happened.
					//If it's immediate, i.e. count = 0, count = 1 -> short (mark for ticking)
					//If it's later, i.e. count = 2, count = 3 -> long (invert)
					//We cannot get >3 as that's handled when we increase count.
					if( count > 1 )
					{
						if( shortflag )
						{
							return -1;
						}
						//invert bit.
						cemitbit = !cemitbit;

						mark_emit = 1;
					}
					else
					{
						if( shortflag )
						{
							mark_emit = 1;
							shortflag = 0;
						}
						else
						{
							shortflag = 1;
						}
					}
					if( mark_emit )
					{
						//Emit cemitbit. 
						cemitbyte |= cemitbit?cemitmask:0;
						cemitmask <<= 1;

						if( (cemitmask&0xff) == 0 )
						{
							output[plen++] = cemitbyte;
							cemitbyte = 0;
							cemitmask = 1;
						}
						mark_emit = 0;
					}
				}

				lastbit = bit;
				count = 0;
			}
			else
			{
				count++;
				if( count > 3 )
				{
					return plen;
				}
			}
		}
		//Next byte.
	}
}


int main(int argc, char**argv)
{
   int sockfd,n;
   struct sockaddr_in servaddr,cliaddr;
   socklen_t len;
   char mesg[1000];

   sockfd=socket(AF_INET,SOCK_DGRAM,0);

   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
   servaddr.sin_port=htons(13312);
   bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

   for (;;)
   {
      len = sizeof(cliaddr);
      n = recvfrom(sockfd,mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
      sendto(sockfd,mesg,n,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
      mesg[n] = 0;

	unsigned char copiedpack[1024];
	memcpy( copiedpack, mesg, n );
	int parsedlen = ParseEthernet( copiedpack, 1024 );
//printf( "MARK\n" );
		int j;

		#define mbins 7
		int bins1[mbins];
		int bins0[mbins];
		memset( bins1, 0, sizeof( bins1 ) );
		memset( bins0, 0, sizeof( bins0 ) );
		int lastbit = 0;
		int count = 0;


		int tainted = 0;
		int curbit = 0;
		int primedshort = 0;
		int progress = -1;
		char outputbuffer[8192];
		memset( outputbuffer, 0, sizeof( outputbuffer ) );
		FILE * lastpack = fopen( "lastpack.txt", "w" );
		for(j = 1; j < n;j ++)
		{
			unsigned r = mesg[j];
			int k;
			for( k = 0x80; k; k>>=1 )
			{
				int bit = (r&k)?1:0;
				printf( "%d", bit );
				fprintf( lastpack, "%d", bit );
				if( bit != lastbit )
				{
					//
					if( !tainted )
				{
					if( progress >= 0 )
					{
						if( count > 3 ) { printf( "TAINT Count\n" ); tainted = 1; }
						else if( count > 1 )
						{
							curbit = !curbit;
							outputbuffer[progress++] = curbit?'1':'0';
							if( primedshort ) { printf( "TAINT uneven\n" ); tainted = 2; }
						}
						else
						{
							if( primedshort == -1 )
							{
								primedshort = 0;
							}
							else if( primedshort )
							{
								outputbuffer[progress++] = curbit?'1':'0';
								primedshort = 0;
							}
							else
							{
								primedshort = 1;
							}
						}
					}
					else
					{
						if( count < 2 && j > 2 )
						{
							progress = 0;
							//outputbuffer[progress++] = '1';
							curbit = 1;
							primedshort = -1;
						}
					}
				}
					//
					//
					if( count < mbins )
					{
						if( lastbit )
							bins1[count]++;
						else
							bins0[count]++;
					}
					else
					{
						goto done;
					}
					lastbit = bit;
					count = 0;
				}
				else
				{
					count++;
				}
				if( count > mbins ) goto done;
			}
			fprintf( lastpack, " " );
		//	printf( "\n" );	
		}
done:
		printf( "\n" );
		fprintf( lastpack, "\n" );

		printf( "1: " );
		for( j = 0; j < mbins; j++ )
		{
			printf( "%5d ", bins1[j] );
		}

		printf( " (%d / %d)", bins1[0] + bins1[1], bins1[2] + bins1[3] + bins1[4] );
		printf( "\n" );

		printf( "0: " );
		for( j = 0; j < mbins; j++ )
		{
			printf( "%5d ", bins0[j] );
		}
		printf( " (%d / %d)", bins0[0] + bins0[1], bins0[2] + bins0[3] + bins0[4] );

		printf( "\n" );

		puts( outputbuffer );

		printf( "\n" );

		unsigned outputdata[1024];
		int by = 0;
		int pl = 1;
		int sofar = 0;
		for( j = 0; j < strlen( outputbuffer ); j++ )
		{
			sofar |= (outputbuffer[j]=='1')?pl:0;
			pl<<=1;
			if( pl > 0x80 )
			{
				outputdata[by++] = sofar;
				sofar = 0; pl = 1;
			}
		}

		for( j = 0; j < by; j++ )
		{
			if( outputdata[j] >= ' ' && outputdata[j] < 127 )
				printf( "%02x[%c] ", outputdata[j], outputdata[j] );
			else
				printf( "%02x ", outputdata[j] );
		}
		printf( "\n" );

//parsedlen

		if( copiedpack <= 0 )
		{
			printf( "-could not parse-\n" );
		}
		else
		{
			printf( "ToCopy: %d\n", parsedlen );
			for( j = 0; j < parsedlen; j++ )
			{
				if( copiedpack[j] >= ' ' && copiedpack[j] < 127 )
					printf( "%02x[%c] ", copiedpack[j], copiedpack[j] );
				else
					printf( "%02x ", copiedpack[j] );
			}
			printf( "\n" );
		}

/*
		if(mesg[0] == 'C' ) printf( "." );
		else
		      printf("%s",mesg);
			fflush( stdout );
//		if(mesg[0] == 'C' ) printf( "\n" );
*/
   }
}

