#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define PI 3.141592

typedef struct {
	unsigned char nb_channels;
	unsigned char nb_bits;
	unsigned short sampling_rate;
	unsigned short sine_freq;
	unsigned char sec;
} t_wave_param;

int getSineHex( t_wave_param *param );
int getsine_params( t_wave_param *param );

void usage()
{
	printf( "Usage: sine_gen [OPTION] ...\n" );
	printf( "  -c  : channels          [nb]	 		(default 1)\n" );
	printf( "  -b  : sample resolution [nb]  	  (default 16)\n" );
	printf( "  -r  : sampling rate 		 [Hz]			(default 44100)\n" );
	printf( "  -f  : frequency 				 [Hz]     (default 440)\n" );
	printf( "  -s  : duration          [sec]  	(default 4)\n" );
}

int main( int argc, char* argv[] )
{
	int option;
	t_wave_param sine_params = { 1, 16, 44100, 440, 1 };
	
	while( ( option = getopt( argc, argv, "c:b:r:f:s:h" ) ) != EOF ) {
		switch( option ) {
		case 99:
			sine_params.nb_channels = ( unsigned )atoi( optarg );
			break;
		case 'b':
			sine_params.nb_bits = ( unsigned )atoi( optarg );
			break;
		case 'r':
			sine_params.sampling_rate = ( unsigned )atoi( optarg );
			break;
		case 'f':
			sine_params.sine_freq = ( unsigned )atoi( optarg );
			break;
		case 's':
			sine_params.sec = ( unsigned )atoi( optarg );
			break;
		case 'h':
			usage();
			exit(EXIT_FAILURE);
		}
//		argc -= optind;
//		argv += optind;
//		printf( "%s\n", optarg );
	}
	/* Chirp detection */
  /*if (sine_params.sine_freq == 0) {
			getSineHex( &sine_params );
		getsine_params( &sine_params );
	} else*/
	{
	//	printf( "%d %d %d %d %d \n", sine_params.nb_bits, sine_params.nb_channels, sine_params.sampling_rate, sine_params.sine_freq, sine_params.sec );
		getSineHex( &sine_params );
		getsine_params( &sine_params );
	}

	return 0;
}


int getSineHex( t_wave_param *param )
{

	FILE* file;
	FILE* dat;
	long i, data, data_size;
	double raw_data;
	char filename[128];
	
	sprintf(filename,"sine_%ldHz.txt",  (long)param->sine_freq);
	if ( ( file = fopen(filename, "wb" ) ) == NULL ) {
		fprintf(stderr, "FAIL : fopen() %s\n", filename );
		return -1;
	}
	
	sprintf(filename,"sine_%ldHz.dat",  (long)param->sine_freq);
	if ( ( dat = fopen(filename, "wb" ) ) == NULL ) {
		fprintf(stderr, "FAIL : fopen() %s\n", filename );
		return -1;
	}

	/* 1 cycle */
	data_size = (( param->nb_bits / 8 ) * param->nb_channels * param->sampling_rate * param->sec);
	//data_size = data_size / param->sine_freq;
	data_size = (param->sampling_rate / param->sine_freq) + 2;

	fprintf(file,"uint8_t sine_%ldHz[%ld] = { ", (long)param->sine_freq, (long)(data_size * ( param->nb_bits / 8 )));
	
	fprintf(dat,"# Angle   Radian\n");

	for(i = 0; i < data_size; i++) {
		if ((i % 8) == 0) {
			fprintf(file,"\n/* %03ld */ ", i*( param->nb_bits / 8 ));
		}

		raw_data = sin( 2 * PI * param->sine_freq * i / param->sampling_rate ) * ( pow( 2, param->nb_bits) / 2 - 1 );
		data = ( long )raw_data;
		if( param->nb_bits == 8 ) {
			data += 128;
		}
		if( param->nb_channels == 2 ) {
			// L, R, ...
			if (param->nb_bits/8 == 1)
					fprintf(file,"0x%02X", (unsigned char)data);
			else
					fprintf(file,"0x%02X, 0x%02X",(unsigned char)(data >> 8)&0xFF,
																	 (unsigned char)(data&0xFF));
		}
		if (param->nb_bits/8 == 1) {
				fprintf(file,"0x%02X", (unsigned char)data);	
		} else {
				fprintf(file,"0x%02X, 0x%02X",(unsigned char)(data >> 8)&0xFF,
																	 (unsigned char)(data&0xFF));
		}

		fprintf(dat," %ld   %.2f\n",i,(float)data);

		if (i + 1 < data_size)
			fprintf(file,", ");
	}

	fprintf(file,"\n};\n");
	fclose(file);
	fclose(dat);
	return 0;
}


int getsine_params( t_wave_param* param ) {
	double raw_data;
	long i, data, data_size;
	char str[256];
	FILE* fp;

	sprintf(str, "%dbit-%dHz-sineWave-%dch_%dHz.wav", param->nb_bits, param->sampling_rate, param->nb_channels, param->sine_freq );
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
	25-28	44100	Sample sampling_rate - 32 byte integer.
				Common values are 44100 (CD), 48000 (DAT). Sample sampling_rate = Number of Samples per second, or Hertz.
	29-32	176400	(Sample sampling_rate * nb_bitsPerSample * Channels) / 8.
	33-34	4	(nb_bitsPerSample * Channels) / 8.1 - 8 bit mono2 - 8 bit stereo/16 bit mono4 - 16 bit stereo
	35-36	16	nb_bits per sample
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
	data_size = ( param->nb_bits / 8 ) *param->nb_channels * param->sampling_rate  * param->sec + 36;
	fwrite( &data_size, 1, 4, fp );

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
	data_size = 16;
	fwrite( &data_size, 4, 1, fp );

	// Format ID (Linear PCM = 1)
	data_size = 1;
	fwrite( &data_size, 2, 1, fp );

	// Channel
	data_size = param->nb_channels;
	fwrite( &data_size, 2, 1, fp );

	// Sampling sampling_rate [Hz]
	data_size = param->sampling_rate;
	fwrite( &data_size, 4, 1, fp );
	//fwrite( &sampling_rate, 4, 1, fp );

	// Bytes per sec
	data_size = ( param->nb_bits / 8 ) * param->nb_channels * param->sampling_rate;
	fwrite( &data_size, 4, 1, fp );

	// Bytes per sample * channels
	data_size = ( param->nb_bits / 8 ) * param->nb_channels;
	fwrite( &data_size, 2, 1, fp );

	// nb_bits per sample
	data_size = param->nb_bits;
	fwrite( &data_size, 2, 1, fp );

	// "data" chunk
	str[0] = 'd';
	str[1] = 'a';
	str[2] = 't';
	str[3] = 'a';
	fwrite( str, 1, 4, fp );

	// Data size (Bytes)
	data_size = ( param->nb_bits / 8 ) * param->nb_channels * param->sampling_rate * param->sec;
	fwrite( &data_size, 4, 1, fp );

	printf("  make %2dch %dbit %dHz --- %8dHz %dsec ... ", param->nb_channels, param->nb_bits, param->sampling_rate, param->sine_freq, param->sec);
	data_size = param->sampling_rate * param->sec;
	for(i = 0; i < data_size; i++) {
		raw_data = sin( 2 * PI * param->sine_freq * i / param->sampling_rate ) * ( pow( 2, param->nb_bits) / 2 - 1 );
		data = ( long )raw_data;
		if( param->nb_bits == 8 ) {
			data += 128;
		}
		if( param->nb_channels == 2 ) {
			// L, R, L, R, ...
			fwrite( &data, ( param->nb_bits / 8 ), 1, fp );
		}
		fwrite( &data, ( param->nb_bits / 8 ), 1, fp );
	}
	printf("Done.\n");

	fclose( fp );
	return 0;
}
