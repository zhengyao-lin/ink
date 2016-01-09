TARGET=build/JSON.$(GLOBAL_LIB_SUFFIX)
REQUIRE=\
	json.o \
	parser.o

CC=$(ARCH_PREFIX)g++
LD=$(ARCH_PREFIX)ld
CPPFLAGS=-c -fPIC $(GLOBAL_CPPFLAGS)
LDFLAGS=-shared -static-libgcc -static-libstdc++
ifeq ($(GLOBAL_PLATFORM), windows)
	LDFLAGS+= -L$(GLOBAL_LIB_PATH) -l$(GLOBAL_CORE_LIB_NAME)
endif
INCLUDES=-I$(GLOBAL_ROOT_PATH)

$(TARGET): $(REQUIRE)
	-mkdir build
	$(CC) -o $@ $(REQUIRE) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS) $(INCLUDES)

clean:
	$(RM) -r *.o *.$(GLOBAL_LIB_SUFFIX) build