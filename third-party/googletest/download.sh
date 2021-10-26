###
 # @Author: your name
 # @Date: 2021-10-25 11:02:51
 # @LastEditTime: 2021-10-26 11:49:09
 # @LastEditors: Please set LastEditors
 # @Description: In User Settings Edit
 # @FilePath: \dialogue-service\third-party\download.sh
### 
#!/bin/bash


# Clone google test framework for unit test
echo ">>>>>>>>Start clone googletest from git..."
# wget https://github.com/google/googletest/archive/refs/tags/release-1.11.0.tar.gz

# extract
tar -xf release-1.11.0.tar.gz

if  [ ! -d "./googletest-release-1.11.0" ]; then
    echo "Folder not exists!"
    exit 1
fi

# rename
mv googletest-release-1.11.0 googletest

# compile
cd googletest

echo ">>>>>>>>Start to build google test..."
cmake -H. -B_builds

cmake --build _builds


