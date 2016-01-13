ifeq ($(ARCH), arm)
	export ARCH_PREFIX=arm-none-linux-gnueabi-
else
	export ARCH_PREFIX=
endif

include makefile.head

ifeq ($(PLATFORM), win64)
	export GLOBAL_PLATFORM = windows
	ifneq ($(GLOBAL_PLATFORM_NAME), MINGW32_NT-6.2)
		export ARCH_PREFIX = x86_64-w64-mingw32-
	endif
	export GLOBAL_LIB_SUFFIX = dll
	export GLOBAL_PLATFORM_ARCH = x64
else
	ifeq ($(PLATFORM), win32)
		export GLOBAL_PLATFORM = windows
		ifneq ($(GLOBAL_PLATFORM_NAME), MINGW32_NT-6.2)
			export ARCH_PREFIX = i686-w64-mingw32-
		endif
		export GLOBAL_LIB_SUFFIX = dll
		export GLOBAL_PLATFORM_ARCH = x86
	endif
endif

LIB_LD_NAME = $(GLOBAL_CORE_LIB_NAME)
LIB_NAME = $(GLOBAL_CORE_LIB_SONAME)

INSTALL_BIN_PATH = $(GLOBAL_INSTALL_PATH)/bin
INSTALL_LIB_PATH = $(GLOBAL_INSTALL_PATH)/lib
INSTALL_LIB_NAME = ink
INSTALL_MODULE_PATH = modules

export BIN_OUTPUT = bin
export LIB_OUTPUT = lib
export WIN_OUTPUT = output
ifeq ($(GLOBAL_PLATFORM), windows)
	export BIN_OUTPUT = $(WIN_OUTPUT)
	export LIB_OUTPUT = $(WIN_OUTPUT)
endif
MOD_OUTPUT = modules

TARGET=$(BIN_OUTPUT)/ink
STATIC_TARGET=$(BIN_OUTPUT)/ink_static
LIBS = core/$(LIB_NAME)
REQUIRE = ink.o

CC=$(ARCH_PREFIX)g++
LD=$(ARCH_PREFIX)ld

CPPFLAGS=$(GLOBAL_CPPFLAGS)
STATIC_CPPFLAGS=$(CPPFLAGS) $(GLOBAL_STATIC_CPPFLAGS)

LDFLAGS=-Lcore -l$(LIB_LD_NAME) -static-libgcc -static-libstdc++
STATIC_LDFLAGS=-static

ifneq ($(GLOBAL_PLATFORM), windows)
	LDFLAGS+= -ldl
	STATIC_LDFLAGS+= -ldl -pthread
else
	STATIC_LDFLAGS+= -Wl,-Bstatic -lwinpthread
endif

CREATE_OUTPUT_DIR = \
	if [ ! -d $(BIN_OUTPUT) ]; then \
	  mkdir $(BIN_OUTPUT); \
	fi; \
	if [ ! -d $(LIB_OUTPUT) ]; then \
	  mkdir $(LIB_OUTPUT); \
	fi

all: main_prog apps modules

main_prog: $(TARGET) static
	$(CREATE_OUTPUT_DIR)
	cp $(LIBS) $(LIB_OUTPUT)

$(TARGET): $(REQUIRE) $(LIBS)
	$(CREATE_OUTPUT_DIR)
	$(CC) -o $@ $(REQUIRE) $(LDFLAGS)

static: $(REQUIRE) $(GLOBAL_CORE_STATIC_LIB_PATH)
	$(CREATE_OUTPUT_DIR)
	$(CC) -o $(STATIC_TARGET) $(REQUIRE) $(GLOBAL_CORE_STATIC_LIB_PATH) $(STATIC_LDFLAGS)

$(GLOBAL_CORE_STATIC_LIB_PATH):
	cd core; $(MAKE) static

core/$(LIB_NAME):
	cd core; $(MAKE)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS)

apps: main_prog FORCE
	cd apps; $(MAKE)

modules: main_prog FORCE
	cd modules; $(MAKE)

output: FORCE
	mkdir -p $(WIN_OUTPUT)/$(INSTALL_MODULE_PATH)
	cp -af modules/*.dll $(WIN_OUTPUT)/$(INSTALL_MODULE_PATH)
	# cp -af $(GLOBAL_ROOT_PATH)/third_parties/winpthread/$(GLOBAL_PLATFORM_ARCH)/* $(WIN_OUTPUT)

install:
	cp -a $(BIN_OUTPUT)/* $(INSTALL_BIN_PATH)

	mkdir -p $(INSTALL_LIB_PATH)/$(INSTALL_LIB_NAME)
	cp -af $(LIB_OUTPUT)/* $(INSTALL_LIB_PATH)/$(INSTALL_LIB_NAME)
	ln -sf $(INSTALL_LIB_PATH)/$(INSTALL_LIB_NAME)/$(LIB_NAME) $(INSTALL_LIB_PATH)/$(LIB_NAME)

	mkdir -p $(INSTALL_LIB_PATH)/$(INSTALL_LIB_NAME)/$(INSTALL_MODULE_PATH)
	-cp -af $(MOD_OUTPUT)/*.so $(INSTALL_LIB_PATH)/$(INSTALL_LIB_NAME)/$(INSTALL_MODULE_PATH)

clean:
	cd core; $(MAKE) clean
	cd modules; $(MAKE) clean
	cd apps; $(MAKE) clean
	$(RM) -r *.o $(TARGET) $(BIN_OUTPUT) $(LIB_OUTPUT) $(WIN_OUTPUT)

FORCE: