TARGET=gc.o
REQUIRE=\
	collect.o \
	engine.o

CC=$(ARCH_PREFIX)g++
LD=$(ARCH_PREFIX)ld
CPPFLAGS=-I../../ -g -Wall -pedantic -fPIC
LDFLAGS=

$(TARGET): $(REQUIRE)
	$(LD) -r -o $@ $(REQUIRE) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS)

clean:
	$(RM) *.o