#Ari Chan
#Make file for Compiler Project

OBJ = lexer.o token.o parser.o microc.o
OPTS = -g -c -Wall -Werror

microc: $(OBJ)
	g++ -o microc $(OBJ)

microc.o: microc.cc token.h lexer.h parser.h
	g++ $(OPTS) microc.cc

parser.o: parser.cc token.h lexer.h parser.h
	g++ $(OPTS) parser.cc

token.o: token.cc token.h
	g++ $(OPTS) token.cc

lexer.o: lexer.cc lexer.h token.h
	g++ $(OPTS) lexer.cc
clean:
	rm -f *.o *~ microc
