LIB_NAME=$(GLOBAL_CORE_LIB_SONAME)
TARGET=$(LIB_NAME)
REQUIRE=\
	gc/gc.o \
	native/native.o \
	thread/thread.o \
	coroutine/coroutine.o \
	package/package.o \
	msg/msg.o \
	interface/interface.o \
	hash.o \
	object.o \
	expression.o \
	context.o \
	general.o \
	debug.o \
	error.o

CC=$(ARCH_PREFIX)g++
LD=$(ARCH_PREFIX)ld
CPPFLAGS=-I../ -g -Wall -pedantic -fPIC
LDFLAGS=-shared -ldl -pthread

$(TARGET): $(REQUIRE)
	$(CC) -o $@ $(REQUIRE) $(LDFLAGS)

gc/gc.o:
	cd gc; $(MAKE)

native/native.o:
	cd native; $(MAKE)

thread/thread.o:
	cd thread; $(MAKE)

coroutine/coroutine.o:
	cd coroutine; $(MAKE)

package/package.o:
	cd package; $(MAKE)

msg/msg.o:
	cd msg; $(MAKE)

interface/interface.o:
	cd interface; $(MAKE)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS)

clean:
	cd gc; $(MAKE) clean
	cd native; $(MAKE) clean
	cd thread; $(MAKE) clean
	cd coroutine; $(MAKE) clean
	cd package; $(MAKE) clean
	cd msg; $(MAKE) clean
	cd interface; $(MAKE) clean
	$(RM) *.o *.so