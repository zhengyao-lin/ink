ifeq ($(ARCH), arm)
	export ARCH_PREFIX=arm-none-linux-gnueabi-
else
	export ARCH_PREFIX=
endif

include makefile.head

LIB_LD_NAME = $(GLOBAL_CORE_LIB_NAME)
LIB_NAME = $(GLOBAL_CORE_LIB_SONAME)

INSTALL_BIN_PATH = $(GLOBAL_INSTALL_PATH)/bin
INSTALL_LIB_PATH = $(GLOBAL_INSTALL_PATH)/lib
INSTALL_LIB_NAME = ink
INSTALL_MODULE_PATH = modules

BIN_OUTPUT = bin
LIB_OUTPUT = lib
MOD_OUTPUT = modules

TARGET=$(BIN_OUTPUT)/ink
STATIC_TARGET=$(BIN_OUTPUT)/ink_static
LIBS = core/$(LIB_NAME)
REQUIRE = ink.o

CC=$(ARCH_PREFIX)g++
LD=$(ARCH_PREFIX)ld

CPPFLAGS=$(GLOBAL_CPPFLAGS)
STATIC_CPPFLAGS=$(CPPFLAGS) $(GLOBAL_STATIC_CPPFLAGS)

LDFLAGS=-Lcore -l$(LIB_LD_NAME)
STATIC_LDFLAGS=-ldl -pthread

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
	$(RM) -r *.o $(TARGET) $(BIN_OUTPUT) $(LIB_OUTPUT)

FORCE: