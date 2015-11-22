TARGET=native.o
REQUIRE=\
	math.o \
	branch.o \
	string.o \
	object.o

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