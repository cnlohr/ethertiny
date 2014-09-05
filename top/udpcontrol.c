//http://www.cs.ucsb.edu/~almeroth/classes/W01.176B/hw2/examples/udp-server.c


/* Sample UDP server */

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <math.h>

unsigned long HSVtoHEX( float hue, float sat, float value )
{

	float pr = 0;
	float pg = 0;
	float pb = 0;

	short ora = 0;
	short og = 0;
	short ob = 0;

	float ro = fmod( hue * 6, 6. );

	float avg = 0;

	ro = fmod( ro + 6 + 1, 6 ); //Hue was 60* off...

	if( ro < 1 ) //yellow->red
	{
		pr = 1;
		pg = 1. - ro;
	} else if( ro < 2 )
	{
		pr = 1;
		pb = ro - 1.;
	} else if( ro < 3 )
	{
		pr = 3. - ro;
		pb = 1;
	} else if( ro < 4 )
	{
		pb = 1;
		pg = ro - 3;
	} else if( ro < 5 )
	{
		pb = 5 - ro;
		pg = 1;
	} else
	{
		pg = 1;
		pr = ro - 5;
	}

	//Actually, above math is backwards, oops!
	pr *= value;
	pg *= value;
	pb *= value;

	avg += pr;
	avg += pg;
	avg += pb;

	pr = pr * sat + avg * (1.-sat);
	pg = pg * sat + avg * (1.-sat);
	pb = pb * sat + avg * (1.-sat);

	ora = pr * 255;
	og = pb * 255;
	ob = pg * 255;

	if( ora < 0 ) ora = 0;
	if( ora > 255 ) ora = 255;
	if( og < 0 ) og = 0;
	if( og > 255 ) og = 255;
	if( ob < 0 ) ob = 0;
	if( ob > 255 ) ob = 255;

	return (ob<<16) | (og<<8) | ora;
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

	float hs = 0;

	for (;;)
	{
		len = sizeof( cliaddr );
		n = recvfrom( sockfd, mesg, 1000, 0, (struct sockaddr *)&cliaddr, &len );
//		sendto( sockfd, mesg, n, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr) );
		mesg[n] = 0;

		uint16_t t = ntohs( *((unsigned short*)&mesg[4]) );
		uint16_t p = ntohs( *((unsigned short*)&mesg[6]) );
		uint16_t ing = ntohs( *((unsigned short*)&mesg[8]) );
		uint16_t hict = ntohs( *((unsigned short*)&mesg[10]) );
		uint16_t loct = ntohs( *((unsigned short*)&mesg[12]) );
		printf( "Value: %5d %5d   Pings: %5d  cts: (%d/%d)\n", t, p, ing, hict, loct );

		char st[15];
		int i;
		for( i = 0; i < 5; i++ )
		{
			uint32_t rr = HSVtoHEX( hs + i * .1, 1.0, (p>128)?.1:1.0 );
			st[i*3+0] = (rr >>  0)&0xff;
			st[i*3+1] = (rr >>  8)&0xff;
			st[i*3+2] = (rr >> 16)&0xff;
			//st[i] = (p>128)?(rand()&0x3f):(rand()%0xff);
		}
		usleep(1000);
		sendto( sockfd, st, 15, MSG_NOSIGNAL, (struct sockaddr *)&cliaddr, sizeof(cliaddr) );
		hs = ( hs + .01 );
		if( hs > 1 ) hs -= 1;
	}

}

