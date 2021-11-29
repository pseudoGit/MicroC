/*
  Ari Chan
  Program Description: Header for a Lexer object
*/

#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include "token.h"

class Lexer {
public:
  // Constructors
  Lexer(istream& input);
  Lexer(); // Default

  // Destructor
  ~Lexer();

  // Member function
  Token nextToken(); // Returns Token object and places next token into input stream

private:
  // Fields
  istream& input; // The source of input 
  int pos; // Current position being read
  int line; // Current line being read
  char currChar; // Current char being read

  // Member function
  char nextChar(); // Reads and return the next character, except for EOF and '\n'
  int isspecial(char ch); // Checks for special character
};

#endif

