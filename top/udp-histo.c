//http://www.cs.ucsb.edu/~almeroth/classes/W01.176B/hw2/examples/udp-server.c

/* Sample UDP server */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>

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

/*
		if(mesg[0] == 'C' ) printf( "." );
		else
		      printf("%s",mesg);
			fflush( stdout );
//		if(mesg[0] == 'C' ) printf( "\n" );
*/
   }
}

