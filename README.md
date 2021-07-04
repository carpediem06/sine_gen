# sine_gen

To generate a sine waveform (.wav) file.

From https://github.com/moutend/getSineWave

## Compile

    $ ./go


## Usage / Options

* c : channels     			[nb]   			mono
* b : sample resolution [bits] 			16 bits
* r : sampling rate 		[Hz] 	 	  	44100 Hz
* f : frequency 				[Hz]     		440 Hz
* s : duration 					[sec]       5 s
* h : help

``` ./sine_gen -c 1 -b 16 -r 44100 -f 440 -s 60
```
Generated file is 16bit-44100Hz-sineWave-1ch_440Hz.wav

## To stream it via RTP

```
	cvlc -vvv 16bit-44100Hz-sineWave-1ch_440Hz.wav  --sout '#duplicate{dst=rtp{dst=172.30.28.201,port=10000}}'
```
```
	ffmpeg -re -i 16bit-44100Hz-sineWave-1ch_440Hz.wav  -ar 44100 -ac 1 -acodec pcm_s16be -ab 705k -f rtp rtp://172.30.28.201:10000
```
