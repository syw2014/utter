#########################################################################
# File Name: compile.sh
# Method: 
# Author: Jerry Shi
# Mail: jerryshi0110@gmail.com
# Created Time: Thu Oct 21 15:35:45 2021
#########################################################################
#!/bin/bash


# CMake configure
if [ ! -d "./_builds" ]; then
    echo ">>>>>Start configure with cmake..."
    cmake -H. -B_builds
fi


# CMake build
echo ">>>>>Start Build project with cmake..."
cmake --build _builds
