#!/bin/sh

unamestr=`uname`

if [ ! -e odoors ]
then
    git clone https://gitlab.com/apamment/odoors.git
fi

cd odoors
if [ "$unamestr" == 'Linux' ]; then
   make
else
   gmake
fi
cd ..
gcc -I./odoors -c main.c 
gcc -o mastermind main.o odoors/libs-`uname`/libODoors.a 
