#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define PI 3.141592

typedef struct {
	unsigned ch;
	unsigned bits;
	unsigned rate;
	unsigned fs;
	unsigned sec;
} t_wave_param;

int getSineWave( t_wave_param *sw );

void usage() {
	printf( "Usage: sine_gen [OPTION] ...\n" );
	printf( "  -c  : channels          [nb]	 		(default 1)\n" );
	printf( "  -b  : sample resolution [bits]  	(default 16)\n" );
	printf( "  -r  : sampling rate 		 [Hz]			(default 44100)\n" );
	printf( "  -f  : frequency 				 [Hz]     (default 440)\n" );
	printf( "  -s  : duration          [sec]  	(default 4)\n" );
}

int main( int argc, char* argv[] ) {
	int option;
	t_wave_param sinewave = { 1, 16, 44100, 440, 5 };
	
	while( ( option = getopt( argc, argv, "c:b:r:f:s:h" ) ) != EOF ) {
		switch( option ) {
		case 99:
			sinewave.ch = ( unsigned )atoi( optarg );
			break;
		case 'b':
			sinewave.bits = ( unsigned )atoi( optarg );
			break;
		case 'r':
			sinewave.rate = ( unsigned )atoi( optarg );
			break;
		case 'f':
			sinewave.fs = ( unsigned )atoi( optarg );
			break;
		case 's':
			sinewave.sec = ( unsigned )atoi( optarg );
			break;
		case 'h':
			usage();
			exit(EXIT_FAILURE);
		}
//		argc -= optind;
//		argv += optind;
//		printf( "%s\n", optarg );
	}
//	printf( "%d %d %d %d %d \n", sinewave.bits, sinewave.ch, sinewave.rate, sinewave.fs, sinewave.sec );
	getSineWave( &sinewave );
	return 0;
}

int getSineWave( t_wave_param* sw ) {
	double raw_data;
	long i, data, var_long;
	char str[256];
	FILE* fp;

	sprintf(str, "%dbit-%dHz-sineWave-%dch_%dHz.wav", ( *sw ).bits, ( *sw ).rate, ( *sw ).ch, ( *sw ).fs );
	if ( ( fp = fopen(str, "wb" ) ) == NULL ) {
		fputs( "ERR : fopen() failed\n", stderr );
		return -1;
	}

/*
	1 - 4	“RIFF”	Marks the file as a riff file. Characters are each 1 byte long.
	5 - 8	File size (integer)	Size of the overall file - 8 bytes, in bytes (32-bit integer). Typically, you’d fill this in after creation.
	9 -12	“WAVE”	File Type Header. For our purposes, it always equals “WAVE”.
	13-16	“fmt "	Format chunk marker. Includes trailing null
	17-20	16	Length of format data as listed above
	21-22	1	Type of format (1 is PCM) - 2 byte integer
	23-24	2	Number of Channels - 2 byte integer
	25-28	44100	Sample Rate - 32 byte integer. 
				Common values are 44100 (CD), 48000 (DAT). Sample Rate = Number of Samples per second, or Hertz.
	29-32	176400	(Sample Rate * BitsPerSample * Channels) / 8.
	33-34	4	(BitsPerSample * Channels) / 8.1 - 8 bit mono2 - 8 bit stereo/16 bit mono4 - 16 bit stereo
	35-36	16	Bits per sample
	37-40	“data”	“data” chunk header. Marks the beginning of the data section.
	41-44	File size (data)	Size of the data section.
				Sample values are given above for a 16-bit stereo source.		
*/
   
	// "RIFF" header
	str[0] = 'R';
	str[1] = 'I';
	str[2] = 'F';
	str[3] = 'F';
	fwrite( str, 1, 4, fp );

	// File size ( with header 36 bytes)
	var_long = ( ( *sw ).bits / 8 ) * ( *sw ).ch * ( *sw ).rate  * ( *sw ).sec + 36;
	fwrite( &var_long, 1, 4, fp );

	// "WAVE" header
	str[0] = 'W';
	str[1] = 'A';
	str[2] = 'V';
	str[3] = 'E';
	fwrite( str, 1, 4, fp );

	// "fmt " chunk
	str[0] = 'f';
	str[1] = 'm';
	str[2] = 't';
	str[3] = ' ';
	fwrite( str, 1, 4, fp );

	// Bytes of fmt chunk (Linear PCM = 16)
	var_long = 16;
	fwrite( &var_long, 4, 1, fp );

	// Format ID (Linear PCM = 1)
	var_long = 1;
	fwrite( &var_long, 2, 1, fp );

	// Channel
	var_long = ( *sw ).ch;
	fwrite( &var_long, 2, 1, fp );

	// Sampling rate [Hz]
	var_long = ( *sw ).rate;
	fwrite( &var_long, 4, 1, fp );
	//fwrite( &rate, 4, 1, fp );

	// Bytes per sec
	var_long = ( ( *sw ).bits / 8 ) * ( *sw ).ch * ( *sw ).rate;
	fwrite( &var_long, 4, 1, fp );

	// Bytes per sample * channels
	var_long = ( ( *sw ).bits / 8 ) * ( *sw ).ch;
	fwrite( &var_long, 2, 1, fp );

	// Bits per sample
	var_long = ( *sw ).bits;
	fwrite( &var_long, 2, 1, fp );

	// "data" chunk
	str[0] = 'd';
	str[1] = 'a';
	str[2] = 't';
	str[3] = 'a';
	fwrite( str, 1, 4, fp );

	// Data size (Bytes)
	var_long = ( ( *sw ).bits / 8 ) * ( *sw ).ch * ( *sw ).rate * ( *sw ).sec;
	fwrite( &var_long, 4, 1, fp );

	printf("  make %2dch %dbit %dHz --- %8dHz %dsec ... ", ( *sw ).ch, ( *sw ).bits, ( *sw ).rate, ( *sw ).fs, ( *sw ).sec);
	var_long = ( *sw ).rate * ( *sw ).sec;
	for(i = 0; i < var_long; i++) {
		raw_data = sin( 2 * PI * ( *sw ).fs * i / ( *sw ).rate ) * ( pow( 2, ( *sw ).bits) / 2 - 1 );
		data = ( long )raw_data;
		if( ( *sw ).bits == 8 ) {
			data += 128;
		}
		if( ( *sw ).ch == 2 ) {
			// L, R, L, R, ...
			fwrite( &data, ( ( *sw ).bits / 8 ), 1, fp );
		}
		fwrite( &data, ( ( *sw ).bits / 8 ), 1, fp );
	}
	printf("OK\n");

	fclose( fp );
	return 0;
}
