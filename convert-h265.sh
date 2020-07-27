#!/bin/bash

ffmpeg -vcodec png -i ./output-%d.png -x265-params lossless=1 -c:v libx265 video.mkv
