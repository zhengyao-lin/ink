TARGET=build/readline.$(GLOBAL_LIB_SUFFIX)
REQUIRE=\
	readline.o

CPPFLAGS=-c -fPIC $(GLOBAL_CPPFLAGS)
LDFLAGS=-shared -static-libgcc -static-libstdc++ -lreadline -ltermcap -L$(GLOBAL_LIB_PATH) -l$(GLOBAL_CORE_LIB_NAME)
INCLUDES=-I$(GLOBAL_ROOT_PATH)

$(TARGET): $(REQUIRE)
	-mkdir build
	$(CC) -o $@ $(REQUIRE) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS) $(INCLUDES)

clean:
	$(RM) -r *.o *.$(GLOBAL_LIB_SUFFIX) build