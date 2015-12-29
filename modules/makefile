CC=$(ARCH_PREFIX)g++
LD=$(ARCH_PREFIX)ld
CPPFLAGS=-I../../ -g -Wall -pedantic -fPIC
LDFLAGS=-shared

SH=bash

all: build_mods.sh
	$(SH) ./build_mods.sh

clean:
	$(RM) *.so *.o *.mod
	$(SH) ./clean.sh