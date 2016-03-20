TARGET=syntax.o
REQUIRE=\
	lex.o \
	grammar.o \
	parser.o

LDFLAGS=

ifeq ($(GLOBAL_PLATFORM), windows)
	CPPFLAGS=-I$(GLOBAL_ROOT_PATH) $(GLOBAL_CPPFLAGS)
else
	CPPFLAGS=-I$(GLOBAL_ROOT_PATH) -fPIC $(GLOBAL_CPPFLAGS)
endif

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