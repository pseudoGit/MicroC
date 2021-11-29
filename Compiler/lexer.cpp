/*
  Ari Chan
  Program Description: A Lexer object takes an input stream and makes tokens
*/

#include <cctype>
#include <string>
#include "lexer.h"

using namespace std;

Lexer::Lexer(istream& input) : input(input) {
  pos = 0;
  line = 1;
  currChar = ' ';
}

Lexer::Lexer() : input(cin) {
  pos = 0;
  line = 1;
  currChar = ' ';
}

// Intentionally empty Lexer object destructor
Lexer::~Lexer() {
}

/*
  Creates tokens based on predefined types
  Calls nextChar() to retrieve characters and builds a lexeme
  Skips spaces and loads next character
*/
Token Lexer::nextToken() {
  string lexeme = "";
  int tokenPos = pos; // The beginning of a token
  while (currChar != '#') { // Loops while not end of file
    if (isspace(currChar)) { // Skip spaces
      currChar = nextChar();
    }
    else if (isalpha(currChar)) { // Keywords and variable names
      while (isalnum(currChar)) {
        lexeme = lexeme + currChar;
        currChar = nextChar();
      }
      if (!lexeme.compare("if")) {
        Token token(Token::IF, lexeme, line, tokenPos);
        return token;
      }
      else if (!lexeme.compare("else")) {
        Token token(Token::ELSE, lexeme, line, tokenPos);
        return token;
      }
      else if (!lexeme.compare("while")) {
        Token token(Token::WHILE, lexeme, line, tokenPos);
        return token;
      }
      else if (!lexeme.compare("function")) {
        Token token(Token::FUNCTION, lexeme, line, tokenPos);
        return token;
      }
      else if (!lexeme.compare("var")) {
        Token token(Token::VAR, lexeme, line, tokenPos);
        return token;
      }
      else if (!lexeme.compare("printf")) {
        Token token(Token::PRINTF, lexeme, line, tokenPos);
        return token;
      }
      else if (!lexeme.compare("return")) {
        Token token(Token::RETURN, lexeme, line, tokenPos);
        return token;
      }
      else {
        Token token(Token::IDENT, lexeme, line, tokenPos);
        return token;
      }
    }
    else if (isdigit(currChar)) { // LITERAL INTS
      lexeme = lexeme + currChar;
      currChar = nextChar();
      if (!isdigit(currChar)) {
        Token token(Token::INTLIT, lexeme, line, tokenPos);
        return token;
      }
    }
    else if (currChar == '+') { // SPECIAL INSTRUCTIONS
      lexeme = lexeme + currChar;
      currChar = nextChar();
      if (!isspecial(currChar)) {
        Token token(Token::PLUS, lexeme, line, tokenPos);
        return token;
      }
    }
    else if (currChar == '-') {
      lexeme = lexeme + currChar;
      currChar = nextChar();
      if (!isspecial(currChar)) {
        Token token(Token::MINUS, lexeme, line, tokenPos);
        return token;
      }
    }
    else if (currChar == '*') {
      lexeme = lexeme + currChar;
      currChar = nextChar();
      if (!isspecial(currChar)) {
        Token token(Token::TIMES, lexeme, line, tokenPos);
        return token;
      }
    }
    else if (currChar == '/') {
      lexeme = lexeme + currChar;
      currChar = nextChar();
      if (!isspecial(currChar)) {
        Token token(Token::DIVIDE, lexeme, line, tokenPos);
        return token;
      }
    }
    else if (currChar == '=') {
      lexeme = lexeme + currChar;
      currChar = nextChar();
      if (isspecial(currChar)) {
        if (currChar == '='){
          lexeme = lexeme + currChar;
          currChar = nextChar();
          Token token(Token::EQ, lexeme, line, tokenPos);
          return token;
        }
        else {
          Token token(Token::ERROR, lexeme, line, tokenPos);
          return token;
        }
      }
      else {
        Token token(Token::ASSIGN, lexeme, line, tokenPos);
        return token;
      }
    }
    else if (currChar == '!') {
      lexeme = lexeme + currChar;
      currChar = nextChar();
      if (isspecial(currChar)) {
        if (currChar == '=') {
          lexeme = lexeme + currChar;
          currChar = nextChar();
          Token token(Token::NE, lexeme, line, tokenPos);
          return token;
        }
        else {
          Token token(Token::ERROR, lexeme, line, tokenPos);
          return token;
        }
      }
      else {
        Token token(Token::ERROR, lexeme, line, tokenPos);
        return token;
      }
    }
    else if (currChar == '<') {
      lexeme = lexeme + currChar;
      currChar = nextChar();
      if (isspecial(currChar)) {
        if (currChar == '=') {
          lexeme = lexeme + currChar;
          currChar = nextChar();
          Token token(Token::LE, lexeme, line, tokenPos);
          return token;
        }
        else {
          Token token(Token::ERROR, lexeme, line, tokenPos);
          return token;
        }
      }
      else {
        Token token(Token::LT, lexeme, line, tokenPos);
        return token;
      }
    }
    else if (currChar == '>') {
      lexeme = lexeme + currChar;
      currChar = nextChar();
      if (isspecial(currChar)) {
        if (currChar == '=') {
          lexeme = lexeme + currChar;
          currChar = nextChar();
          Token token(Token::GE, lexeme, line, tokenPos);
          return token;
        }
      }
      else {
        Token token(Token::GT, lexeme, line, tokenPos);
        return token;
      }
    }
    else if (currChar == '&') {
      lexeme = lexeme + currChar;
      currChar = nextChar();
      if (currChar == '&') {
        lexeme = lexeme + currChar;
        currChar = nextChar();
        Token token(Token::AND, lexeme, line, tokenPos);
        return token;
      }
      else {
        Token token(Token::ERROR, lexeme, line, tokenPos);
        return token;
      }
    }
    else if (currChar == '|') {
      lexeme = lexeme + currChar;
      currChar = nextChar();
      if (currChar == '|') {
        lexeme = lexeme + currChar;
        currChar = nextChar();
        Token token(Token::OR, lexeme, line, tokenPos);
        return token;
      }
      else {
        Token token(Token::ERROR, lexeme, line, tokenPos);
        return token;
      }
    }
    else if (currChar == '(') {
      lexeme = lexeme + currChar;
      currChar = nextChar();
      Token token(Token::LPAREN, lexeme, line, tokenPos);
      return token;
    }
    else if (currChar == ')') {
      lexeme = lexeme + currChar;
      currChar = nextChar();
      Token token(Token::RPAREN, lexeme, line, tokenPos);
      return token;
    }
    else if (currChar == '{') {
      lexeme = lexeme + currChar;
      currChar = nextChar();
      if (!isspecial(currChar)) {
        Token token(Token::LBRACE, lexeme, line, tokenPos);
        return token;
      }
    }
    else if (currChar == '}') {
      lexeme = lexeme + currChar;
      currChar = nextChar();
      if (!isspecial(currChar)) {
        Token token(Token::RBRACE, lexeme, line, tokenPos);
        return token;
      }
    }
    else if (currChar == ',') {
      lexeme = lexeme + currChar;
      currChar = nextChar();
      if (!isspecial(currChar)) {
        Token token(Token::COMMA, lexeme, line, tokenPos);
        return token;
      }
    }
    else if (currChar == ';') {
      lexeme = lexeme + currChar;
      currChar = nextChar();
      if (!isspecial(currChar)) {
        Token token(Token::SEMICOLON, lexeme, line, tokenPos);
        return token;
      }
    }
    else if (currChar == '"') {
      lexeme = lexeme + currChar;
      currChar = nextChar(); 
      while (currChar != '"') { // Reads until another double quote is found
        lexeme = lexeme + currChar;
        currChar = nextChar(); 
      }
      lexeme = lexeme + currChar; // Adds the double quote
      currChar = nextChar();
      Token token(Token::STRINGLIT, lexeme, line, tokenPos);     
      return token;
    }
    else { // Errors
      Token token(Token::ERROR, lexeme, line, tokenPos);
      return token;
    }
  }
  // End of file
  Token token(Token::ENDOFFILE, lexeme, line, tokenPos);
  return token;
}

// Reads and returns a character from istream; special cases for EOF and new line
char Lexer::nextChar() {
  char ch = input.get();
  if (input.eof()) {
    return '#';
  }
  else if (ch == '\n') {
    pos = 0;
    line++;
    return ' ';
  }
  else {
    pos++; 
    return ch;
  }
}

// Checks if a character is a special character
int Lexer::isspecial(char ch) {
  if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == '!' || ch == '<' || ch == '>' || ch == '&' || ch == '|' || ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == ',' || ch == ';') {
    return 1;
  }
  else {
    return 0;
  }
}
