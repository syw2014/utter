###
 # @Author: your name
 # @Date: 2021-10-25 11:02:51
 # @LastEditTime: 2021-11-03 14:49:08
 # @LastEditors: Please set LastEditors
 # @Description: In User Settings Edit
 # @FilePath: \dialogue-service\third-party\download.sh
### 
#!/bin/bash

if [ ! -e boost ]; then
  echo "Cloning boost ..."
  git clone --recursive git@github.com:boostorg/boost.git
fi
echo "Updating boost..."

if [ ! -e boost ];then
    echo "Clone failed!"
    exit 1
fi

cd boost
git checkout master
git pull
git checkout boost-1.76.0

echo "Building boost..."
mkdir -p _builds/ouput
./bootstrap.sh  --prefix=./_builds/ouput
./b2 install


