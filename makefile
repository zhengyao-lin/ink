ifeq ($(ARCH), arm)
	export ARCH_PREFIX=arm-none-linux-gnueabi-
else
	export ARCH_PREFIX=
endif

TARGET=test
REQUIRE=\
	syntax/syntax.o \
	core/core.o \
	msg/msg.o \
	interface/interface.o \
	test.o

CC=$(ARCH_PREFIX)g++
LD=$(ARCH_PREFIX)ld
CPPFLAGS= -g -Wall -pedantic
LDFLAGS=-pthread

$(TARGET): $(REQUIRE)
	$(CC) -o $@ $(REQUIRE) $(LDFLAGS)

core/core.o:
	cd core; $(MAKE)

syntax/syntax.o:
	cd syntax; $(MAKE)

msg/msg.o:
	cd msg; $(MAKE)

interface/interface.o:
	cd interface; $(MAKE)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS)

clean:
	cd core; $(MAKE) clean
	cd syntax; $(MAKE) clean
	cd msg; $(MAKE) clean
	cd interface; $(MAKE) clean
	$(RM) *.o $(TARGET)