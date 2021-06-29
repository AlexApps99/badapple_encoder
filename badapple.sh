#!/bin/bash

W=80
H=64
VID_SIZE="${W}x${H}"
FPS=30

# Download video
youtube-dl https://www.youtube.com/watch?v=FtutLA63Cp8 -o bad

# Convert video to two tone black and white
# This approach had too much dithering
#ffmpeg -hide_banner -loglevel error -y -i bad.mkv -vf "fps=$FPS,scale=$VID_SIZE,format=monob" -sws_dither none -f rawvideo worse

# This approach has weird jagged edges and I don't like it much either, oh well...
ffmpeg -hide_banner -loglevel error -y -i bad.mkv -f lavfi -i color=gray -f lavfi -i color=black -f lavfi -i color=white -lavfi "fps=$FPS,scale=320:240,threshold,scale=$VID_SIZE,format=monob" -sws_dither none -f rawvideo bad_mono

# Script to encode the video
python3 videnc.py

# Compare file sizes
du -bsh bad_mono bad_mono.enc

# Compile decoder
cc decoder/main.c -o decoder/decoder

# Test decoder against original data
decoder/decoder | cmp - bad_mono

# Display decoded video
decoder/decoder | ffplay -hide_banner -loglevel error -f rawvideo -pixel_format monob -video_size $VID_SIZE -framerate $FPS pipe:
