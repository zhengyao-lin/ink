TARGET=build/JSON.$(GLOBAL_LIB_SUFFIX)
REQUIRE=\
	json.o \
	parser.o

LDFLAGS=-shared -static-libgcc -static-libstdc++ -L$(GLOBAL_LIB_PATH) -l$(GLOBAL_CORE_LIB_NAME)
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
	$(RM) -r *.o *.$(GLOBAL_LIB_SUFFIX) build