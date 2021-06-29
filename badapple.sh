#!/bin/bash

W=80
H=64
VID_SIZE="${W}x${H}"
FPS=30

#youtube-dl https://www.youtube.com/watch?v=FtutLA63Cp8 -o bad
#ffmpeg -hide_banner -loglevel error -y -i bad.mkv -vf "fps=$FPS,scale=$VID_SIZE,format=monob" -sws_dither none -f rawvideo worse
#ffmpeg -hide_banner -loglevel error -y -i bad.mkv -f lavfi -i color=gray -f lavfi -i color=black -f lavfi -i color=white -lavfi "fps=$FPS,scale=320:240,threshold,scale=$VID_SIZE,format=monob" -sws_dither none -f rawvideo bad_mono
#python3 videnc.py
#du -bsh bad_mono bad_mono.bin bad_mono.rle bad_mono.rle.bin bad_mono.enc bad_mono.enc.bin
cc decoder/main.c -o decoder/decoder
#decoder/decoder | hexdump -ve '64/1 "%04X" "\n"' > blog
decoder/decoder | cmp - bad_mono
decoder/decoder | ffplay -hide_banner -loglevel error -f rawvideo -pixel_format monob -video_size $VID_SIZE -framerate $FPS pipe:
