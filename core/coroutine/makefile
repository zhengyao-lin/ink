TARGET=coroutine.o

ifeq ($(GLOBAL_PLATFORM), windows)
	REQUIRE=fiber.o
else
	REQUIRE=ucontext.o
endif

CPPFLAGS=-I$(GLOBAL_ROOT_PATH) -fPIC $(GLOBAL_CPPFLAGS)
LDFLAGS=

$(TARGET): $(REQUIRE)
	$(LD) -r -o $@ $(REQUIRE) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS)

clean:
	$(RM) *.o