TARGET=coroutine.o

ifeq ($(GLOBAL_PLATFORM), windows)
	REQUIRE=fiber.o
	CPPFLAGS=-I$(GLOBAL_ROOT_PATH) $(GLOBAL_CPPFLAGS)
else
	REQUIRE=ucontext.o
	CPPFLAGS=-I$(GLOBAL_ROOT_PATH) -fPIC $(GLOBAL_CPPFLAGS)
endif

LDFLAGS=

$(TARGET): $(REQUIRE)
	$(LD) -r -o $@ $(REQUIRE) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS)

clean:
	$(RM) *.o