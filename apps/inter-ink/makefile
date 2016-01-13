TARGET = build/inki
REQUIRE = interface.o

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