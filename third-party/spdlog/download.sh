###
 # @Author: your name
 # @Date: 2021-10-25 11:02:51
 # @LastEditTime: 2021-10-27 16:02:31
 # @LastEditors: Please set LastEditors
 # @Description: In User Settings Edit
 # @FilePath: \dialogue-service\third-party\download.sh
### 
#!/bin/bash


# Clone spdlog from github use
echo ">>>>>>>>Start clone googletest from git..."
git clone git@github.com:gabime/spdlog.git

# build library
cmake -H. -B_builds
cmake --build _builds



