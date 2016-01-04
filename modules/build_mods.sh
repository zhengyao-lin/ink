#! /bin/bash

modules=`find -maxdepth 1 -type d`

LD=${ARCH_PREFIX}ld
LDFLAGS=-shared

for mod in $modules
do
	if [ ! $mod = "." ]; then
		echo "find mod ${mod}"
		cd $mod
		make
		cp build/*.so "../${mod}.so"
		cd ../
	fi
done