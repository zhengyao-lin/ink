TARGET=native.o
REQUIRE=\
	math.o \
	global.o \
	string.o \
	object.o \
	function.o \
	array.o

CC=g++
LD=ld
CPPFLAGS=-I../../ -g -Wall -pedantic
LDFLAGS=

$(TARGET): $(REQUIRE)
	$(LD) -r -o $@ $(REQUIRE) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS)

clean:
	$(RM) *.o