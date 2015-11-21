TARGET=test
REQUIRE=\
	syntax/syntax.o \
	core/core.o \
	msg/msg.o \
	test.o

CC=g++
LD=ld
CPPFLAGS= -g -Wall -pedantic
LDFLAGS=

$(TARGET): $(REQUIRE)
	$(CC) -o $@ $(REQUIRE) $(LDFLAGS)

core/core.o:
	cd core; $(MAKE)

syntax/syntax.o:
	cd syntax; $(MAKE)

msg/msg.o:
	cd msg; $(MAKE)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS)

clean:
	cd core; $(MAKE) clean
	cd syntax; $(MAKE) clean
	cd msg; $(MAKE) clean
	$(RM) *.o $(TARGET)