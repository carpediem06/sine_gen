#!/bin/sh

gcc sine_gen.c -lm -o sine_gen
#	Param 	|		Description		| Unit		| default
#  -c  		: channels     			[nb]   			mono
#  -b  		: sample resolution [bits] 			16 bits
#  -r  		: sampling rate 		[Hz] 	 	  	44100 Hz
#  -f  		: frequency 				[Hz]     		440 Hz
#  -s  		: duration 					[sec]       4 s
#
#./sine_gen -c 1 -b 16 -r 44100 -f 440 -s 60
# cvlc -vvv 16bit-44100Hz-sineWave-1ch_440Hz.wav  --sout '#duplicate{dst=rtp{dst=<ip>,port=10000}}'

SAMPLING_RATE=44100
NB_BITS=16
NB_CH=1
#------------------------------------------------
# Note			Frequency (Hz)	Wavelength (cm)
#------------------------------------------------
# 	A4  		440.00
#		C5			523.25 					65.93
#		D5			587.33 					58.74
# 	F#5/Gb5 739.99 					46.62
#		G5			783.99 					44.01
# 	C6			1046.50					32.97
# 	F7  		2793.83					12.35
# 	E8  		5274.04					6.54
# 	B8  		7902.13					4.37
#------------------------------------------------
for SINE_FREQ in 523 523 523 587 659 587 523 659 587 587 523 440 739 783 1046 2794 5274 7902
do
	./sine_gen -c 1 -b ${NB_BITS} -r ${SAMPLING_RATE} -f ${SINE_FREQ} -s ${NB_CH}
	cvlc --play-and-exit  ${NB_BITS}bit-${SAMPLING_RATE}Hz-sineWave-${NB_CH}ch_${SINE_FREQ}Hz.wav
done



