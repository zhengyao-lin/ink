TARGET=build/mod_IO.so
REQUIRE=\
	file.o

CC=$(ARCH_PREFIX)g++
LD=$(ARCH_PREFIX)g++
CPPFLAGS=-g -c -fPIC -Wall -pedantic
LDFLAGS=-shared -L../../lib -link-core
INCLUDES=-I../../

$(TARGET): $(REQUIRE)
	-mkdir build
	$(CC) -o $@ $(REQUIRE) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS) $(INCLUDES)

clean:
	$(RM) -r *.o *.so build