#include <stdio.h>
#include <math.h>
#include <string.h>

const char * inputstream = "01011110001110100010111010100001010000101010101010010111010101110100101001010111110101010101001010100010000";//1101010101111010101010010101000000";

#define NOISE 0.0

float GetTimeAt( float f )
{
	f += ((double)(rand()%10000)/5000.0 - 1.0) * NOISE;

	int maxstream = strlen( inputstream );
	int ipl = floor( f );
	if( ipl >= maxstream ) return 0.0;

	float ffm = fmodf( f, 1.0 );
	int r = (inputstream[ipl] == '1');
	//Low-to-high is a 1, high-to-low is a 0


	if( ffm < 0.5 )
	{
		return !r;
	}
	else
	{
		return r;
	}
}

int main()
{
	float f = -.2; //.65 goodish, .25 goodish, .75 goodish.
	float f1q = 0.28;//33 MHz
	int maxstream = strlen( inputstream );

	int ict = 0, ictlast = 0;
	int lastr = GetTimeAt(f);;

	int ictcarry = 0;

	int long_def = 1.0 / f1q*1.1;
	int short_def = long_def-1;
	printf( "LD: %d SD: %d\n", long_def, short_def );

	int unknown = 0;
	int paired = 0;
	int last_short = 0;
	int guessedbit = 0;

	for( ; f < maxstream + 2; f+= f1q )
	{
		int r = GetTimeAt(f);
		if( lastr != r )
		{


//			int lict = ict + ((ict==2)?ictcarry:0);
//			if( ict < 2 ) ictcarry = 1;
//			if( ict > 2 ) ictcarry = -1;

			int lict = ict;
			
			int is_long  = (lict>=long_def);
			int is_short = (lict<=short_def);
			//If neither, then is indeterminate.
//			printf( "\n%d (%d %d)\n", ict, is_long, is_short );

			printf( "%c", (is_long?'L':is_short?'S':'?') );
if( 0 ){
			if( is_long )
			{
				if( last_short )
				{
					//last was long.
					printf( "(%d)", guessedbit );
					last_short = 0;
					guessedbit = !guessedbit;
				}
				//Process out any back-logged indeterminates and shorts.
				printf( "%d", guessedbit );
				//Flip bit, and print.
				guessedbit = !guessedbit;
				
			}
			else if( is_short )
			{
				if( last_short )
				{
					printf( "%d", guessedbit );
					last_short = 0;
				}
				else
				{
					last_short = 1;
				}
				//Mark off as known shorts.
			}
			else
			{
				//Unknown.
//				printf( "K" );
				unknown++;
			}
}
			ict = 0;
		}
		ict++;
		lastr = r;
//		printf( "%f\n", GetTimeAt(f) );
	}
	printf( "\n" ); 
	puts( inputstream );
	return 0;
}

