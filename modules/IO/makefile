TARGET=build/IO.so
REQUIRE=\
	file.o

CC=$(ARCH_PREFIX)g++
LD=$(ARCH_PREFIX)g++
CPPFLAGS=-g -c -fPIC -Wall -pedantic
LDFLAGS=-shared -L$(GLOBAL_LIB_PATH) -l$(GLOBAL_CORE_LIB_NAME)
INCLUDES=-I../../

$(TARGET): $(REQUIRE)
	-mkdir build
	$(CC) -o $@ $(REQUIRE) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS) $(INCLUDES)

clean:
	$(RM) -r *.o *.so build