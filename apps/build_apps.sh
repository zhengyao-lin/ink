#! /bin/bash

apps=`find -maxdepth 1 -type d`

LD=${ARCH_PREFIX}ld
LDFLAGS=-shared

for app in $apps
do
	if [ ! $app = "." ]; then
		cd $app
		make
		cp build/* $1
		cd ../
	fi
done