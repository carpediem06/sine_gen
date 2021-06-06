#!/bin/sh

gcc sinus_gen.c -lm -o sinus_gen
#	Param 	|		Description		| Unit		| default
#  -c  		: channels     			[nb]   			mono
#  -b  		: sample resolution [bits] 			16 bits
#  -r  		: sampling rate 		[Hz] 	 	  	44100 Hz
#  -f  		: frequency 				[Hz]     		440 Hz
#  -s  		: duration 					[sec]       4 s
./sinus_gen
