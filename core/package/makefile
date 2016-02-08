TARGET=package.o
REQUIRE=\
	load.o \
	module.o

CPPFLAGS=-I$(GLOBAL_ROOT_PATH) -fPIC $(GLOBAL_CPPFLAGS)
LDFLAGS=

$(TARGET): $(REQUIRE)
	$(LD) -r -o $@ $(REQUIRE) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS)

clean:
	$(RM) *.o