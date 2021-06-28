#!/bin/bash

W=80
H=64
VID_SIZE="${W}x${H}"
FPS=30

# ways to improve compression:
# Encode/decode with RLE (use piping to use FFplay when decoding)
# Get rid of dithering due to reduced RLE efficiency
# Consider storing in order of tiles so it can be streamed on the NES
# This might mean the RLE doesn't go in scan lines, but a different pattern
# Store in byte increments to make it easier to decode, and experiment with different configs eg
# nnnb nnnb (two RLEs per byte)
# nnnn nnnb (one RLE per byte)
# nnnn nnnn nnnb
# nnnn nnnn nnnn nnnb

# keep track of changes between frames

# https://wiki.nesdev.com/w/index.php/Bad_Apple
# Different frame types ^^

# Heatshrink compression looks very promising, perhaps after performing proper video encoding

#youtube-dl https://www.youtube.com/watch?v=FtutLA63Cp8 -o bad
#ffmpeg -hide_banner -loglevel error -y -i bad.mkv -vf "fps=$FPS,scale=$VID_SIZE,format=monob" -sws_dither none -f rawvideo worse


# Frame types:
# 01 Full frame, no RLE
# 10 Full frame, RLE
# 11 Changes, RLE
# 00 No change???
# The rest of the byte can just be ignored, I guess?

# the format:
# I-frame: FF <640 bytes of the frame>
# P-frame: 00 

#ffmpeg -hide_banner -loglevel error -y -i bad.mkv -f lavfi -i color=gray -f lavfi -i color=black -f lavfi -i color=white -lavfi "fps=$FPS,scale=320:240,threshold,scale=$VID_SIZE,format=monob" -sws_dither none -f rawvideo bad_mono
#python3 rle.py
#python3 videnc.py
#du -bsh bad_mono bad_mono.bin bad_mono.rle bad_mono.rle.bin bad_mono.enc bad_mono.enc.bin
cc decoder/main.c -o decoder/decoder
decoder/decoder | hexdump -ve '64/1 "%04X" "\n"' > blog
decoder/decoder | ffplay -hide_banner -loglevel error -f rawvideo -pixel_format monob -video_size $VID_SIZE -framerate $FPS pipe:
