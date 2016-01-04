TARGET=build/JSON.so
REQUIRE=\
	json.o

CC=$(ARCH_PREFIX)g++
LD=$(ARCH_PREFIX)ld
CPPFLAGS=-c -fPIC $(GLOBAL_CPPFLAGS)
LDFLAGS=-shared 
# -L$(GLOBAL_LIB_PATH) -l$(GLOBAL_CORE_LIB_NAME)
INCLUDES=-I$(GLOBAL_ROOT_PATH)

$(TARGET): $(REQUIRE)
	-mkdir build
	$(CC) -o $@ $(REQUIRE) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS) $(INCLUDES)

clean:
	$(RM) -r *.o *.so build