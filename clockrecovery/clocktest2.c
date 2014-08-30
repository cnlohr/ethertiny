#include <stdio.h>
#include <strings.h>
int main()
{
	int binct0[6];
	int binct1[6];
	bzero( binct0, sizeof( binct0 ) );
	bzero( binct1, sizeof( binct1 ) );

	FILE * f = fopen ("clocktest2.txt", "rb" );
	int c;

	int qty = 0;
	int lastv = 0;
	int bits = 0;
	int samples = 0;
	while( (c = fgetc(f)) != EOF )
	{
		int v;
		if( c == '1' ) v = 1;
		else if( c == '0' ) v = 0;
		else continue;

		samples++;

		//have v.
		if( lastv == v )
		{
			qty++;
			continue;
		}

		printf( "%d(%d) ", qty, lastv );
		//If lastv == 0, 2,3 = Long; 0,1 = short... (1) MAAAY be questionable.
		//If lastv == 1, 3,4 = Long 0,1,2 = short;  (2) Indeterminate

		if( lastv ) binct1[qty]++;
			else	binct0[qty]++;
		if( lastv == 0 ) bits++;

		lastv = v;
		qty = 0;
	}
	printf( "\nTotal bits: %d  Bytes: %d Sample Bytes: %d\n", bits, bits/8, samples/8 );
/*
	int i;
	printf( "0:\n" );
	for( i = 0; i < 6; i++ )
	{
		printf( "%d %d\n", i, binct0[i] );
	}
	printf( "1:\n" );
	for( i = 0; i < 6; i++ )
	{
		printf( "%d %d\n", i, binct1[i] );
	}*/
}

