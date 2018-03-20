#!/bin/sh

gcc -o testencodeing test/testencodeing.c -I$(pwd) libx264.a -lpthread -ldl -lm
