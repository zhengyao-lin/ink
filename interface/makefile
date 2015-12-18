TARGET=interface.o
REQUIRE=\
	engine.o \
	setting.o

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