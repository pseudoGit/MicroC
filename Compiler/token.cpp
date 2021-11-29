/*
  Ari Chan
  Program Description: Implementation of a token which contains four fields
*/

#include <cstddef>
#include "token.h"

Token::Token(int type, string lexeme, int line, int pos) : tokType(type), tokLexeme(lexeme), tokLine(line), tokPos(pos) {
}

Token::Token() : tokType(0), tokLexeme(""), tokLine(0), tokPos(0) {
}

// Intentionally empty Token object destructor
Token::~Token() {
}

