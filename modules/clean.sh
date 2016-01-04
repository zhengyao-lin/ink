#! /bin/bash

modules=`find -maxdepth 1 -type d`

LD=${ARCH_PREFIX}ld
LDFLAGS=-shared

for mod in $modules
do
	if [ ! $mod = "." ]; then
		cd $mod
		make clean
		cd ../
	fi
done