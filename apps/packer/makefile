TARGET = build/ink-packer
REQUIRE = packer.o

CC=$(ARCH_PREFIX)g++
LD=$(ARCH_PREFIX)g++
CPPFLAGS=-c -fPIC $(GLOBAL_CPPFLAGS)
LDFLAGS=-L$(GLOBAL_LIB_PATH) -l$(GLOBAL_CORE_LIB_NAME) -static-libgcc -static-libstdc++
INCLUDES=-I$(GLOBAL_ROOT_PATH)

$(TARGET): $(REQUIRE)
	-mkdir build
	$(CC) -o $@ $(REQUIRE) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS) $(INCLUDES)

clean:
	$(RM) -r *.o *.so build