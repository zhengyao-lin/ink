TARGET = build/ink-packer
REQUIRE = packer.o

LDFLAGS=-L$(GLOBAL_LIB_PATH) -l$(GLOBAL_CORE_LIB_NAME) -static-libgcc -static-libstdc++
INCLUDES=-I$(GLOBAL_ROOT_PATH)

ifeq ($(GLOBAL_PLATFORM), windows)
	CPPFLAGS=-c $(GLOBAL_CPPFLAGS)
else
	CPPFLAGS=-c -fPIC $(GLOBAL_CPPFLAGS)
endif

$(TARGET): $(REQUIRE)
	-mkdir build
	$(CC) -o $@ $(REQUIRE) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS) $(INCLUDES)

clean:
	$(RM) -r *.o *.so build