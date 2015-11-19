TARGET=core.o
REQUIRE=\
	gc/gc.o \
	hash.o \
	object.o \
	expression.o \
	context.o

CC=g++
LD=ld
CPPFLAGS=-I../ -g -Wall -pedantic
LDFLAGS=

$(TARGET): $(REQUIRE)
	$(LD) -r -o $@ $(REQUIRE) $(LDFLAGS)

gc/gc.o:
	cd gc; $(MAKE);

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS)

clean:
	cd gc; $(MAKE) clean
	$(RM) *.o