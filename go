#!/bin/sh

gcc sine_gen.c -lm -o sine_gen
#	Param 	|		Description		| Unit		| default
#  -c  		: channels     			[nb]   			mono
#  -b  		: sample resolution [bits] 			16 bits
#  -r  		: sampling rate 		[Hz] 	 	  	44100 Hz
#  -f  		: frequency 				[Hz]     		440 Hz
#  -s  		: duration 					[sec]       4 s
./sine_gen
