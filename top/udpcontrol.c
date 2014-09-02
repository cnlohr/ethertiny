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
		len = sizeof( cliaddr );
		n = recvfrom( sockfd, mesg, 1000, 0, (struct sockaddr *)&cliaddr, &len );
		sendto( sockfd, mesg, n, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr) );
		mesg[n] = 0;

		uint16_t t = ntohs( *((unsigned short*)&mesg[4]) );
		uint16_t p = ntohs( *((unsigned short*)&mesg[6]) );
		uint16_t ing = ntohs( *((unsigned short*)&mesg[8]) );
		uint16_t hict = ntohs( *((unsigned short*)&mesg[10]) );
		uint16_t loct = ntohs( *((unsigned short*)&mesg[12]) );
		printf( "Value: %5d %5d   Pings: %5d  cts: (%d/%d)\n", t, p, ing, hict, loct );

		char st[15];
		int i;
		for( i = 0; i < 15; i++ )
		{
			st[i] = (p>128)?(rand()&0x3f):(rand()%0xff);
		}
		usleep(10000);
		sendto( sockfd, st, 14, MSG_NOSIGNAL, (struct sockaddr *)&cliaddr, sizeof(cliaddr) );
	}
}

