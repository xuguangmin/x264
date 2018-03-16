#!/bin/sh


./configure --prefix=$(pwd)/build  #--cross-prefix=arm-linux- --host=arm-linux 


make -j4
