#!/bin/bash

W=80
H=64
VID_SIZE="${W}x${H}"
FPS=20

# Download video
echo "Downloading video..."
youtube-dl https://www.youtube.com/watch?v=FtutLA63Cp8 -o bad

echo "Converting video..."
# Convert video to two tone black and white
ffmpeg -hide_banner -loglevel error -y -i bad.mkv -vf "fps=$FPS,scale=$VID_SIZE,format=monob" -f rawvideo bad_mono
du -bsh bad_mono | cut -f -1

# Script to encode the video
echo "Encoding video..."
python3 videnc.py $W $H
du -bsh bad_mono.enc | cut -f -1

# Compile decoder
echo "Compiling decoder..."
cc -DW=$W -DH=$H -DNDEBUG -O2 decoder/main.c -o decoder/decoder

# Test decoder against original data
echo "Testing decoder..."
decoder/decoder | cmp - bad_mono

# Display decoded video
echo "Displaying decoded video..."
decoder/decoder | ffplay -hide_banner -loglevel error -f rawvideo -pixel_format monob -video_size $VID_SIZE -framerate $FPS pipe:
