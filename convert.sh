#!/bin/bash

ffmpeg -vcodec png -i ./output-%d.png -vcodec rawvideo -f rawvideo -pixel_format rgb8 video.raw 
