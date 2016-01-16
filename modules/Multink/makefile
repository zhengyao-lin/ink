TARGET=build/Multink.$(GLOBAL_LIB_SUFFIX)
REQUIRE=\
	actor.o

CPPFLAGS=-c -fPIC $(GLOBAL_CPPFLAGS)
LDFLAGS=-shared -static-libgcc -static-libstdc++ -L$(GLOBAL_LIB_PATH) -l$(GLOBAL_CORE_LIB_NAME)
INCLUDES=-I$(GLOBAL_ROOT_PATH)

$(TARGET): $(REQUIRE)
	-mkdir build
	$(CC) -o $@ $(REQUIRE) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS) $(INCLUDES)

clean:
	$(RM) -r *.o *.$(GLOBAL_LIB_SUFFIX) build