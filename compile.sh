#########################################################################
###
 # @Author: your name
 # @Date: 2021-10-21 15:35:47
 # @LastEditTime: 2021-10-26 11:51:32
 # @LastEditors: your name
 # @Description: In User Settings Edit
 # @FilePath: \dialogue-service\compile.sh
### 
 # @Author: your name
 # @Date: 2021-10-21 15:35:47
 # @LastEditTime: 2021-10-22 17:45:56
 # @LastEditors: your name
 # @Description: In User Settings Edit
 # @FilePath: \dialogue-service\compile.sh
#########################################################################
#!/bin/bash


# CMake configure
if [ ! -d "./_builds" ]; then
    echo ">>>>>Start configure with cmake..."
    cmake -H. -B_builds -DCMAKE_BUILD_TYPE=DEBUG
fi


# CMake build
echo ">>>>>Start Build project with cmake..."
cmake --build _builds
