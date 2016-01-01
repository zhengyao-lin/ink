TARGET=syntax.o
REQUIRE=\
	lex.o \
	grammar.o

CC=$(ARCH_PREFIX)g++
LD=$(ARCH_PREFIX)ld
CPPFLAGS=-I$(GLOBAL_ROOT_PATH) -fPIC $(GLOBAL_CPPFLAGS)
LDFLAGS=

$(TARGET): $(REQUIRE)
	$(LD) -r -o $@ $(REQUIRE) $(LDFLAGS)

lex.cpp: lex.l grammar.hpp
	lex -o $@ $^

#parser
grammar.cpp: grammar.y
	yacc -dv -o $@ $^
grammar.hpp: grammar.cpp

%.o: %.cpp
	$(CC) -c $^ $(CPPFLAGS)

clean:
	$(RM) *.o *.output grammar.cpp grammar.hpp lex.cpp