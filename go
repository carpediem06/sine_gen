#!/bin/sh

gcc sine_gen.c -lm -o sine_gen
#	Param 	|		Description		| Unit		| default
#  -c  		: channels     			[nb]   			mono
#  -b  		: sample resolution [bits] 			16 bits
#  -r  		: sampling rate 		[Hz] 	 	  	44100 Hz
#  -f  		: frequency 				[Hz]     		440 Hz
#  -s  		: duration 					[sec]       4 s

./sine_gen -c 1 -b 16 -r 44100 -f 440 -s 60

# cvlc -vvv 16bit-44100Hz-sineWave-1ch_440Hz.wav  --sout '#duplicate{dst=rtp{dst=<ip>,port=10000}}'
