TARGET=thread.o
REQUIRE=\
	general.o \
	actor.o

LDFLAGS=

ifeq ($(GLOBAL_PLATFORM), windows)
	CPPFLAGS=-I$(GLOBAL_ROOT_PATH) $(GLOBAL_CPPFLAGS)
else
	CPPFLAGS=-I$(GLOBAL_ROOT_PATH) -fPIC $(GLOBAL_CPPFLAGS)
endif

$(TARGET): $(REQUIRE)
	$(LD) -r -o $@ $(REQUIRE) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS)

clean:
	$(RM) *.o