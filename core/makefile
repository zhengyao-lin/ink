TARGET=core.o
REQUIRE=\
	gc/gc.o \
	native/native.o \
	hash.o \
	object.o \
	expression.o \
	context.o \
	error.o

CC=g++
LD=ld
CPPFLAGS=-I../ -g -Wall -pedantic
LDFLAGS=

$(TARGET): $(REQUIRE)
	$(LD) -r -o $@ $(REQUIRE) $(LDFLAGS)

gc/gc.o:
	cd gc; $(MAKE);

native/native.o:
	cd native; $(MAKE);

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS)

clean:
	cd gc; $(MAKE) clean
	cd native; $(MAKE) clean
	$(RM) *.o