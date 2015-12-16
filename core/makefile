TARGET=core.o
REQUIRE=\
	gc/gc.o \
	native/native.o \
	thread/thread.o \
	coroutine/coroutine.o \
	hash.o \
	object.o \
	expression.o \
	context.o \
	general.o \
	error.o

CC=g++
LD=ld
CPPFLAGS=-I../ -g -Wall -pedantic
LDFLAGS=

$(TARGET): $(REQUIRE)
	$(LD) -r -o $@ $(REQUIRE) $(LDFLAGS)

gc/gc.o:
	cd gc; $(MAKE)

native/native.o:
	cd native; $(MAKE)

thread/thread.o:
	cd thread; $(MAKE)

coroutine/coroutine.o:
	cd coroutine; $(MAKE)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS)

clean:
	cd gc; $(MAKE) clean
	cd native; $(MAKE) clean
	cd thread; $(MAKE) clean
	cd coroutine; $(MAKE) clean
	$(RM) *.o