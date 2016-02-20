#! /bin/bash

modules=`find -maxdepth 1 -type d`

LD=${ARCH_PREFIX}ld
LDFLAGS=-shared

if [ ! -d "build" ]; then
	mkdir build
fi

for mod in $modules
do
	if [ ! $mod = "." ]; then
		echo "find mod ${mod}"
		cd $mod
		make
		cp -r build/* "../build/"
		cd ../
	fi
done