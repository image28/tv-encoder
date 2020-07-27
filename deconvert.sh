#!/bin/bash

ffmpeg -vcodec rawvideo -f rawvideo -s 800x600 -pixel_format rgb8 -i ./video.raw -vcodec png  output-%d.png 
