#! /bin/bash

modules=`find mod_* -maxdepth 0 -type d`

LD=${ARCH_PREFIX}ld
LDFLAGS=-shared

for mod in $modules
do
	cd $mod
	make
	cp build/*.so "../${mod}.mod"
done