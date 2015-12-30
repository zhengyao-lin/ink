ifeq ($(ARCH), arm)
	export ARCH_PREFIX=arm-none-linux-gnueabi-
else
	export ARCH_PREFIX=
endif

LIB_LD_NAME := ink-core
LIB_NAME = lib$(LIB_LD_NAME).so

INSTALL_BIN_PATH := /usr/bin
INSTALL_LIB_PATH := /usr/lib
INSTALL_LIB_NAME := ink
INSTALL_MODULE_PATH := modules

BIN_OUTPUT := bin
LIB_OUTPUT := lib
MOD_OUTPUT := modules

TARGET=$(BIN_OUTPUT)/ink
LIBS=\
	core/$(LIB_NAME)
REQUIRE=\
	syntax/syntax.o \
	msg/msg.o \
	interface/interface.o \
	ink.o

CC=$(ARCH_PREFIX)g++
LD=$(ARCH_PREFIX)ld
CPPFLAGS= -g -Wall -pedantic
LDFLAGS=-Lcore -l$(LIB_LD_NAME)

$(TARGET): $(REQUIRE) $(LIBS)

ifneq ($(BIN_OUTPUT), $(wildcard $(BIN_OUTPUT)))
	mkdir $(BIN_OUTPUT)
endif

ifneq ($(LIB_OUTPUT), $(wildcard $(LIB_OUTPUT)))
	mkdir $(LIB_OUTPUT)
endif

	$(CC) -o $@ $(REQUIRE) $(LDFLAGS)
	cp $(LIBS) $(LIB_OUTPUT)
	cd modules; $(MAKE)

core/$(LIB_NAME):
	cd core; $(MAKE)

syntax/syntax.o:
	cd syntax; $(MAKE)

msg/msg.o:
	cd msg; $(MAKE)

interface/interface.o:
	cd interface; $(MAKE)

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS)

install:
	cp -a $(BIN_OUTPUT)/* $(INSTALL_BIN_PATH)

	mkdir -p $(INSTALL_LIB_PATH)/$(INSTALL_LIB_NAME)
	cp -af $(LIB_OUTPUT)/* $(INSTALL_LIB_PATH)/$(INSTALL_LIB_NAME)
	ln -sf $(INSTALL_LIB_PATH)/$(INSTALL_LIB_NAME)/$(LIB_NAME) $(INSTALL_LIB_PATH)/$(LIB_NAME)

	mkdir -p $(INSTALL_LIB_PATH)/$(INSTALL_LIB_NAME)/$(INSTALL_MODULE_PATH)
	cp -af $(MOD_OUTPUT)/*.mod $(INSTALL_LIB_PATH)/$(INSTALL_LIB_NAME)/$(INSTALL_MODULE_PATH)

clean:
	cd core; $(MAKE) clean
	cd syntax; $(MAKE) clean
	cd msg; $(MAKE) clean
	cd interface; $(MAKE) clean
	cd modules; $(MAKE) clean
	$(RM) -r *.o $(TARGET) $(BIN_OUTPUT) $(LIB_OUTPUT)